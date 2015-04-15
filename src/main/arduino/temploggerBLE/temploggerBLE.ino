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
#define LOGGING_FREQUENCY_MINUTES              20
#define BLE_REMAINS_ON_AFTER_POWER_ON_MINUTES  15
#define TIME_ZONE_OFFSET_HOURS                 -4
#define I2C_BUS_POWER                           2

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
float readings[5];
float average;

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
  DateTime now = getTime();
  logReading(now);
}

void loop() {
  
  // grab the start time (used to calculate sleep duration)
  long start = millis();
  
  // get the current time for calculations and the like
  DateTime now = getTime();
  
  // log a temperature reading every N minutes
  if (++temp_log_interval >= LOGGING_FREQUENCY_MINUTES) {
    temp_log_interval = 0;
    logReading(now);
  }
  
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

bool shouldBleBeEnabled(DateTime dt) {

  // off on weekends
  int weekday = dt.dayOfWeek();  
  if (weekday == 0 || weekday == 6) {
    return false; 
  }
  
  // off between 1730 and 0900 (local)
  int hour = (24 + dt.hour() + TIME_ZONE_OFFSET_HOURS) % 24;
  int minutes = hour * 60 + dt.minute();
  if (540 <= minutes && minutes <= 1050) {
    return (minutes % 10 == 0); // only on when the minute ends with '0'
  }
  
  // off if no positive condition is satisfied
  return false; 
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

void logReading(DateTime now) {
  
  tempSensor.powerUp();

  average = 0;
  for (int i = 0; i < 5; i++) {
    RFduino_ULPDelay(300); // must wait for the sensor to refresh the temperature value
    readings[i] = tempSensor.readTempC();
    average += readings[i];
  }
  
  tempSensor.powerDown();

  average /= 5.0;
  
  File file;
  if (file = SD.open("data.csv", FILE_WRITE)) {
    
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
    file.print("Z,");

    for (int i = 0; i < 5; i++) {
      file.print(readings[i], 4);
      file.print(',');
    }
    file.println(average, 4);
    // TODO checksum?
    
    file.close(); // implicit flush
  }
}

void RFduinoBLE_onConnect(){
  is_connected = true;
}

void RFduinoBLE_onDisconnect(){
  is_connected = false;
}

void RFduinoBLE_onReceive(char *data, int len){

  if (strncmp(data, "Echo", 4) == 0) {
    RFduinoBLE.send(data, len);
  } else if (strncmp(data, "GetTemperature", 14) == 0) {
    tempSensor.powerUp();
    delay(300); // required conversion time is 250ms
    float temperature = tempSensor.readTempC();
    tempSensor.powerDown();
    RFduinoBLE.sendFloat(temperature);
  } else if (strncmp(data, "GetTime", 7) == 0) {
    // get the time from the i2c device
    digitalWrite(I2C_BUS_POWER, HIGH);
    DateTime now = RTC.now();
    digitalWrite(I2C_BUS_POWER, LOW);
    // write it out
    RFduinoBLE.sendInt(now.unixtime());
  } else if (strncmp(data, "SetTime", 7) == 0) {
    // get the time from the payload
    // write it to the i2c device
    char time [len - 7];
    for (int i = 0, il = len - 7; i < il; i++) {
      time[i] = data[i + 7];
    }
    int theTime = String(time).toInt();
    RTC.adjust(DateTime(theTime));
    RFduinoBLE.sendInt(theTime);
  } else if (strncmp(data, "GetStationId", 12) == 0) {
    RFduinoBLE.send(station_id, 7);
  } else if (strncmp(data, "BeginDataTransfer", 17) == 0) {
    // bulk transfer of the data
    xfer_file = SD.open("data.csv");
    if (xfer_file) {
      is_xfer = true; 
      int xfer_size = xfer_file.size();
      RFduinoBLE.sendInt(xfer_size);
    } else {
      is_xfer = false;
      RFduinoBLE.sendInt(0);
    }
  } else if (strncmp(data, "ContinueDataTransfer", 20) == 0) {
    // bulk transfer of the data
    char buf[32];
    int len = 0;
    for (int i = 0; i < 32; i++) {
       tmp_int = xfer_file.read();
       if (tmp_int == -1) {
         xfer_file.close();
         break;
       } else {
         buf[i] = tmp_int;
         len += 1;
       }
    }
    while (!RFduinoBLE.send(buf, len));
  } else if (strncmp(data, "GetBattery", 10) == 0) {
    // http://forum.rfduino.com/index.php?topic=265.0
    analogReference(VBG); // Sets the Reference to 1.2V band gap           
    analogSelection(VDD_1_3_PS);  //Selects VDD with 1/3 prescaling as the analog source
    int sensorValue = analogRead(1); // the pin has no meaning, it uses VDD pin
    float batteryVoltage = sensorValue * (3.6 / 1023.0); // convert value to voltage
    analogReference(DEFAULT); // switch back to default reference
    RFduinoBLE.sendFloat(batteryVoltage);
  }
}
