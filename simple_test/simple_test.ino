#include <TMC2209.h>

// LilyGo ESP32 UART configuration
const long SERIAL_BAUD_RATE = 115200;

// UART Pins
const int UART1_RX = 26;  // LilyGo UART1 RX pin
const int UART1_TX = 17;  // LilyGo UART1 TX pin

const int UART2_RX = 27;  // LilyGo UART2 RX pin
const int UART2_TX = 13;  // LilyGo UART2 TX pin

// Motor parameters
const int32_t RUN_VELOCITY = 17000; // 36000 steps per period * 2 seconds is 1 revolution.
const int32_t STOP_VELOCITY = 0;

// Test specific parameters
const int DURATION = 2000;
const uint8_t RUN_CURRENT_PERCENT = 70;  

// Init global variables
HardwareSerial &serial_stream1 = Serial1;
TMC2209 driver1;
HardwareSerial &serial_stream2 = Serial2;
TMC2209 driver2;

void setup() {
    // Initialize Serial for debugging
    Serial.begin(SERIAL_BAUD_RATE);
    
    // Setup TMC2209 
    driver1.setup(serial_stream1, SERIAL_BAUD_RATE, TMC2209::SERIAL_ADDRESS_0, UART1_RX, UART1_TX);
    driver1.setRunCurrent(RUN_CURRENT_PERCENT);
    // driver1.enableAutomaticCurrentScaling();
    driver1.enableCoolStep();
    driver1.enable();

    driver2.setup(serial_stream2, SERIAL_BAUD_RATE, TMC2209::SERIAL_ADDRESS_0, UART2_RX, UART2_TX);
    driver2.setRunCurrent(RUN_CURRENT_PERCENT);
    // driver2.enableAutomaticCurrentScaling();
    driver2.enableCoolStep();
    driver2.enable();

    // driver1.setStandstillMode(driver1.FREEWHEELING);
}

void loop() {
    if (driver1.isSetupAndCommunicating())
    {
        Serial.println("Driver 1 is setup and communicating!");
    }
    else if (driver1.isCommunicatingButNotSetup())
    {
        Serial.println("Driver 1 is communicating but not setup!");
        driver1.setup(serial_stream1, SERIAL_BAUD_RATE, TMC2209::SERIAL_ADDRESS_0, UART1_RX, UART1_TX);
    }
    else
    {
        Serial.println("Driver 1 is not communicating!");
    }

    if (driver2.isSetupAndCommunicating())
    {
        Serial.println("Driver 2 is setup and communicating!");
    }
    else if (driver2.isCommunicatingButNotSetup())
    {
        Serial.println("Driver 2 is communicating but not setup!");
        driver2.setup(serial_stream2, SERIAL_BAUD_RATE, TMC2209::SERIAL_ADDRESS_0, UART2_RX, UART2_TX);
    }
    else
    {
        Serial.println("Driver 2 is not communicating!");
    }

    bool hardware_disabled;
    Serial.println();

    hardware_disabled = driver1.hardwareDisabled();
    Serial.print("hardware_disabled = ");
    Serial.println(hardware_disabled);
    driver1.moveAtVelocity(RUN_VELOCITY);
    driver2.moveAtVelocity(STOP_VELOCITY);
    delay(DURATION);
    driver1.moveAtVelocity(STOP_VELOCITY);
    driver2.moveAtVelocity(RUN_VELOCITY);
    delay(DURATION);
}