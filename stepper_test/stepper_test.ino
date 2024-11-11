#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <math.h>
// defines pins
#define stepPin 2
#define dirPin 17
#define enPin 18

// Settings
const int ROLLING_INTERVAL = 20; // Interval between rollouts
const int TIME_START = 7; // Morning start hour
const int TIME_END = 19; // Evening stop hour
const float ROTATIONAL_SPEED  = PI / 2; //rad per second

const int steps_per_rev = 1600; // Motor & driver steps per rev

// Global Variables


unsigned long lastTaskTime = 0;

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
}

void loop() {

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(tft.width() / 2, 0); // Center top
  tft.drawString(getTime(), tft.width() / 2, tft.height() / 2 - 32);
  tft.drawString("Next rollout in %d minutes", tft.width() / 2, tft.height() / 2);
  
  DateTime now = rtc.now();
  if (now.hour() >= TIME_START && TIME_END < 19) {
    unsigned long currentMinutes = now.hour() * 60 + now.minute();

    if ((currentMinutes - lastTaskTime) >=  ROLLING_INTERVAL) {
      float rotational_period;
      float pulse_width;

      rotational_period = 2 * PI / ROTATIONAL_SPEED;
      pulse_width = (rotational_period / steps_per_rev) / 2;
      tft.drawString("ROLLING", tft.width() / 2, tft.height() / 2);
      rollFilm(pulse_width);
      lastTaskTime = currentMinutes;
    }
  }

  delay(1000);
}

void rollFilm(int pulse_width) {
  tft.fillScreen(TFT_BLACK);
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
}

String getTime() {
  DateTime now = rtc.now();
  char timeStr[20];
  
  // Format: HH:MM:SS
  sprintf(timeStr, "%02d:%02d:%02d", 
    now.hour(),
    now.minute(), 
    now.second()
  );
  
  return String(timeStr);
}