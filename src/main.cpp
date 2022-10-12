#include <Arduino.h>
#include "SevSeg.h"

#include <WiFi.h>

#include <NTPClient.h>          // for access to time servers
#include <WiFiUdp.h>            // for using UDP protocol
#include <time.h>               // to access to Unix library calls

#include "wifi_ruts.h"

#define TZ(tz)      (tz*3600)               //  Macro to convert from time zone in hours to seconds

//  Week names

const char *daysOfTheWeek[] =
{
    "Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"
};

/*
 *  Creacion de objetos
 */

WiFiUDP ntpUDP;                                                 //  Object for using UDP
NTPClient timeClient(ntpUDP, NTP_SERVER, TZ(TIME_ZONE) );       //  Object NTPClient

SevSeg sevseg;

void setup(){

  Serial.begin(SERIAL_BAUD);
  wifi_connect();
  timeClient.begin();

  byte numDigits = 4;
  byte digitPins[] = {26, 25, 33, 32};
  byte segmentPins[] = {17, 2, 14, 5, 27, 16, 13, 4};

  bool resistorsOnSegments = true; 
  bool updateWithDelaysIn = true;
  byte hardwareConfig = COMMON_CATHODE; 
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(95);
}

void loop(){
    time_t epoch_time;
    char buf[80];

    timeClient.update();                    //  Get time and date from server

    Serial.print( daysOfTheWeek[timeClient.getDay()] );
    Serial.print( ": ");
    Serial.println(timeClient.getFormattedTime());

    Serial.print( "Epoch = " );
    epoch_time = timeClient.getEpochTime();
    Serial.println( epoch_time );
    strftime(buf, sizeof(buf),"%H%M", localtime(&epoch_time));
    Serial.println(buf);

    Serial.println();

    unsigned long prMillis = millis();

    sevseg.setNumber(std::stoi(buf), 2);

    while (millis() - prMillis < 500)
    {
      sevseg.refreshDisplay();
    }
    
    delay(1000);
}