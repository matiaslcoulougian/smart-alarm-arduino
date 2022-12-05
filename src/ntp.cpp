#include <WiFi.h>

#include <NTPClient.h>          // for access to time servers
#include <WiFiUdp.h>            // for using UDP protocol
#include <time.h>               // to access to Unix library calls

#include "ntp.h"

#define TZ(tz)      (tz*3600)               //  Macro to convert from time zone in hours to seconds

WiFiUDP ntpUDP;                                                 //  Object for using UDP
NTPClient timeClient(ntpUDP, NTP_SERVER, TZ(TIME_ZONE) );       //  Object NTPClient

void setupTime() {
  timeClient.begin();
}

struct Time_Record getTimeUpdated() {
  Time_Record t;
  time_t epoch_time;
  char hours[80];
  char minutes[80];
  char seconds[80];

  timeClient.update();

  epoch_time = timeClient.getEpochTime();

  strftime(hours, sizeof(hours),"%H", localtime(&epoch_time));
  strftime(minutes, sizeof(minutes),"%M", localtime(&epoch_time));
  strftime(seconds, sizeof(seconds),"%S", localtime(&epoch_time));

  t.hours = std::stoi(hours);
  t.minutes = std::stoi(minutes);
  t.seconds = std::stoi(seconds);
  return t;
}
