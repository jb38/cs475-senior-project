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

Adafruit_MCP9808 tempSensor;

void setup() {
  
  Serial.begin(9600);
  
  Wire.beginOnPins(2, 3); // pins 0 and 1 are reserved for serial (when used)
  
  while(!tempSensor.begin()) {
    Serial.println("Could not initialize temp sensor");
    delay(1000);
  }
}

void loop() {
  
  tempSensor.powerUp();
  readTemp();
  tempSensor.powerDown();
  
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
    delay(300);
    tmp_readings[i] = tempSensor.readTempC();
    tmp_avg += tmp_readings[i];
  }
  
  tmp_avg /= 5.0;
}

