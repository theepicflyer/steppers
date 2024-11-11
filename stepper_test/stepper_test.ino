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
float rotational_period;
float pulse_width;

unsigned long lastTaskTime = 0;

RTC_DS3231 rtc;
TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

void setup() {

  // Sets the two pins as Outputs
  pinMode(stepPin, OUTPUT); 
  pinMode(dirPin, OUTPUT);

  rotational_period = 2 * PI / ROTATIONAL_SPEED;
  pulse_width = (rotational_period / steps_per_rev) / 2;

  if (!rtc.begin()) {
    // Serial.println("Couldn't find RTC"); // Change this to screen
    while (1);
  }

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(0, 0);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);
  
  tft.drawString("HELLO", tft.width() / 2, tft.height() / 2);

}
void loop() {
  Datetime now = rtc.now();

  tft.fillScreen(TFT_BLACK);
  tft.drawString("CURRENT TIME:", tft.width() / 2, tft.height() / 2);
  tft.drawString(getTime(), tft.width() / 2, tft.height() / 2 + 16);
  
  // If during active hours
  if (now.hour() >= TIME_START && TIME_END < 19) {
    unsigned long currentMinutes = now.hour() * 60 + now.minute();

    // It's time to roll
    if ((currentMinutes - lastTaskTime) >=  ROLLING_INTERVAL) {
      moveFilm(pulse_width);
      lastTaskTime = currentMinutes;
    }
  }
  // Not in active hours just do nothing

  // Check every second
  delay(1000);
}

void moveFilm(pulse_width){
  tft.fillScreen(TFT_BLACK);
  tft.drawString("ROLLING", tft.width() / 2, tft.height() / 2);

  digitalWrite(enPin, LOW); // Is it active low or high
  digitalWrite(dirPin, HIGH);

  for(int x = 0; x < 1600; x++) {
    digitalWrite(stepPin, HIGH); 
    delayMicroseconds(pulse_width);    // by changing this time delay between the steps we can change the rotation speed
    digitalWrite(pulse_width, LOW);
    delayMicroseconds(pulse_width); 
  }

  digitalWrite(enPin, HIGH); // Is it active low or high
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

