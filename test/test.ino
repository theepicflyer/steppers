#include <TMC2209.h>

// This example will not work on Arduino boards without HardwareSerial ports,
// such as the Uno, Nano, and Mini.
//
// See this reference for more details:
// https://www.arduino.cc/reference/en/language/functions/communication/serial/

HardwareSerial & serial_stream = Serial2;

const long SERIAL_BAUD_RATE = 115200;
const int RX_PIN = 15;  // LilyGo UART RX pin
const int TX_PIN = 17;  // LilyGo UART TX pin, tested with pins 17 and 13. 15 doesnt work

const int DELAY = 200;
// current values may need to be reduced to prevent overheating depending on
// specific motor and power supply voltage
const uint8_t RUN_CURRENT_PERCENT = 100;
const int32_t VELOCITY = 20000;
const uint8_t STALL_GUARD_THRESHOLD = 50;


// Instantiate TMC2209
TMC2209 stepper_driver;


void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);

  stepper_driver.setup(serial_stream, SERIAL_BAUD_RATE, TMC2209::SERIAL_ADDRESS_0, RX_PIN, TX_PIN);

  stepper_driver.setRunCurrent(RUN_CURRENT_PERCENT);
  stepper_driver.enableAutomaticCurrentScaling();
  stepper_driver.enableCoolStep();
  stepper_driver.setStallGuardThreshold(STALL_GUARD_THRESHOLD);
  stepper_driver.enable();
  stepper_driver.moveAtVelocity(VELOCITY);
}

void loop()
{
  if (not stepper_driver.isSetupAndCommunicating())
  {
    Serial.println("Stepper driver not setup and communicating!");
    return;
  }

  Serial.print("stall_guard_threshold = ");
  Serial.println(STALL_GUARD_THRESHOLD);

  uint16_t stall_guard_result = stepper_driver.getStallGuardResult();
  Serial.print("stall_guard_result = ");
  Serial.println(stall_guard_result);

  Serial.println();
  delay(DELAY);

}