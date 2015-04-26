/**
 * Test 01 - Prove that the SD Card can be initialized
 *
 *
 *
 */

#include <RFduinoBLE.h>

#include <SPI.h>
#include <SD.h>

bool sd_state = false;

void setup() {
  
  Serial.begin(9600);
  
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(10, OUTPUT);
  
  sd_state = SD.begin(6);
  if (!sd_state) {
    while (1) {
      Serial.println("could not initialize sd card");
      delay(1000);
    }
  }
}

void loop() {
  RFduino_ULPDelay(INFINITE);
}
