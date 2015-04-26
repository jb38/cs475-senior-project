/**
 * Test 04 - Prove that data can be written to the sd card
 *
 *
 *
 */

#include <RFduinoBLE.h>

#include <Wire.h>
#include <Adafruit_MCP9808.h>

float tmp_readings[5];
float tmp_avg;

//Adafruit_MCP9808 tempSensor;

void setup() {
  
  Serial.begin(9600);
  
  Wire.beginOnPins(2, 3); // pins 0 and 1 are reserved for serial (when used)
  
  uint8_t man_id = i2cread(MCP9808_I2CADDR_DEFAULT, MCP9808_REG_MANUF_ID);
  uint8_t dev_id = i2cread(MCP9808_I2CADDR_DEFAULT, MCP9808_REG_DEVICE_ID);
  
  while (1) {
    Serial.print("Manufacturer: ");
    Serial.println(man_id);
    Serial.print("Device: ");
    Serial.println(dev_id);
    delay(2000);
  }
  
  while(!tempSensor.begin()) {
    Serial.println("Could not initialize temp sensor");
    delay(1000);
  }
}

void loop() {
  
  readTemp();
  
  for (int i = 0; i < 5; i++) {
    Serial.print(tmp_readings[i], 4);
    Serial.print(',');
  }
  
  Serial.print("**,");
  Serial.println(tmp_avg, 4);
  
  RFduino_ULPDelay(SECONDS(1));
}

void readTemp() {
  
  tmp_avg = 0;
  
  for (int i = 0; i < 5; i++) {
    tmp_readings[i] = tempSensor.readTempC();
    tmp_avg += tmp_readings[i];
    delay(10);
  }
  
  tmp_avg /= 5.0;
}

