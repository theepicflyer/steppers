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
const int TIME_START = 9; // Morning start hour
const int TIME_END = 18; // Evening stop hour

const int ROLLOUT_INTERVAL = 60; // Interval between rollouts, minutes
const float ROLLOUT_SPEED = 15; // mm per sec
const float DAY_ROLLOUT_LENGTH = 500; // mm
const float NIGHT_ROLLOUT_LENGTH = 100; // mm

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

bool invert_direction = true;

// Use Hardware Serial1 for TMC2209 communication
const uint8_t RUN_CURRENT_PERCENT = 100;  

HardwareSerial &serial_stream1 = Serial1;
HardwareSerial &serial_stream2 = Serial2;
TMC2209 drivers[6] = {
  TMC2209(),
  TMC2209(),
  TMC2209(),
  TMC2209(),
  TMC2209(),
  TMC2209()
};

const TMC2209::SerialAddress ADDRESSES[6] = {
  TMC2209::SERIAL_ADDRESS_0,
  TMC2209::SERIAL_ADDRESS_1,
  TMC2209::SERIAL_ADDRESS_2,
  TMC2209::SERIAL_ADDRESS_0,
  TMC2209::SERIAL_ADDRESS_1,
  TMC2209::SERIAL_ADDRESS_2
};

const char *motor_names[6] = {
  "R1",
  "R2",
  "R3",
  "S1",
  "S2",
  "S3"
};


void setup() {

  for (int i = 0; i < 3; i++){
    drivers[i].setup(serial_stream1, SERIAL_BAUD_RATE, ADDRESSES[i], UART1_RX, UART1_TX);
    drivers[i].setStandstillMode(TMC2209::STRONG_BRAKING);        // When not pulling
    drivers[i].setRunCurrent(RUN_CURRENT_PERCENT);
    drivers[i].enableAutomaticCurrentScaling();
    drivers[i].enableAutomaticGradientAdaptation();
    drivers[i].enableCoolStep();
    drivers[i].enableInverseMotorDirection();
    drivers[i].enable();
  }

  for (int i = 3; i < 6; i++){
    drivers[i].setup(serial_stream2, SERIAL_BAUD_RATE, ADDRESSES[i], UART2_RX, UART2_TX);
    drivers[i].setStandstillMode(TMC2209::STRONG_BRAKING);        // When not pulling
    drivers[i].setRunCurrent(RUN_CURRENT_PERCENT);
    drivers[i].enableAutomaticCurrentScaling();
    drivers[i].enableAutomaticGradientAdaptation();
    drivers[i].enableCoolStep();
    drivers[i].enableInverseMotorDirection();
    drivers[i].enable();
  }

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
      float rotations = (DAY_ROLLOUT_LENGTH / RADIUS_CURRENT / (2*PI)); 
      float period = DAY_ROLLOUT_LENGTH / ROLLOUT_SPEED;
      
      for (int i = 0; i < 6; i++){
        rollFilm(rotations, period, i);
      }

      RADIUS_CURRENT = sqrt(DAY_ROLLOUT_LENGTH * AVERAGE_THICKNESS / PI + sq(RADIUS_CURRENT));

      lastRollTime = rtc.now().unixtime();
      nextRollTime = lastRollTime + ROLLOUT_INTERVAL * 60;
    }
  }
  else {
    if (currentTime >= nextRollTime) {
      float rotations = (NIGHT_ROLLOUT_LENGTH / RADIUS_CURRENT / (2*PI)); 
      float period = NIGHT_ROLLOUT_LENGTH / ROLLOUT_SPEED;
      
      for (int i = 0; i < 6; i++){
        rollFilm(rotations, period, i);
      }

      RADIUS_CURRENT = sqrt(NIGHT_ROLLOUT_LENGTH * AVERAGE_THICKNESS / PI + sq(RADIUS_CURRENT));

      lastRollTime = rtc.now().unixtime();
      nextRollTime = lastRollTime + ROLLOUT_INTERVAL * 60;
    }
  }

  delay(1000);
}

void rollFilm(float rotations, float period, int motor_i) {
  int steps = STEPS_PER_REV * rotations;
  int rotational_speed = steps / period;
  
  tft.fillScreen(TFT_BLACK);
  
  char text0[30];
  char text1[30];
  char text2[30];
  sprintf(text0, "ROLLING: %s", motor_names[motor_i]);
  sprintf(text1, "Speed: %d", rotational_speed);
  sprintf(text2, "Rotations: %f", rotations);

  tft.setTextSize(3);
  tft.drawString(text0, tft.width() / 2, tft.height() / 2 - 24);
  tft.setTextSize(2);
  tft.drawString(text1, tft.width() / 2, tft.height() / 2 + 12);
  tft.drawString(text2, tft.width() / 2, tft.height() / 2 + 36);

  drivers[motor_i].moveAtVelocity(rotational_speed);
  delay(period * 1000);
  drivers[motor_i].moveAtVelocity(0);

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