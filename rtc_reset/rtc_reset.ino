#include <TFT_eSPI.h>
#include <SPI.h>
#include <RTClib.h>

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library
RTC_DS3231 rtc;

void setup() {
  if (!rtc.begin()) {
    tft.drawString("ERROR: RTC NOT DETECTED", tft.width() / 2, tft.height() / 2);
    while (1);
  }
  
  // put your setup code here, to run once:
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // Screen setup
  tft.init();
  tft.setRotation(1); // Check the orientation
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);

}

void loop() {
  // put your main code here, to run repeatedly:
  tft.setCursor(tft.width() / 2, 0); // Center top
  tft.drawString(getTime(), tft.width() / 2, tft.height() / 2 - 16);
  delay(1);
}

String getTime() {
  DateTime now = rtc.now();
  char timeStr[20];
  
  // Format: HH:MM:SS
  sprintf(timeStr, "%02d:%02d:%02d", 
    now.hour(),
    now.minute(), 
    now.second()
  );
  
  return String(timeStr);
}