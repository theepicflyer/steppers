#include <RTClib.h>
RTC_DS3231 rtc;
/* Example implementation of an alarm using DS3231
 *
 * VCC and GND of RTC should be connected to some power source
 * SDA, SCL of RTC should be connected to SDA, SCL of arduino
 * SQW should be connected to CLOCK_INTERRUPT_PIN
 * CLOCK_INTERRUPT_PIN needs to work with interrupts
 */

// the pin that is connected to SQW
#define CLOCK_INTERRUPT_PIN 2

void setup() {
  if(!rtc.begin()) {
        Serial.println("Couldn't find RTC!");
        Serial.flush();
        while (1) delay(10);
  }
  
  // put your setup code here, to run once:
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));


}

void loop() {
  // put your main code here, to run repeatedly:

}
