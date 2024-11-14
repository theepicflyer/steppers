#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <math.h>
// defines pins
#define stepPin 2
#define dirPin 17
#define enPin 15

// Settings
const int ROLLING_INTERVAL = 20; // Interval between rollouts, minutes
const int TIME_START = 7; // Morning start hour
const int TIME_END = 19; // Evening stop hour
const float ROTATIONAL_SPEED  = PI / 2; //rad per second

const int steps_per_rev = 1600; // Motor & driver steps per rev

// Global Variables


uint32_t lastRollTime;
uint32_t nextRollTime;

RTC_DS3231 rtc;
TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

void setup() {

  // Stepper setup
  pinMode(stepPin, OUTPUT); 
  pinMode(dirPin, OUTPUT);

  // Screen setup
  tft.init();
  tft.setRotation(1); // Check the orientation
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(2);
  
  tft.drawString("HELLO", tft.width() / 2, tft.height() / 2);

  // RTC setup
  if (!rtc.begin()) {
    tft.drawString("ERROR: RTC NOT DETECTED", tft.width() / 2, tft.height() / 2);
    while (1);
  }

  // Do an initial roll in 1 minute
  nextRollTime = rtc.now().unixtime() + 1 * 60;
}

void loop() {
  uint32_t currentTime = rtc.now().unixtime();

  //tft.fillScreen(TFT_BLACK);
  tft.setCursor(tft.width() / 2, 0); // Center top

  tft.setTextSize(3);
  tft.drawString(unixTimeString(currentTime), tft.width() / 2, tft.height() / 2 - 36);
  tft.setTextSize(2);
  tft.drawString("Next rollout time:", tft.width() / 2, tft.height() / 2);
  tft.drawString(unixTimeString(nextRollTime), tft.width() / 2, tft.height() / 2 + 24);
  
  if (rtc.now().hour() >= TIME_START && rtc.now().hour() < TIME_END) {
    if (currentTime >= nextRollTime) {
      float rotational_period;
      float pulse_width;

      rotational_period = 2 * PI / ROTATIONAL_SPEED;
      pulse_width = (rotational_period / steps_per_rev) / 2;
      tft.drawString("ROLLING", tft.width() / 2, tft.height() / 2);
      rollFilm(500);
      lastRollTime = currentTime;
      nextRollTime = currentTime + ROLLING_INTERVAL * 60;
    }
  }

  delay(1000);
}

void rollFilm(int pulse_width) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(4);
  tft.drawString("ROLLING", tft.width() / 2, tft.height() / 2);

  digitalWrite(enPin, LOW); // Active low
  digitalWrite(dirPin, HIGH);

  for(int x = 0; x < 1600; x++) {
    digitalWrite(stepPin, HIGH); 
    delayMicroseconds(pulse_width);    // by changing this time delay between the steps we can change the rotation speed
    digitalWrite(stepPin, LOW);
    delayMicroseconds(pulse_width); 
  }

  digitalWrite(enPin, HIGH);
  tft.fillScreen(TFT_BLACK);
}

String unixTimeString(uint32_t unixTime) {
  uint8_t hours = (unixTime / 3600) % 24;
  uint8_t minutes = (unixTime / 60) % 60;
  uint8_t seconds = unixTime % 60;
  char timeStr[9];
  
  // Format: HH:MM:SS
  sprintf(timeStr, "%02d:%02d:%02d", 
    hours,
    minutes, 
    seconds
  );
  
  return String(timeStr);
}