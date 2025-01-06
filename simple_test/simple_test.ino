#include <TMC2209.h>

// LilyGo ESP32 UART configuration
const long SERIAL_BAUD_RATE = 115200;
const int UART1_RX = 26;  // LilyGo UART1 RX pin
const int UART1_TX = 17;  // LilyGo UART1 TX pin

// Motor parameters
const int32_t RUN_VELOCITY = 14000; // 36000 steps per period * 2 seconds is 1 revolution.
const int32_t STOP_VELOCITY = 0;

// Test specific parameters
const int DELAY = 4000;
const uint8_t RUN_CURRENT_PERCENT = 100;  
const uint8_t HOLD_CURRENT_STANDSTILL = 0;

// Init global variables
HardwareSerial &serial_stream1 = Serial1;
TMC2209 driver1;

void setup() {
    // Initialize Serial for debugging
    Serial.begin(SERIAL_BAUD_RATE);
    
    // Setup TMC2209 
    driver1.setup(serial_stream1, SERIAL_BAUD_RATE, TMC2209::SERIAL_ADDRESS_0, UART1_RX, UART1_TX);
    driver1.setRunCurrent(RUN_CURRENT_PERCENT);
    driver1.setHoldCurrent(HOLD_CURRENT_STANDSTILL);
    // driver1.enableAutomaticCurrentScaling();
    driver1.enableCoolStep();
    driver1.enable();
}

void loop() {
    
    change_mode(TMC2209::NORMAL);
    driver1.moveAtVelocity(RUN_VELOCITY);
    delay(DELAY);
    driver1.moveAtVelocity(STOP_VELOCITY);
    delay(DELAY);

    change_mode(TMC2209::FREEWHEELING);
    driver1.moveAtVelocity(RUN_VELOCITY);
    delay(DELAY);
    driver1.moveAtVelocity(STOP_VELOCITY);
    delay(DELAY);

    change_mode(TMC2209::STRONG_BRAKING);
    driver1.moveAtVelocity(RUN_VELOCITY);
    delay(DELAY);
    driver1.moveAtVelocity(STOP_VELOCITY);
    delay(DELAY);

    change_mode(TMC2209::BRAKING);
    driver1.moveAtVelocity(RUN_VELOCITY);
    delay(DELAY);
    driver1.moveAtVelocity(STOP_VELOCITY);
    delay(DELAY);
}

void change_mode(TMC2209::StandstillMode mode) {
    TMC2209::Settings settings;
    bool hardware_disabled;

    Serial.print("Setting Mode: ");
    switch (mode)
    {
        case TMC2209::NORMAL:
            Serial.println("normal");
            break;
        case TMC2209::FREEWHEELING:
            Serial.println("freewheeling");
            break;
        case TMC2209::STRONG_BRAKING:
            Serial.println("strong_braking");
            break;
        case TMC2209::BRAKING:
            Serial.println("braking");
            break;
    }
    
    driver1.setStandstillMode(mode);

    Serial.println("Reading:");
    settings = driver1.getSettings();
    hardware_disabled = driver1.hardwareDisabled();
    Serial.print("hardware_disabled = ");
    Serial.println(hardware_disabled);
    Serial.print("Mode = ");
    switch (settings.standstill_mode)
    {
        case TMC2209::NORMAL:
            Serial.println("normal");
            break;
        case TMC2209::FREEWHEELING:
            Serial.println("freewheeling");
            break;
        case TMC2209::STRONG_BRAKING:
            Serial.println("strong_braking");
            break;
        case TMC2209::BRAKING:
            Serial.println("braking");
            break;
    }
    Serial.print("irun_percent = ");
    Serial.println(settings.irun_percent);
    Serial.print("ihold_percent = ");
    Serial.println(settings.ihold_percent);
    Serial.println();
}