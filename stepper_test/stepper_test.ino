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
const int TIME_START = 7; // Morning start hour
const int TIME_END = 19; // Evening stop hour

const int ROLLOUT_INTERVAL = 1; // Interval between rollouts, minutes
const float ROLLOUT_SPEED = 15; // mm per sec
const float ROLLOUT_LENGTH = 900; // mm

const float TOTAL_LENGTH = 28; // meters
const float OUTER_DIAMETER = 107; // mm
const float INNER_DIAMETER = 97; // mm
const float AVERAGE_THICKNESS = PI * (sq(OUTER_DIAMETER / 2) - sq(INNER_DIAMETER / 2)) / (TOTAL_LENGTH * 1000);

const int STEPS_PER_REV = 1600; // Motor & driver steps per rev

// Global Variables

float RADIUS_CURRENT = 10;

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
  tft.drawString(unixTimeString(currentTime), tft.width() / 2, tft.height() / 2 - 48);
  tft.setTextSize(2);
  tft.drawString("Next rollout time:", tft.width() / 2, tft.height() / 2 - 24);
  tft.drawString(unixTimeString(nextRollTime), tft.width() / 2, tft.height() / 2);
  
  if (rtc.now().hour() >= TIME_START && rtc.now().hour() < TIME_END) {
    if (currentTime >= nextRollTime) {
      int pulse_width;
      float rotations;      
      pulse_width = 2 * PI / ROLLOUT_SPEED * RADIUS_CURRENT / STEPS_PER_REV / 2 * 1e6; // in us
      rotations = (ROLLOUT_LENGTH / RADIUS_CURRENT / (2*PI));
      
      rollFilm(pulse_width, rotations);

      RADIUS_CURRENT = sqrt(ROLLOUT_LENGTH * AVERAGE_THICKNESS / PI + sq(RADIUS_CURRENT));

      lastRollTime = rtc.now().unixtime();
      nextRollTime = lastRollTime + ROLLOUT_INTERVAL * 60;
    }
  }

  delay(1000);
}

void rollFilm(int pulse_width, float rotations) {
  // pulse_width in microseconds
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(3);
  tft.drawString("ROLLING", tft.width() / 2, tft.height() / 2 - 24);
  char text1[30];
  char text2[30];
  sprintf(text1, "Pulse Width: %d", pulse_width);
  sprintf(text2, "Rotations: %f", rotations);
  tft.setTextSize(2);
  tft.drawString(text1, tft.width() / 2, tft.height() / 2);
  tft.drawString(text2, tft.width() / 2, tft.height() / 2 + 12);

  digitalWrite(enPin, LOW); // Active low
  digitalWrite(dirPin, HIGH);

  int steps = STEPS_PER_REV * rotations;

  for(int x = 0; x < steps; x++) {
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