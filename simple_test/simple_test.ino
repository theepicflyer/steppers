#include <TMC2209.h>

// LilyGo ESP32 UART configuration
const long SERIAL_BAUD_RATE = 115200;

// UART Pins
const int UART1_RX = 12;  // LilyGo UART1 RX pin
const int UART1_TX = 13;  // LilyGo UART1 TX pin

const int UART2_RX = 2;  // LilyGo UART2 RX pin
const int UART2_TX = 15;  // LilyGo UART2 TX pin

// Motor parameters
const int32_t RUN_VELOCITY = 17000; // 36000 steps per period * 2 seconds is 1 revolution.
const int32_t STOP_VELOCITY = 0;

// Test specific parameters
const int DURATION = 5000;
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
    delay(1000);
    driver1.setup(serial_stream1, SERIAL_BAUD_RATE, TMC2209::SERIAL_ADDRESS_0, UART1_RX, UART1_TX);
    driver1.setRunCurrent(RUN_CURRENT_PERCENT);
    driver1.enableAutomaticCurrentScaling();
    driver1.enableCoolStep();
    driver1.enable();

}

void loop() {

    static bool is_running = false;
    static unsigned long run_start_time;

    // Modified movement section with StallGuard monitoring
    if (!is_running) {
        driver1.moveAtVelocity(RUN_VELOCITY);
        run_start_time = millis();
        is_running = true;
        Serial.println("Starting movement with torque monitoring");
    }
    else {
        // Monitor torque during movement
        if (millis() - run_start_time < DURATION) {
            // Read StallGuard result every 100ms
            static unsigned long last_sg_read = 0;
            if (millis() - last_sg_read >= 100) {
                last_sg_read = millis();
                
                // Get torque feedback
                uint16_t sg_result = driver1.getStallGuardResult();
                Serial.print("Torque load: ");
                Serial.print(sg_result);  // Lower values = higher load
                Serial.print(" (");
                Serial.print(map(sg_result, 0, 1023, 100, 0));
                Serial.println("% load)");

                // Safety threshold (calibrate for your system)
                const uint16_t STALL_THRESHOLD = 100; 
                if (sg_result < STALL_THRESHOLD) {
                    Serial.println("WARNING: Approaching torque limits!");
                    // Optional: Add automatic response here
                }
            }
        }
        else {
            driver1.moveAtVelocity(STOP_VELOCITY);
            is_running = false;
            Serial.println("Stopped movement");
            delay(DURATION);
        }
    }
}