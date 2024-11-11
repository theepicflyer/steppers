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

void setup() {
  // Sets the two pins as Outputs
  pinMode(stepPin, OUTPUT); 
  pinMode(dirPin, OUTPUT);

  rotational_period = 2 * PI / ROTATIONAL_SPEED;
  pulse_width = (rotational_period / steps_per_rev) / 2;

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

}
void loop() {
  Datetime now = rtc.now();
  
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