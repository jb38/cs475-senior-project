/*

The MIT License (MIT)

Copyright (c) 2015 Jim Blaney

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "observation.h"

// RFduino
#include <RFduinoBLE.h>

// I2C
#include <Wire.h>
#include <RTC_DS3231.h>
#include <Adafruit_MCP9808.h>

// SPI
#include <SPI.h>
#include <SD.h>

/**
 * configuration parameters
 */
#define LOGGING_FREQUENCY_MINUTES               1
#define BLE_REMAINS_ON_AFTER_POWER_ON_MINUTES  15
#define TIME_ZONE_OFFSET_HOURS                 -4
#define I2C_BUS_POWER                           2
#define TEMPERATURE_SAMPLE_READINGS             5

/**
 * sensor variables
 */
RTC_DS3231 RTC;
Adafruit_MCP9808 tempSensor;

/**
 * state variables
 */
bool is_connected = false;
bool is_xfer = false;
bool is_ble_on = false;

/**
 * timing variables
 */
int ble_enabled_on_start_minutes = BLE_REMAINS_ON_AFTER_POWER_ON_MINUTES;
int temp_log_interval = 0;

/**
 * temporary variables
 */
File tmp_file;
File xfer_file;
int tmp_int;
char tmp_char;
char station_id [9];

char buff [20];
int bytes_read;
int bytes_remaining;
int num_pages;
int len;

/**
 * functions
 */
void setup() {
  
  // initialize the RTC module
  pinMode(I2C_BUS_POWER, OUTPUT);
  digitalWrite(I2C_BUS_POWER, HIGH);
  delay(5);
  Wire.begin();
  RTC.begin();
  digitalWrite(I2C_BUS_POWER, LOW);
  
  // initialize the temperature sensor
  tempSensor.begin();
  delay(5);
  tempSensor.powerDown();

  // initialize the SD card
  pinMode(10, OUTPUT);
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);
  SD.begin(SS);

  // prepare BLE and start
  setupBLE();
  startBLE();
  
  // log the first set of temperature readings
  logObservation(getObservation());
}

void loop() {
  
  //while (RFduinoBLE.radioActive);
  
  // grab the start time (used to calculate sleep duration)
  long start = millis();
  
  // log a temperature reading every N minutes
  if (++temp_log_interval >= LOGGING_FREQUENCY_MINUTES) {
    temp_log_interval = 0;
    logObservation(getObservation());
  }
  
  // get the current time for calculations and the like
  DateTime now = getTime();
  
  // determine whether or not to start/stop bluetooth and do so
  if (ble_enabled_on_start_minutes > 0) {
    ble_enabled_on_start_minutes--;
  } else if (is_connected) {
    // do nothing if connected
  } else if (shouldBleBeEnabled(now) && !is_ble_on) {
    startBLE();
  } else if (is_ble_on) {
    stopBLE();
  }
  
  // sleep until the next minute comes around
  long sleepDuration = MINUTES(1) - (millis() - start);
  if (sleepDuration > 0) {
    RFduino_ULPDelay(sleepDuration);
  }
}

void startBLE() {
  
  is_ble_on = true;
  RFduinoBLE.begin(); 
}

void stopBLE() {
  
  is_ble_on = false;
  RFduinoBLE.end();
}

void setupBLE() {
  
  // read the station id from the file
  if (SD.exists("id.txt")) {
    File file = SD.open("id.txt", FILE_READ);
    
    char stationId[9] = { 'B', 'O', 'O', 'S', 'T', '0', '0', '0', 0 };
    for (int i = 0; i < 8; i++) {
      if (file.available()) {
        stationId[i] = file.read();
      }
    }
    file.close();
    RFduinoBLE.deviceName = stationId;
  } else {
    RFduinoBLE.deviceName = "BOOST999";
  }
  
  RFduinoBLE.advertisementData = "Hood";
  RFduinoBLE.advertisementInterval = SECONDS(5);
  RFduinoBLE.txPowerLevel = -20; // lowest power option
}

DateTime getTime() {
  
  digitalWrite(I2C_BUS_POWER, HIGH);
  DateTime now = RTC.now();
  digitalWrite(I2C_BUS_POWER, LOW);
 
  return now; 
}

Observation getObservation() {
  
  DateTime now = getTime();
  float readings[TEMPERATURE_SAMPLE_READINGS];
  float average;
  float battery_voltage = getBatteryVoltage(); // get the battery voltage
    
  tempSensor.powerUp();
  average = 0;
  for (int i = 0; i < TEMPERATURE_SAMPLE_READINGS; i++) {
    RFduino_ULPDelay(300); // must wait for the sensor to refresh the temperature value
    readings[i] = tempSensor.readTempC();
    average += readings[i];
  }
  tempSensor.powerDown();
  average /= (TEMPERATURE_SAMPLE_READINGS * 1.0); // get the temperature
  
  Observation obs = { now.unixtime(), battery_voltage, average };
  return obs;
}

void logObservation(Observation obs) {
  
  File file;
  if (file = SD.open("data.csv", FILE_WRITE)) {

    file.println(formatObservation(obs));
    
    file.close();
  }
}

String formatObservation(Observation obs) {
  
  String msg = "$OBS," + 
               String(obs.time, DEC) + 
               "," + 
               String(int(obs.battery_voltage)) + "." + String(getDecimal(obs.battery_voltage)) +
               "," + 
               String(int(obs.temperature)) + "." + String(getDecimal(obs.temperature)) +
               "*";
               
  msg += String(crc(msg), HEX);
  
  return msg;
}

// http://www.arduino-hacks.com/float-to-string-float-to-character-array-arduino/
long getDecimal(float val)
{
   int intPart = int(val);
   long decPart = 1000 * (val - intPart);         //I am multiplying by 1000 assuming that the foat values will have a maximum of 3 decimal places
                                                  //Change to match the number of decimal places you need
   if (decPart > 0) return (decPart);             //return the decimal part of float number if it is available 
   else if (decPart < 0) return ((-1) * decPart); //if negative, multiply by -1
   else if (decPart = 0) return (00);             //return 0 if decimal part of float number is not available
}

void RFduinoBLE_onConnect()    { is_connected = true;  }
void RFduinoBLE_onDisconnect() { is_connected = false; }

void RFduinoBLE_onReceive(char *data, int len) {

  if (strncmp(data, "Echo", 4) == 0) {
    
    RFduinoBLE.send(data, len);
    
  } else if (strncmp(data, "GetObservation", 14) == 0) {
    
    String msg = formatObservation(getObservation());
    len = msg.length();
    bytes_remaining = len;
    num_pages = len / 20 + (len % 20 > 0 ? 1 : 0);
    for (int p = 0; p < num_pages; p++) {
      int page_len = min(20, bytes_remaining);
      for (int i = 0; i < page_len; i++) {
        buff[i] = msg[i + p * 20];
      }
      while (!RFduinoBLE.send(buff, page_len));
      bytes_remaining -= page_len;
    }
        
  } else if (strncmp(data, "SetTime", 7) == 0) {
    
    char time [len - 7];
    for (int i = 0, il = len - 7; i < il; i++) {
      time[i] = data[i + 7];
    }
    int theTime = String(time).toInt();
    RTC.adjust(DateTime(theTime));
    RFduinoBLE.sendInt(theTime);
    
  } else if (strncmp(data, "TransferData", 12) == 0) {

    // bulk transfer of the data
    xfer_file = SD.open("data.csv", FILE_READ);
    if (xfer_file) {
      is_xfer = true;
      
      String str = String(xfer_file.size(), DEC);
      len = str.length();
      for (int i = 0; i < len; i++) {
        buff[i] = str[i]; 
      }
      if (buff[0] == '0' && len == 1) {
        is_xfer = false;
        xfer_file.close(); 
      }
      RFduinoBLE.send(buff, len);
    } else {
      buff[0] = '0';
      RFduinoBLE.send(buff, 1);
    }
  } else if (strncmp(data, "ContinueData", 12) == 0) {
     bytes_read = xfer_file.readBytes(buff, 20);
     if (bytes_read > 0) {
       RFduinoBLE.send(buff, bytes_read);
     } 
     if (bytes_read < 20) {
       xfer_file.close();
       is_xfer = false;
     }
  }
}

