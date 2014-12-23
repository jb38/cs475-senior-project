
#include <Wire.h>
//#include <SPI.h>
#include <RTC_DS3231.h>  // rtc

RTC_DS3231 RTC;

void setup() {

  Wire.begin(); // I2C init as master

  setupRTC();
  
  
}

void setupRTC() {

  RTC.begin();

  if (!RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  // set the date on the RTC to be the date and time of compilation.
  // this may result in a few-second offset between "real" time and
  // RTC time, but this is negligible as temperature will not change
  // drastically in a matter of a few seconds
  DateTime now = RTC.now();
  DateTime compiled = DateTime(__DATE__, __TIME__);
  if (now.unixtime() < compiled.unixtime()) {
    RTC.adjust(DateTime(__DATE__, __TIME__));
  } 
}

void loop() {




}


