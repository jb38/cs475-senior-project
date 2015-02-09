#include <JeeLib.h>

#include <Wire.h>
#include <RTC_DS3231.h>

#include <OneWire.h> // update at http://www.pjrc.com/teensy/arduino_libraries/OneWire.zip

#include <SPI.h>
#include <SD.h>

#define LOGGING_FREQ_SECONDS   60

#define RTC_POWER_PIN          A3
RTC_DS3231 RTC;

#define TEMP_SIGNAL_PIN        8
OneWire ds(TEMP_SIGNAL_PIN);

#define SD_CARD_SELECT         9
String stationId;

// Define watchdog timer interrupt.
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

// Take a sensor reading and send it to the server.
void logSensorReading() {
  
  DateTime now = getTime();
  float temperature = getTemp();
  
  File file;
  if (file = SD.open("data.txt", FILE_WRITE)) {
    file.print(now.year(), DEC);
    file.print('-');
    file.print(now.month(), DEC);
    file.print('-');
    file.print(now.day(), DEC);
    file.print('T');
    file.print(now.hour(), DEC);
    file.print(':');
    file.print(now.minute(), DEC);
    file.print(':');
    file.print(now.second(), DEC);
    file.print('Z');
    file.print('\t');
    file.println(temperature, 3);
    file.close(); 
  }

  // Connect to the server and send the reading.
  Serial.print(now.year(), DEC);
  Serial.print('-');
  Serial.print(now.month(), DEC);
  Serial.print('-');
  Serial.print(now.day(), DEC);
  Serial.print('T');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.print('Z');
  Serial.print('\t');
  Serial.println(temperature, 3);

  // Note that if you're sending a lot of data you
  // might need to tweak the delay here so the CC3000 has
  // time to finish sending all the data before shutdown.
  delay(100);
}

DateTime getTime() {
  digitalWrite(RTC_POWER_PIN, HIGH);
  DateTime now = RTC.now();
  digitalWrite(RTC_POWER_PIN, LOW);
  return now;
}

float getTemp() { //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if (!ds.search(addr)) {
    //no more sensors on chain, reset search
    ds.reset_search();
    return -1000;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return -1000;
  }

  if (addr[0] != 0x10 && addr[0] != 0x28) {
    Serial.print("Device is not recognized");
    return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1); // start conversion, with parasite power on at the end

  delay(750); // required wait time for parasitic power

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
  float temperatureSum = tempRead / 16;

  return temperatureSum;
}

void setup(void) {
  
  Serial.begin(115200);
  
  // Configure digital output connected to rtc module.
  pinMode(RTC_POWER_PIN, OUTPUT);
  digitalWrite(RTC_POWER_PIN, HIGH);

  Wire.begin(); // I2C init as master
  RTC.begin();
  
  DateTime now = RTC.now();
  DateTime compiled = DateTime(__DATE__, __TIME__);
  if (now.unixtime() < compiled.unixtime()) {
    //Serial.println("RTC is older than compile time!  Updating");
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  digitalWrite(RTC_POWER_PIN, LOW);

  pinMode(10, OUTPUT);
  pinMode(SD_CARD_SELECT, OUTPUT);
  SD.begin(SD_CARD_SELECT);
  
  Serial.println(F("Setup complete."));
}

void loop(void) {
  
  // Log the sensor reading
  logSensorReading();

  Sleepy::loseSomeTime(LOGGING_FREQ_SECONDS * 1000);
}

