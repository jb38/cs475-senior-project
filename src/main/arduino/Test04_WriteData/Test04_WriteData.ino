/**
 * Test 04 - Prove that data can be written to the sd card
 *
 *
 *
 */

#include <RFduinoBLE.h>

#include <SPI.h>
#include <SD.h>

bool sd_state = false;

File tmp_file;
char tmp_char;

char station_id [7];

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
  
  tmp_file = SD.open("id.txt", FILE_READ);
  if (!tmp_file) {
    while (1) {
      Serial.println("could not open file");
      delay(1000);
    } 
  }
  
  for (int i = 0; i < 7; i++) {
    tmp_char = tmp_file.read();
    station_id[i] = tmp_char;
  }
  tmp_file.close();
  
  RFduinoBLE.deviceName = station_id;
  RFduinoBLE.advertisementData = "-logger";
  RFduinoBLE.advertisementInterval = SECONDS(1);
  RFduinoBLE.txPowerLevel = -20;
  RFduinoBLE.begin();
}

void loop() {
  RFduino_ULPDelay(INFINITE);
}

void RFduinoBLE_onAdvertisement(bool start)
{
  tmp_file = SD.open("advlog.txt", FILE_WRITE);
  
  tmp_file.print(millis(), DEC);
  tmp_file.print(',');
  tmp_file.println((int)start, DEC);
  
  tmp_file.close();
}
