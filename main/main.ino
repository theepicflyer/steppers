#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <math.h>
#include <TMC2209.h>

// LilyGo ESP32 UART configuration
const long SERIAL_BAUD_RATE = 115200;
const int UART1_RX = 12;  // LilyGo UART1 RX pin
const int UART1_TX = 13;  // LilyGo UART1 TX pin

const int UART2_RX = 2;  // LilyGo UART2 RX pin
const int UART2_TX = 15;  // LilyGo UART2 TX pin

// Settings
const int TIME_START = 7; // Morning start hour
const int TIME_END = 19; // Evening stop hour

const int ROLLOUT_INTERVAL = 1; // Interval between rollouts, minutes
const float ROLLOUT_SPEED = 15; // mm per sec
const float ROLLOUT_LENGTH = 900; // mm
const float ROLLOUT_PERIOD = ROLLOUT_LENGTH / ROLLOUT_SPEED;

const float TOTAL_LENGTH = 30; // meters
const float OUTER_DIAMETER = 74; // mm
const float INNER_DIAMETER = 19; // mm
const float AVERAGE_THICKNESS = PI * (sq(OUTER_DIAMETER / 2) - sq(INNER_DIAMETER / 2)) / (TOTAL_LENGTH * 1000);
// const float AVERAGE_THICKNESS = 0.1

const int STEPS_PER_REV = 72000; // Motor & driver steps per rev

// Global Variables

float RADIUS_CURRENT = 10;

uint32_t lastRollTime;
uint32_t nextRollTime;

RTC_DS3231 rtc;
TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

// Use Hardware Serial1 for TMC2209 communication
const uint8_t RUN_CURRENT_PERCENT = 100;  

HardwareSerial &serial_stream1 = Serial1;
TMC2209 receive_driver;

HardwareSerial &serial_stream2 = Serial2;
TMC2209 supply_driver;

bool invert_direction = false;

void setup() {

  // Stepper setup
  receive_driver.setup(serial_stream1, SERIAL_BAUD_RATE, 
                      TMC2209::SERIAL_ADDRESS_0, UART1_RX, UART1_TX);
  receive_driver.setStandstillMode(receive_driver.STRONG_BRAKING);        // When not pulling
  receive_driver.setRunCurrent(RUN_CURRENT_PERCENT);
  receive_driver.enableAutomaticCurrentScaling();
  receive_driver.enableCoolStep();
  receive_driver.enable();

  supply_driver.setup(serial_stream2, SERIAL_BAUD_RATE, 
                      TMC2209::SERIAL_ADDRESS_0, UART2_RX, UART2_TX);
  supply_driver.setStandstillMode(supply_driver.STRONG_BRAKING);                 // All the time
  supply_driver.setRunCurrent(RUN_CURRENT_PERCENT);
  supply_driver.enableAutomaticCurrentScaling();
  supply_driver.enableCoolStep();
  supply_driver.enable();

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

  tft.setCursor(tft.width() / 2, 0); // Center top
  tft.setTextSize(3);
  tft.drawString(unixTimeString(currentTime), tft.width() / 2, tft.height() / 2 - 48);
  tft.setTextSize(2);
  tft.drawString("Next rollout time:", tft.width() / 2, tft.height() / 2 - 24);
  tft.drawString(unixTimeString(nextRollTime), tft.width() / 2, tft.height() / 2);
  
  if (rtc.now().hour() >= TIME_START && rtc.now().hour() < TIME_END) {
    if (currentTime >= nextRollTime) {
      float rotations = (ROLLOUT_LENGTH / RADIUS_CURRENT / (2*PI)); 
                 
      rollFilm(rotations);

      RADIUS_CURRENT = sqrt(ROLLOUT_LENGTH * AVERAGE_THICKNESS / PI + sq(RADIUS_CURRENT));

      lastRollTime = rtc.now().unixtime();
      nextRollTime = lastRollTime + ROLLOUT_INTERVAL * 60;
    }
  }

  delay(1000);
}

void rollFilm(float rotations) {
  int steps = STEPS_PER_REV * rotations;
  int rotational_speed = steps / ROLLOUT_PERIOD;
  
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(3);
  tft.drawString("ROLLING", tft.width() / 2, tft.height() / 2 - 24);
  char text1[30];
  char text2[30];
  sprintf(text1, "Speed: %d", rotational_speed);
  sprintf(text2, "Rotations: %f", rotations);
  tft.setTextSize(2);
  tft.drawString(text1, tft.width() / 2, tft.height() / 2 + 12);
  tft.drawString(text2, tft.width() / 2, tft.height() / 2 + 36);

  receive_driver.moveAtVelocity(rotational_speed);
  delay(ROLLOUT_PERIOD * 1000);
  receive_driver.moveAtVelocity(0); // Change to the braking thing?

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