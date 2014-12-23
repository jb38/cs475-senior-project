#include <Wire.h>
#include <OneWire.h>
#include <RTC_DS3231.h>

#define LOOP_DELAY 1000 //ms delay time in loop
//#define LOOP_DELAY 59000 //ms delay time in loop
#define TEMP_PIN 38 // signal pin for DS18S20

RTC_DS3231 RTC;
OneWire ds(TEMP_PIN);

void setup() {
  Serial.begin(57600);
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
  //DateTime now = RTC.now();
  //DateTime compiled = DateTime(__DATE__, __TIME__);
  //if (now.unixtime() < compiled.unixtime()) {
  //  RTC.adjust(DateTime(__DATE__, __TIME__));
  //} 
}

void loop() {

  /*
  Toggle Count over 5000ms with PWM_COUNT of 1020:  0
  2014/12/23 17:26:27
   since midnight 1/1/1970 = 1419355587s = 16427d
   now + 7d + 30s: 2014/12/30 17:26:57
  Temp as float: 25.2500000000
  Temp as word: 25.25
  */

  DateTime now = RTC.now();
  printTime(now);

  float temperature = getTemp();
  printTemp(temperature);

  // calculate the remaining time
  
  // sleep for the remaining time
  
  delay(LOOP_DELAY); // DEBUG
}

void printTime(DateTime now) {
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println(); 
}

void printTemp(float temperature) {
  Serial.println(temperature); 
}

float getTemp() {
//  http://bildr.org/2011/07/ds18b20-arduino/
 //returns the temperature from one DS18S20 in DEG Celsius

   byte data[12];
   byte addr[8];
  
   if ( !ds.search(addr)) {
     //no more sensors on chain, reset search
     ds.reset_search();
     return -1000;
   }
  
   if ( OneWire::crc8( addr, 7) != addr[7]) {
     Serial.println("CRC is not valid!");
     return -1000;
   }
  
   if ( addr[0] != 0x10 && addr[0] != 0x28) {
     Serial.print("Device is not recognized");
     return -1000;
   }
  
   ds.reset();
   ds.select(addr);
   ds.write(0x44,1); // start conversion, with parasite power on at the end
  
   byte present = ds.reset();
   ds.select(addr);  
   ds.write(0xBE); // Read Scratchpad
  
   
   for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
   }
   
   ds.reset_search();
   
   byte MSB = data[1];
   byte LSB = data[0];
  
   float tempRead = ((MSB << 8) | LSB); //using two's compliment
   float TemperatureSum = tempRead / 16;
   
   return TemperatureSum;
 
}
