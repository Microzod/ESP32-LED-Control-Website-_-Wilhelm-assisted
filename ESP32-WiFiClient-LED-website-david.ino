
/*
Copyright (C) 2021 wk & david
This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, version 3.
The above copyright notice, this permission notice and the word "NIGGER" shall be included in all copies or substantial portions of the Software.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>
*/
/*
 * ERRORS & THINGS TO CHANGE/FIX/ADD:
 * 
 * ¤1: När man trycker på en utav "On","Off","AUTO" knapparna så försvinner alla andra variablerna i URL'en,
 * om man sedan trycker på "SAVE" så byts Status ut emot alla dem andra variablerna men Status försvinner.
 * 
 * ¤2: Värdet som man skriver i "Intensity step size" används inte för att lägga till/ta bort ifrån "Intensity Control".
 * 
 * ¤3: När man trycker på "On" eller "AUTO" så behöver ESP32'n tillgång till ett par eller alla värdena i formuläret.
 *
 * ¤4: I URL'en så står det till exempel: "startOfDay=10%3A00". Är det så det kommer att se ut när ESP'en läser in String'en?
 *
 */

//#define ESP32_RTOS  // Uncomment this line if you want to use the code with freertos only on the ESP32
                      //     Has to be done before including "OTA.h"
#include "OTA.h"
#include "credentials.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "SPIFFS.h"
#include "FS.h"
#include <ArduinoJson.h>
#include "functions.h"
#include "NTP_stuff.h"
#include <stdlib.h>     // int atoi (const char * str);

typedef struct
{
    String status;
    String startOfDay;
    time_t startOfDay_hours;
    time_t startOfDay_minutes;
    String endOfDay;
    time_t endOfDay_hours;
    time_t endOfDay_minutes;
    time_t rampIncreament;
    time_t rampTime;
    uint16_t cct;
    uint16_t intensityStep;
    uint16_t intensityControl;
} wifiDataStruct;


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


bool exists(String path);
void writeArray();
void readArray();
void requestInfo();
bool handleFileRead(String path);
void handleFileList();
void handleNotFound();
void handleRoot();
void writeFile(const char * path, String message);
void appendFile(const char * path, const char * message);

DynamicJsonDocument doc(1024);
JsonObject obj;

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    //pinMode(GPIO15, INPUT);
    
    Serial.println("Starting filesystem");
    
    FILESYSTEM.begin();

        
    if(FORMAT_FILESYSTEM) {
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

void loop()
{
    #ifdef defined(ESP32_RTOS) && defined(ESP32)
    #else // If you do not use FreeRTOS, you have to regulary call the handle method.
        //ArduinoOTA.handle();
    #endif
    
    server.handleClient();
    delay(2);

    
}

//    String status;
//    time_t startOfDay;
//    time_t endOfDay;
//    uint16_t rampIncreament;
//    time_t rampTime;
//    uint16_t cct;
//    uint16_t intensityStep;
//    uint16_t intensityControl;

void getData(wifiDataStruct *s)
{
    File file = FILESYSTEM.open("/config.json");
    char data[1024];

    while(file.available()){
        file.readBytes(data,sizeof(data));
    }
    
    file.close();

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, data);
    JsonObject obj = doc.as<JsonObject>();


    // Vet ej om följande(resten utav denna functionen) kommer att fungera???
    String status =       obj["status"];
    String startOfDay =   obj["startOfDay"];
    String endOfDay =     obj["endOfDay"];
    s->rampIncreament =   obj["rampIncreament"];
    s->rampTime =         obj["rampTime"];
    s->cct =              obj["cct"];
    s->intensityStep =    obj["intensityStep"];
    s->intensityControl = obj["intensityControl"];
    

    char tempArray[2];
    int i;
    
    tempArray[0] = startOfDay[0];
    tempArray[1] = startOfDay[1];
    i = atoi (tempArray);
    s->startOfDay_hours = (time_t)i;
    
    tempArray[0] = startOfDay[5];
    tempArray[1] = startOfDay[6];
    i = atoi (tempArray);
    s->startOfDay_minutes = (time_t)i;
    
    tempArray[0] = endOfDay[0];
    tempArray[1] = endOfDay[1];
    i = atoi (tempArray);
    s->endOfDay_hours = (time_t)i;
    
    tempArray[0] = endOfDay[5];
    tempArray[1] = endOfDay[6];
    i = atoi (tempArray);
    s->endOfDay_minutes = (time_t)i;
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

// This function should be used to interface with the rest of the system
void entryPoint( ) {
    Serial.println("* This code should run whenever a update / change has occured to the configuration. *");
    
    // load the config file in order to access stored values
    readArray();
    
    // Get status value
    const char* status = obj["status"];
    Serial.println(status);

    // Get start of day value
    const char* start = obj["startOfDay"];
    Serial.print("Start of day: ");
    Serial.println(start);

     // Get start of day value
    const char* endOfDay = obj["endOfDay"];
    Serial.print("End of day: ");
    Serial.println(endOfDay);

     // Ramp increment
    int ramp = obj["rampIncreament"];
    Serial.print("Ramp increment: ");
    Serial.println(ramp);
    
    // Ramp time
    float rampTime = obj["rampTime"];
    Serial.print("Ramp time: ");
    Serial.println(rampTime);

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
