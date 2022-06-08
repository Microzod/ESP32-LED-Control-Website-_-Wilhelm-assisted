/*
Copyright (C) 2021 wk & david
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, version 3.
The above copyright notice, this permission notice and the word "NIGGER" shall be included in all copies or substantial portions of the Software.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>
*/
// 
// Nu funkar allt på kretskortet förutom DAC'en!
// 

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "SPIFFS.h"
#include "FS.h"
#include <ArduinoJson.h>
#include <stdlib.h>         // function used: int atoi (const char * str);

#include <LTC2633Library.h>
#include <colorTemperature.h>
#include <TimeLib.h>
#include <TimeAlarms.h>
#include "NTP_stuff.h"
#include "OTA.h"
#include "credentials.h"
#include "esp32_pins.h"
#include "functions.h"
#include <IRremote.hpp>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/timer.h"
#include "esp_timer.h"

#include "esp32-hal-timer.h"
#include "freertos/xtensa_api.h"
#include "rom/ets_sys.h"
#include "soc/timer_group_struct.h"
#include "soc/dport_reg.h"
#include "esp_attr.h"
#include "esp_intr.h"


//CorrelatedColorTemperature LED(26, 27, LTC2633_CA0_VCC, &Wire);

typedef struct
{        
    String startOfDay;
    String endOfDay;
    double sunriseLength;
    double sunsetLength;
    
    time_t startOfDay_t;
    time_t endOfDay_t;
    time_t sunriseLength_t;
    time_t sunsetLength_t;
    uint16_t cct;
    uint16_t intensityControl;
    
    uint8_t sunlight_isOnOrOff;
    uint8_t startOfDay_isAutoActive;
    uint8_t endOfDay_isAutoActive;
    
    bool newValue;
    
} serverDataStruct;

typedef struct
{
    time_t startOfDay_t;
    time_t endOfDay_t;
    time_t sunriseLength_t;
    time_t sunsetLength_t;
    uint16_t cct;
    uint16_t intensityControl;
    
    uint8_t sunlight_isOnOrOff;
    uint8_t startOfDay_isAutoActive;
    uint8_t endOfDay_isAutoActive;
    
    uint8_t triggerSunrise;
    uint8_t triggerSunset;
    
    bool newValue;
    
} dataResultStruct;

serverDataStruct espReceiveData_struct = {"0", "0", 0};
dataResultStruct espResult_struct = {0};

String hostname = "esp.lan";
WebServer server(80);

// Do you need to create & format a filesystem
#define FILESYSTEM SPIFFS
// You only need to format the filesystem once
#define FORMAT_FILESYSTEM true
//#define Serial Serial

File fsUploadFile;
//String status;
//String ramp;

// David functions:
void compareData(serverDataStruct *s, dataResultStruct *result);
void serialPrintDataStruct(serverDataStruct *s);
void getData(serverDataStruct *s);
//void parseData(serverDataStruct *s);

// Wille functions:
void entryPoint( );
void serverSetup();
bool exists(String path);
void writeArray();
void readArray();
void requestInfo();
bool handleFileRead(String path);
void handleNotFound();
void handleRoot();
void writeFile(const char * path, String message);
void appendFile(const char * path, const char * message);

DynamicJsonDocument doc(1024);
JsonObject obj;

void setup()
{
    LED.begin(1, 2);
    
    pinMode(BUTTON_pin, INPUT);
    pinMode(IR_OUT_pin, INPUT);
    pinMode(RF_REMOTE_pin, INPUT);
    pinMode(LED1_pin, OUTPUT);
        digitalWrite(LED1_pin, LOW);

    for (int i = 0; i < 5; i++)
    {
        digitalWrite(LED1_pin, HIGH);
        delay(250);
        digitalWrite(LED1_pin, LOW);
        delay(250);
    }
    
    digitalWrite(LED1_pin, HIGH);
    
    
    
    Serial.begin(115200);
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE)); // Print which program is running.
    Serial.setDebugOutput(true);
    //pinMode(GPIO15, INPUT);

    serverSetup();
}

void loop()
{
    server.handleClient();
    delay(2);
}

// This function should be used to interface with the rest of the system
void entryPoint()
{
    readArray();    // load the config file in order to access stored values
    
    getData(&espReceiveData_struct);
    compareData(&espReceiveData_struct, &espResult_struct);
    //serialPrintDataStruct(&espResult_struct);
}

void getData(serverDataStruct *s)
{
    File file = FILESYSTEM.open("/config.json");
    char data[1024];
    
    while(file.available())
    {
        file.readBytes(data,sizeof(data));
    }
    
    file.close();
    
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, data);
    JsonObject obj = doc.as<JsonObject>();
    
    
    // Extract the values from the JsonObject value-pairs, using the name of the pair:
    String startOfDay =          obj["startOfDay"];      s->startOfDay = startOfDay; // Weird work-around, Wille, om du ser detta får du gärna ändra det till const char* istället för String.
    String endOfDay =            obj["endOfDay"];        s->endOfDay = endOfDay;     // Weird work-around, Wille, om du ser detta får du gärna ändra det till const char* istället för String.
    s->sunriseLength =           obj["sunriseLength"];
    s->sunsetLength =            obj["sunsetLength"];
    s->cct =                     obj["cct"];
    s->intensityControl =        obj["intensityControl"];
    s->sunlight_isOnOrOff =      obj["p"];
    s->startOfDay_isAutoActive = obj["startOfDayCheck"];
    s->endOfDay_isAutoActive =   obj["endOfDayCheck"];
    
    
    // Convert the startOfDay String with the format 'hh:mm' into a time_t variable to give the number of seconds:
    char tempArray[2];
    int i;
    time_t Hours;
    time_t Minutes;
    tempArray[0] = s->startOfDay[0];
    tempArray[1] = s->startOfDay[1];
    i = atoi(tempArray);
    Hours = (time_t)i;
    
    tempArray[0] = s->startOfDay[3];
    tempArray[1] = s->startOfDay[4];
    i = atoi(tempArray);
    Minutes = (time_t)i;
    
    s->startOfDay_t = (60 * Minutes) + (3600 * Hours);
    
    
    // Convert the endOfDay String with the format 'hh:mm' into a time_t variable to give the number of seconds:
    tempArray[0] = s->endOfDay[0];
    tempArray[1] = s->endOfDay[1];
    i = atoi(tempArray);
    Hours = (time_t)i;
    
    tempArray[0] = s->endOfDay[3];
    tempArray[1] = s->endOfDay[4];
    i = atoi(tempArray);
    Minutes = (time_t)i;
    
    s->endOfDay_t = (60 * Minutes) + (3600 * Hours);
    
    
    // Convert sunriseLength & sunsetLength from minutes into seconds:
    s->sunriseLength_t = (60.0 * s->sunriseLength);
    s->sunsetLength_t = (60.0 * s->sunsetLength);
}

void compareData(serverDataStruct *s, dataResultStruct *result)
{
    if (!(s->startOfDay_t == result->startOfDay_t))
    {
        result->newValue = true;
    }
    else if (!(s->endOfDay_t == result->endOfDay_t))
    {
        result->newValue = true;
    }
    else if (!(s->sunriseLength_t == result->sunriseLength_t))
    {
        result->newValue = true;
    }
    else if (!(s->sunsetLength_t == result->sunsetLength_t))
    {
        result->newValue = true;
    }
    else if (!(s->cct == result->cct))
    {
        result->newValue = true;
    }
    else if (!(s->intensityControl == result->intensityControl))
    {
        result->newValue = true;
    }
    else if (!(s->sunlight_isOnOrOff == result->sunlight_isOnOrOff))
    {
        result->newValue = true;
    }
    else if (!(s->startOfDay_isAutoActive == result->startOfDay_isAutoActive))
    {
        result->newValue = true;
    }
    else if (!(s->endOfDay_isAutoActive == result->endOfDay_isAutoActive))
    {
        result->newValue = true;
    }
    else
    {
        result->newValue = false;
    }
    
    
    if (result->newValue)
    {
        result->startOfDay_t            = s->startOfDay_t;
        result->endOfDay_t              = s->endOfDay_t;
        result->sunriseLength_t         = s->sunriseLength_t;
        result->sunsetLength_t          = s->sunsetLength_t;
        result->cct                     = s->cct;
        result->intensityControl        = s->intensityControl;
        
        result->sunlight_isOnOrOff      = s->sunlight_isOnOrOff;
        result->startOfDay_isAutoActive = s->startOfDay_isAutoActive;
        result->endOfDay_isAutoActive   = s->endOfDay_isAutoActive;
        
        serialPrintDataStruct(&espResult_struct);
        
        
        // Use new values for LED settings:
        LED.setSunriseAlarmTime(result->startOfDay_t);
        LED.setSunriseDuration(result->sunriseLength_t);
        if (result->startOfDay_isAutoActive)
            LED.enableSunriseAlarm();
        else
            LED.disableSunriseAlarm();
        
        
        LED.setSunsetAlarmTime(result->endOfDay_t);
        LED.setSunsetDuration(result->sunsetLength_t);
        if (result->endOfDay_isAutoActive)
            LED.enableSunsetAlarm();
        else
            LED.disableSunsetAlarm();
        
        LED.setPowerState(result->sunlight_isOnOrOff);
        LED.setColorTemperature(result->cct);
        LED.setIntensity(result->intensityControl);
        LED.writeToLed();
        
        result->newValue = false;
    }
    
    //int round_step_s = (s->intensityStep + 0.5);  int round_step_result = (result->intensityStep + 0.5);
    //int round_s = int(s->intensityControl + 0.5); int round_result = int(result->intensityControl + 0.5);
    //if (!(round_step_s == round_step_result))
    //    result->newValue = true;
    //
    //if (!(round_s == round_result))
    //    result->newValue = true;
}

void serialPrintDataStruct(dataResultStruct *s)
{
    time_t startOfDay_total = s->startOfDay_t;
    time_t startOfDay_minutes = startOfDay_total / 60;
    time_t startOfDay_seconds = startOfDay_total % 60;
    time_t startOfDay_hours = startOfDay_minutes / 60;
    startOfDay_minutes = startOfDay_minutes % 60;

    time_t endOfDay_total = s->endOfDay_t;
    time_t endOfDay_minutes = endOfDay_total / 60;
    time_t endOfDay_seconds = endOfDay_total % 60;
    time_t endOfDay_hours = endOfDay_minutes / 60;
    endOfDay_minutes = endOfDay_minutes % 60;
    
    Serial.println("Data parsed from server json file:");
    
    Serial.print("Start of day(seconds): ");
    Serial.print(s->startOfDay_t);
    Serial.println(" s");
    Serial.print("Start of day(hh:mm:ss): ");
    if (startOfDay_hours < 10)
        Serial.print("0");
    Serial.print(startOfDay_hours);
    Serial.print(":");
    if (startOfDay_minutes < 10)
        Serial.print("0");
    Serial.print(startOfDay_minutes);
    Serial.print(":");
    if (startOfDay_seconds < 10)
        Serial.print("0");
    Serial.println(startOfDay_seconds);
    
    Serial.print("End of day(seconds): ");
    Serial.print(s->endOfDay_t);
    Serial.println(" s");
    Serial.print("End of day(hh:mm:ss) - ");
    if (endOfDay_hours < 10)
        Serial.print("0");
    Serial.print(endOfDay_hours);
    Serial.print(":");
    if (endOfDay_minutes < 10)
        Serial.print("0");
    Serial.print(endOfDay_minutes);
    Serial.print(":");
    if (endOfDay_seconds < 10)
        Serial.print("0");
    Serial.println(endOfDay_seconds);
    
    Serial.print("Sunrise Length: ");
    Serial.println(s->sunriseLength_t);
    
    Serial.print("Sunset Length: ");
    Serial.println(s->sunsetLength_t);
    
    Serial.print("Correlated Color Temperature(degrees Kelvin): ");
    Serial.print(s->cct);
    Serial.println("°K");
    
    Serial.print("Intensity Control Level(0-4095): ");
    Serial.println(s->intensityControl);

    if (s->sunlight_isOnOrOff)
        Serial.println("The light is turned ON");
    else
        Serial.println("The light is turned OFF");
    
    if (s->startOfDay_isAutoActive)
        Serial.println("Morning alarm is active(1)");
    else
        Serial.println("Morning alarm is inactive(0)");
    
    if (s->endOfDay_isAutoActive)
        Serial.println("Evening alarm is active(1)");
    else
        Serial.println("Evening alarm is inactive(0)");
}

void serverSetup()
{
    Serial.println("Starting filesystem");
    
    FILESYSTEM.begin();

        
    if(FORMAT_FILESYSTEM)
    {
        FILESYSTEM.format();
        writeFile("/index.htm", indexData);
    }
       
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(mySSID);

    WiFi.begin(mySSID, myPASSWORD);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname(hostname.c_str());

    while(WiFi.status() != WL_CONNECTED)
    {
        delay(100);
        Serial.print(".");
    }

    
    Serial.println("\nConnected!");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Hostname: ");
    Serial.println(hostname);
        
    if(MDNS.begin("esp32"))
    {
        Serial.println("MDNS responder started");
    }

    server.on("/", HTTP_GET, handleRoot);

    server.onNotFound([]()
    {
        Serial.println(server.uri());
        if (!handleFileRead(server.uri()))
        {
            server.send(404, "text/plain", "404");
        }
    });

    server.begin();
    Serial.println("HTTP server started");
    //setupOTA("ESP32-black-stand-offs", mySSID, myPASSWORD);
    setupNTP();
}

bool exists(String path)
{
    bool yes = false;
    File file = FILESYSTEM.open(path, "r");

    if(!file.isDirectory())
    {
        yes = true;
    }

    file.close();
    return yes;
}

void writeArray()
{
    DynamicJsonDocument obj(1024);
    String data;

    for(uint8_t i = 0; i < server.args(); i++)
    {
        obj[server.argName(i)] = server.arg(i);
    }

    serializeJson(obj, data);
    writeFile("/config.json", data);
    
    data = "";
    serializeJsonPretty(obj, data);
    writeFile("/config_pretty.json", data);
    //readArray();
}

void readArray()
{
    File file = FILESYSTEM.open("/config.json");
    char data[1024];

    while(file.available())
    {
        file.readBytes(data,sizeof(data));
    }
    
    file.close();
    //Serial.println(data);

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, data);
    //JsonObject obj = doc.as<JsonObject>();
    obj = doc.as<JsonObject>();
    //setConfigData(obj);
}

void requestInfo()
{
    Serial.println("REQUEST INFO");
    Serial.println("Method: " + (server.method() == HTTP_GET) ? "GET" : "POST");
    Serial.println("Arguments: ");
    
    for(uint8_t i = 0; i < server.args(); i++)
    {
        Serial.println(server.argName(i) + ": " + server.arg(i));
    }
}

bool handleFileRead(String path)
{
    Serial.println("handleFileRead: " + path);

    if(server.args() > 0)
    {
        //requestInfo();
        writeArray();
        
        // After form-data written to config JSON.
        entryPoint();
    }

    //requestInfo();
    
    if(path.endsWith("/"))
    {
        path += "index.htm";
    }

    String contentType = getContentType(path);

    if(exists(path))
    {
        File file = FILESYSTEM.open(path, "r");
        server.streamFile(file, contentType);
        file.close();
        
        return true;
    }
    
    return false;
}

void handleNotFound()
{
    String message = "<h1>404</h1><span>File was not found!</span>\n\n";
    server.send(404, "text/plain", message);
}

void handleRoot()
{
    if(!exists("/index.htm"))
    {
        server.send(200, "text/html", indexData);
    } else {
        handleFileRead("/index.htm");
    }
}

void writeFile(const char * path, String message)
{
    File file = FILESYSTEM.open(path, FILE_WRITE);
    
    if(!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }

    if(file.print(message))
    {
        Serial.print("Wrote to: ");
        Serial.println(path);
    } else
    {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(const char * path, const char * message)
{
    Serial.printf("Appending to file: %s\n", path);

    File file = FILESYSTEM.open(path, FILE_APPEND);
    if(!file)
    {
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message))
    {
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }

    file.close();
}
