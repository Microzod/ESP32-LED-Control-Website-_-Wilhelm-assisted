#ifndef _NTP_STUFF_
#define _NTP_STUFF_

// 
// 
// Need more work, has barely begun.
// 
// 
// 
#include "credentials.h"
#include <WiFi.h>
#include "time.h"
#include <TimeLib.h>


struct tm ntp_tm;
time_t ntp_time;
tmElements_t timeAlarms_tm;
time_t timeAlarms_time;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 0;

time_t getNTPTime();

void setupNTP()
{
    //init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    ntp_time = getNTPTime();
}

time_t getNTPTime()
{
    time_t _time;
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    if(!getLocalTime(&ntp_tm)){return 0;}
    ntp_time = mktime(&ntp_tm);
    //ntp_time = ntp_tm.tm_sec + (ntp_tm.tm_min * 60) + (ntp_tm.tm_hour * 3600); 
    
    return ntp_time;
}

/*
void getNTP_tm(struct tm * _tm)  //  struct tm * getNTP_tm() ???? Does this code somehow maintain the scope????
{
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    if(!getLocalTime(_tm)){return 0;}
    ntp_time = mktime(_tm);
    //ntp_time = ntp_tm.tm_sec + (ntp_tm.tm_min * 60) + (ntp_tm.tm_hour * 3600); 
}
*/
#endif // _NTP_STUFF_




/*
time_t printLocalTime()
{
    struct tm _tm;
    time_t now;
    time(&now);
    localtime_r(&now, &_tm);
    time_t _time = mktime(&_tm);
}
*/




/*

//   This sketch shows an example of sending a reading to data.sparkfun.com once per day.
//   It uses the Sparkfun testing stream so the only customizing required is the WiFi SSID and password.
//   The Harringay Maker Space
//   License: Apache License v2

#include <NTPtimeESP.h>

#define DEBUG_ON


NTPtime NTPch("pool.ntp.org");   // Choose server pool as required


//      * The structure contains following fields:
//   * struct strDateTime
//  {
//      byte hour;
//      byte minute;
//      byte second;
//      int year;
//      byte month;
//      byte day;
//      byte dayofWeek;
//      boolean valid;
//  };

strDateTime dateTime;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booted");
  Serial.println("Connecting to Wi-Fi");

  WiFi.mode(WIFI_STA);
  WiFi.begin (mySSID, myPASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi connected");
}

void loop() {

  // first parameter: Time zone in floating point (for India); second parameter: 1 for European summer time; 2 for US daylight saving time; 0 for no DST adjustment; (contributed by viewwer, not tested by me)
  dateTime = NTPch.getNTPtime(1.0, 1);

  // check dateTime.valid before using the returned time
  // Use "setSendInterval" or "setRecvTimeout" if required
  if(dateTime.valid){
    NTPch.printDateTime(dateTime);

    byte actualHour = dateTime.hour;
    byte actualMinute = dateTime.minute;
    byte actualsecond = dateTime.second;
    int actualyear = dateTime.year;
    byte actualMonth = dateTime.month;
    byte actualday =dateTime.day;
    byte actualdayofWeek = dateTime.dayofWeek;
  }
}

*/
