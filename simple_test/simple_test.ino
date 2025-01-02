#include <TMC2209.h>

// LilyGo ESP32 UART configuration
const long SERIAL_BAUD_RATE = 115200;

const int RX_PIN = 15;  // LilyGo UART RX pin
const int TX_PIN = 33;  // LilyGo UART TX pin, tested with pins 17 and 13. 15 doesnt work

// Motor parameters
const int32_t RUN_VELOCITY = 17000; // 36000 steps per period * 2 seconds is 1 revolution.
const int32_t STOP_VELOCITY = 0;

// Test specific parameters
const int RUN_DURATION = 2000;
const int STOP_DURATION = 2000;
const uint8_t RUN_CURRENT_PERCENT = 100;  
int mode = 0;

// Init global variables
HardwareSerial &serial_stream = Serial2;
TMC2209 stepper_driver;

void setup() {
    // Initialize Serial for debugging
    Serial.begin(SERIAL_BAUD_RATE);
    
    // Setup TMC2209 with specific pins and address
    stepper_driver.setup(serial_stream, SERIAL_BAUD_RATE, TMC2209::SERIAL_ADDRESS_0, RX_PIN, TX_PIN);

    stepper_driver.setRunCurrent(RUN_CURRENT_PERCENT);
    stepper_driver.enableAutomaticCurrentScaling();
    stepper_driver.enableCoolStep();
    stepper_driver.enable();

    stepper_driver.setStandstillMode(stepper_driver.FREEWHEELING);
}

void loop() {
    mode = (mode + 1) % 5;
    stepper_driver.moveAtVelocity(RUN_VELOCITY);
    delay(RUN_DURATION);

    stepper_driver.moveAtVelocity(STOP_VELOCITY);
    delay(STOP_DURATION);
}