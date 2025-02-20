#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <math.h>
#include <TMC2209.h>

// Black V4 PCB
const int UART1_RX = 12; // LilyGo UART1 RX pin
const int UART1_TX = 13; // LilyGo UART1 TX pin
const int UART2_RX = 2;  // LilyGo UART2 RX pin
const int UART2_TX = 15; // LilyGo UART2 TX pin

// White V3 PCB
// const int UART1_RX = 26;  // LilyGo UART1 RX pin
// const int UART1_TX = 17;  // LilyGo UART1 TX pin
// const int UART2_RX = 27;  // LilyGo UART2 RX pin
// const int UART2_TX = 13;  // LilyGo UART2 TX pin


RTC_DS3231 rtc;
TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

bool invert_direction = true;

// Use Hardware Serial1 for TMC2209 communication
const uint8_t RUN_CURRENT_PERCENT = 90;

const float idle_percent = 0.85;
const int idle_period = int(idle_percent * 60.0);
const int run_period = 60 - idle_period;

HardwareSerial &serial_stream1 = Serial1;
HardwareSerial &serial_stream2 = Serial2;
TMC2209 drivers[6] = {
    TMC2209(),
    TMC2209(),
    TMC2209(),
    TMC2209(),
    TMC2209(),
    TMC2209()};

const TMC2209::SerialAddress ADDRESSES[6] = {
    TMC2209::SERIAL_ADDRESS_0,
    TMC2209::SERIAL_ADDRESS_1,
    TMC2209::SERIAL_ADDRESS_2,
    TMC2209::SERIAL_ADDRESS_0,
    TMC2209::SERIAL_ADDRESS_1,
    TMC2209::SERIAL_ADDRESS_2};

const char *motor_names[6] = {
    "R1",
    "R2",
    "R3",
    "S1",
    "S2",
    "S3"};

const long SERIAL_BAUD_RATE = 115200;

void setup()
{
    // Screen setup
    tft.init();
    tft.setRotation(1); // Check the orientation
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(2);

    tft.drawString("HELLO", tft.width() / 2, tft.height() / 2);

    // RTC setup
    if (!rtc.begin())
    {
        tft.drawString("ERROR: RTC NOT DETECTED", tft.width() / 2, tft.height() / 2);
        while (1)
            ;
    }

    delay(500);
    // TMC2209 setup
    for (int i = 0; i < 3; i++)
    {
        drivers[i].setup(serial_stream1, SERIAL_BAUD_RATE, ADDRESSES[i], UART1_RX, UART1_TX);
        drivers[i].setStandstillMode(TMC2209::STRONG_BRAKING); // When not pulling
        drivers[i].setRunCurrent(RUN_CURRENT_PERCENT);
        drivers[i].disableAutomaticCurrentScaling();
        drivers[i].setPwmOffset(125);
        // drivers[i].enableAutomaticGradientAdaptation();
        drivers[i].enableCoolStep();
        drivers[i].enableInverseMotorDirection();
        drivers[i].enable();
    }

    for (int i = 3; i < 6; i++)
    {
        drivers[i].setup(serial_stream2, SERIAL_BAUD_RATE, ADDRESSES[i], UART2_RX, UART2_TX);
        drivers[i].setStandstillMode(TMC2209::STRONG_BRAKING); // When not pulling
        drivers[i].setRunCurrent(RUN_CURRENT_PERCENT);
        drivers[i].disableAutomaticCurrentScaling();
        drivers[i].setPwmOffset(125);
        // drivers[i].enableAutomaticGradientAdaptation();
        drivers[i].enableCoolStep();
        drivers[i].enableInverseMotorDirection();
        drivers[i].enable();
    }
}

void loop()
{
    for (int i = 0; i < 6; i++)
    {
        drivers[i].moveAtVelocity(30000);
    }
    delay(run_period * 1000);

    for (int i = 0; i < 6; i++)
    {
        drivers[i].moveAtVelocity(0);
    }
    delay(idle_period * 1000);
}
