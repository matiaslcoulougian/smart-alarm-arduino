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

long int last_milis = 0;
long int interval = 1000;

struct rtc
{
  int hours;
  int minutes;
  int seconds;
};

rtc real_time_clock;

void setRealTimeClockWithNTP(){
  time_t time;
  char hours[] = "";
  char minutes[] = "";
  char seconds[] = "";

  time = timeClient.getEpochTime();
  strftime(hours, sizeof(hours),"%H%", localtime(&time));
  Serial.println(hours);
  strftime(minutes, sizeof(minutes),"%M%", localtime(&time));
  Serial.println(minutes);
  strftime(seconds, sizeof(seconds),"%S%", localtime(&time));

  real_time_clock = {
    hours: (int) hours, 
    minutes:  (int) minutes, 
    seconds: 0
  };
}

void update_rtc_seconds(){
  if (real_time_clock.seconds < 59) {
    real_time_clock.seconds++;
  }
  else {
    real_time_clock.seconds = 0;
    if (real_time_clock.minutes < 59) {
      real_time_clock.minutes++;
    }
    else {
      real_time_clock.minutes = 0;
      if (real_time_clock.hours < 23) {
        real_time_clock.hours++;
      }
      else {
        real_time_clock.hours = 0;
        real_time_clock.minutes = 0;
        real_time_clock.seconds = 0;
      }
      
    }
  }
  
}

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

  setRealTimeClockWithNTP();
}

void loop(){
    if(millis() >= last_milis + interval) {
      last_milis = millis();
      update_rtc_seconds();
    }

    // time_t epoch_time;

    // char hours[] = "";
    // char minutes[] = "";
    // char seconds[] = "";

    // timeClient.update();

    // Serial.print( daysOfTheWeek[timeClient.getDay()] );
    // Serial.print( ": ");
    // Serial.println(timeClient.getFormattedTime());

    // Serial.print( "Epoch = " );
    // epoch_time = timeClient.getEpochTime();
    // Serial.println( epoch_time );
    // strftime(hours, sizeof(hours),"%H%", localtime(&epoch_time));
    // Serial.println(hours);
    // strftime(minutes, sizeof(minutes),"%M%", localtime(&epoch_time));
    // Serial.println(minutes);
    // strftime(seconds, sizeof(seconds),"%S%", localtime(&epoch_time));
    // Serial.println(seconds);

    // Serial.println();
    std::string hourToString = std::to_string(real_time_clock.hours) + std::to_string(real_time_clock.minutes) + std::to_string(real_time_clock.seconds);
    printf(hourToString.c_str());
    sevseg.setNumber(std::stoi(hourToString), 2);
    sevseg.refreshDisplay();
}