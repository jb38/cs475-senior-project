#include <JeeLib.h>

#include <Wire.h>
#include <RTC_DS3231.h>

#include <OneWire.h> // update at http://www.pjrc.com/teensy/arduino_libraries/OneWire.zip

#include <SPI.h>
#include <SD.h>

#include <Adafruit_MCP9808.h>

#define LOGGING_FREQ_SECONDS   5

#define RTC_POWER_PIN          A3
RTC_DS3231 RTC;

#define TEMP_SIGNAL_PIN        8
#define TEMP2_SIGNAL_PIN       6

OneWire ds(TEMP_SIGNAL_PIN);
OneWire ds2(TEMP2_SIGNAL_PIN);

Adafruit_MCP9808 temp3 = Adafruit_MCP9808();

#define SD_CARD_SELECT         9
String stationId;

// Define watchdog timer interrupt.
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

// Take a sensor reading and send it to the server.
void logSensorReading() {
  
  DateTime now = getTime();
  float temperature2 = getTemp(ds2);
  float temperature = getTemp(ds);
  float temperature3 = temp3.readTempC();
  
  
  
  File file;
  if (file = SD.open("data2.txt", FILE_WRITE)) {
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
    file.print('\t');
    file.print(temperature, 3);
    file.print('\t');
    file.print(temperature2, 3);        
    file.print('\t');
    file.println(temperature3, 3);
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
  Serial.print('\t');
  Serial.print(temperature, 3);
  Serial.print('\t');
  Serial.print(temperature2, 3);        
  Serial.print('\t');
  Serial.println(temperature3, 3);

  // Note that if you're sending a lot of data you
  // might need to tweak the delay here so the Serial has
  // time to finish sending all the data before shutdown.
  delay(100);
}

DateTime getTime() {
  digitalWrite(RTC_POWER_PIN, HIGH);
  DateTime now = RTC.now();
  digitalWrite(RTC_POWER_PIN, LOW);
  return now;
}

float getTemp(OneWire d) { //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if (!d.search(addr)) {
    //no more sensors on chain, reset search
    d.reset_search();
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

  d.reset();
  d.select(addr);
  d.write(0x44, 1); // start conversion, with parasite power on at the end

  delay(750); // required wait time for parasitic power

  byte present = ds.reset();
  d.select(addr);
  d.write(0xBE); // Read Scratchpad

  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = d.read();
  }

  d.reset_search();

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
  
  temp3.begin();
  
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

  long start = millis();
  
  // Log the sensor reading
  logSensorReading();
  
  long finish = millis();

  delay(LOGGING_FREQ_SECONDS * 1000 - (finish - start));

  //Sleepy::loseSomeTime(LOGGING_FREQ_SECONDS * 1000 - (finish - start));
}

