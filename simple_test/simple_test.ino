#include <TMC2209.h>

// ESP32-C3 UART configuration
const long SERIAL_BAUD_RATE = 115200;
const int RX_PIN = 20;  // ESP32-C3 UART RX pin
const int TX_PIN = 21;  // ESP32-C3 UART TX pin

// Motor parameters
const int32_t RUN_VELOCITY = 20000;
const int32_t STOP_VELOCITY = 0;
const int RUN_DURATION = 2000;
const int STOP_DURATION = 1000;
const uint8_t RUN_CURRENT_PERCENT = 80;  // Reduced to prevent overheating

// Use Hardware Serial1 for TMC2209 communication
HardwareSerial &serial_stream = Serial1;
TMC2209 stepper_driver;
bool invert_direction = false;

void setup() {
    // Initialize Serial for debugging
    Serial.begin(SERIAL_BAUD_RATE);
    
    // Setup TMC2209 with specific pins and address
    stepper_driver.setup(serial_stream, SERIAL_BAUD_RATE, 
                        TMC2209::SERIAL_ADDRESS_0, RX_PIN, TX_PIN);

    stepper_driver.setRunCurrent(RUN_CURRENT_PERCENT);
    stepper_driver.enableCoolStep();
    stepper_driver.enable();
}

void loop() {
    // Your existing loop code remains the same
    stepper_driver.moveAtVelocity(STOP_VELOCITY);
    delay(STOP_DURATION);
    
    if (invert_direction) {
        stepper_driver.enableInverseMotorDirection();
    } else {
        stepper_driver.disableInverseMotorDirection();
    }
    invert_direction = !invert_direction;

    stepper_driver.moveAtVelocity(RUN_VELOCITY);
    delay(RUN_DURATION);
}