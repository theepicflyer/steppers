# steppers

## Setup
Install the following libraries
* RTClib by Adafruit
* TFT_eSPI by Bodmer

Navigate to TFT_eSPI library folder. Likely in Documents/Arduino/libraries.
In `User_Setup_Select.h`,
* Comment out `#include <User_Setup.h>           // Default setup is root library folder`
* Uncomment `#include <Setup25_TTGO_T_Display.h>    // Setup file for ESP32 and TTGO T-Display ST7789V SPI bus TFT`