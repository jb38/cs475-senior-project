#include <SPI.h>
#include <SD.h>
//#include <stdlib.h>
#include <RFduinoBLE.h>
#include <Wire.h>
#include <RTC_DS3231.h>

#include <OneWire.h> // http://forum.rfduino.com/index.php?topic=103.msg261#msg261

// Wire.beginOnPins(2, 3); // Starts the I2C interface with SCL on GPIO 2 and SDA on GPIO 3

#define LOGGING_FREQ_SECONDS   300

#define RTC_POWER_PIN          2u
RTC_DS3231 RTC;

#define TEMP_SIGNAL_PIN        6u
OneWire ds(TEMP_SIGNAL_PIN);

void setup() {
  
  RFduinoBLE.deviceName = "Hood-CS475";
  RFduinoBLE.advertisementData = "data";
  RFduinoBLE.advertisementInterval = 1000; // 1s
  RFduinoBLE.txPowerLevel = -20;
  RFduinoBLE.begin();
  
  Wire.beginOnPins(1u, 0u);
}

void loop() {
  // put your main code here, to run repeatedly:


  if (!RFduinoBLE.radioActive) {
    RFduino_ULPDelay(SECONDS(LOGGING_FREQ_SECONDS));
  }
}

void RFduinoBLE_onConnect(){
// Insert code
}

void RFduinoBLE_onDisconnect(){
// Insert code here
}

void RFduinoBLE_onReceive(char *data, int len){
  uint8_t myByte = data[0]; // store first char in array to myByte
  Serial.println(myByte); // print myByte via serial
  
  // GetStationIdentifier
  if (strcmp(data, "GetStationIdentifer") == 0) {
    getStationIdentifer()
  } 
  // GetAllObservations
  else if (strcmp(data, "GetAllObservations") == 0) {
    getAllObservations()
  } 
  // GetSystemState
  else if (strcmp(data, "GetSystemState") == 0) {
    getSystemState()
  } 
}

void getStationIdentifer() {
  // RFduinoBLE.send(const char *data, int len);
  
}

void getAllObservations() {
  
}

void getSystemState() {
  
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
