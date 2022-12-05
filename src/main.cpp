#include <Arduino.h>

#include "SevSeg.h"

#include "ntp.h"

#include "wifi_ruts.h"

#include "mqtt_sub.h"

SevSeg sevseg;

Time_Record time_this;

int seconds = 0;

bool activate_alarm = false;

unsigned long previousMillis = 0;

const long ntpInterval = 60000 * 60;

const long internalClockInterval = 10000;

const long buzzerInterval = 800;

void setup(){

  pinMode(BUZZ, OUTPUT);

  pinMode(BUTTON, INPUT_PULLUP);

  wifi_connect();

  setupTime();

  Serial.begin(SERIAL_BAUD);

  Serial.println("INITIATING");
  init_mqtt();

  byte numDigits = 4;
  byte digitPins[] = {26, 25, 33, 32};
  byte segmentPins[] = {17, 2, 14, 5, 27, 16, 13, 4};

  bool resistorsOnSegments = true;
  bool updateWithDelaysIn = true;
  byte hardwareConfig = COMMON_CATHODE;
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(95);
  time_this = getTimeUpdated();
}

int formatTime() {
  return time_this.hours * 100 + time_this.minutes;
}

void updateLocalTime() {
  if (time_this.seconds < 59) {
    time_this.seconds += 1;
    return;
  }
  else {
    if (time_this.minutes < 59) {
      time_this.seconds = 0;
      time_this.minutes += 1;
      return;
    }
    else {
      if (time_this.hours < 23) {
        time_this.seconds = 0;
        time_this.minutes = 0;
        time_this.hours += 1;
        return;
      }
      else {
        time_this.seconds = 0;
        time_this.minutes = 0;
        time_this.hours = 0;
        return;
      }
    }
  }  
}

void loop(){

  unsigned long currentMillis = millis();

  int n = activation_times_length / sizeof(*activation_times);
  
  if (std::find(activation_times, activation_times + n, formatTime()) != activation_times + n) {
    activate_alarm = true;
  }

  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    updateLocalTime();
  }
  

  if (currentMillis - previousMillis >= ntpInterval) {
    previousMillis = currentMillis;
    time_this = getTimeUpdated();
  }

  if (activate_alarm == true){
    if (currentMillis - previousMillis >= buzzerInterval) {
      previousMillis = currentMillis;
      digitalWrite(BUZZ, !digitalRead(BUZZ));
    }
  }
  

  if (digitalRead(BUTTON) == LOW){
    activate_alarm = false;
    digitalWrite(BUZZ, 0);
    int time_index = std::find(activation_times, activation_times + n,  formatTime()) - activation_times;
    activation_times[time_index] = 0;
  }

  mqtt_comms();
  

  //Display set and refresh
  sevseg.setNumber(formatTime(), 2);
  sevseg.refreshDisplay();
}