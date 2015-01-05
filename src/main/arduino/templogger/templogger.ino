#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#include <Wire.h>
#include <RTC_DS3231.h>

#include <OneWire.h> // update at http://www.pjrc.com/teensy/arduino_libraries/OneWire.zip

#define LOGGING_FREQ_SECONDS   10
#define MAX_SLEEP_ITERATIONS   LOGGING_FREQ_SECONDS / 8
#define ADDITIONAL_SLEEP       LOGGING_FREQ_SECONDS % 8

#define RTC_POWER_PIN          5

#define TEMP_POWER_PIN         1
#define TEMP_SIGNAL_PIN        0

RTC_DS3231 RTC;
OneWire ds(TEMP_SIGNAL_PIN);
String stationId;

int sleepIterations = 0;
volatile bool watchdogActivated = false;

// Define watchdog timer interrupt.
ISR(WDT_vect)
{
  // Set the watchdog activated flag.
  watchdogActivated = true;
}

// Put the Arduino to sleep.
void sleep()
{
  // Set sleep to full power down.  Only external interrupts or
  // the watchdog timer can wake the CPU!
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // Disable the ADC while asleep.
  power_adc_disable();

  // Enable sleep.
  sleep_enable();

  // Disable brown-out detection during sleep.  This is timing critical and
  // must be done right before entering sleep mode.
  MCUCR |= (1 << BODS) | (1 << BODSE);
  MCUCR &= ~(1 << BODSE);

  // Enter sleep mode.
  sleep_cpu();

  // CPU is now asleep and program execution completely halts!
  // Once awake, execution will resume at this point.

  // When awake, disable sleep mode and turn on all devices.
  sleep_disable();
  power_all_enable();
}

// Take a sensor reading and send it to the server.
void logSensorReading() {
  // turn on power to the rtc module
  digitalWrite(RTC_POWER_PIN, HIGH);

  // read the current time from the rtc module
  DateTime now = RTC.now();

  // turn off power to the rtc module
  digitalWrite(RTC_POWER_PIN, LOW);

  // Turn on power to the sensor and wait for it to stabilize
  digitalWrite(TEMP_POWER_PIN, HIGH);
  delay(100);

  // Take a sensor reading
  float temperature = getTemp();

  // Turn off power to the sensor.
  digitalWrite(TEMP_POWER_PIN, LOW);

  // Connect to the server and send the reading.
  // TODO write the reading and timestamp to the file
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

float getTemp() {
  //returns the temperature from one DS18S20 in DEG Celsius

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

void setup(void)
{
  Serial.begin(115200);

  //  SD.begin();

  // Configure digital output connected to rtc module.
  pinMode(RTC_POWER_PIN, OUTPUT);

  // Configure digital output connected to sensor.
  pinMode(TEMP_POWER_PIN, OUTPUT);

  // turn on power to the rtc module
  digitalWrite(RTC_POWER_PIN, HIGH);
  delay(100); // TODO necessary?

  Wire.begin(); // I2C init as master
  RTC.begin();

  // Turn off power to the rtc module.
  digitalWrite(RTC_POWER_PIN, LOW);

  // Turn off power to the sensor.
  digitalWrite(TEMP_POWER_PIN, LOW);

  // Setup the watchdog timer to run an interrupt which
  // wakes the Arduino from sleep every 8 seconds.

  // Note that the default behavior of resetting the Arduino
  // with the watchdog will be disabled.

  // This next section of code is timing critical, so interrupts are disabled.
  // See more details of how to change the watchdog in the ATmega328P datasheet
  // around page 50, Watchdog Timer.
  noInterrupts();

  // Set the watchdog reset bit in the MCU status register to 0.
  MCUSR &= ~(1 << WDRF);

  // Set WDCE and WDE bits in the watchdog control register.
  WDTCSR |= (1 << WDCE) | (1 << WDE);

  // Set watchdog clock prescaler bits to a value of 8 seconds.
  WDTCSR = (1 << WDP0) | (1 << WDP3);

  // Enable watchdog as interrupt only (no reset).
  WDTCSR |= (1 << WDIE);

  // Enable interrupts again.
  interrupts();

  Serial.println(F("Setup complete."));
}

void loop(void)
{
  // Don't do anything unless the watchdog timer interrupt has fired.
  if (watchdogActivated)
  {
    watchdogActivated = false;
    // Increase the count of sleep iterations and take a sensor
    // reading once the max number of iterations has been hit.
    sleepIterations += 1;
    if (sleepIterations >= MAX_SLEEP_ITERATIONS) {
      // Reset the number of sleep iterations.
      sleepIterations = 0;

      delay(ADDITIONAL_SLEEP * 1000);

      // Log the sensor reading
      logSensorReading();
    }
  }

  // Go to sleep!
  sleep();
}

