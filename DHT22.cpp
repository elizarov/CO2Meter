/******************************************************************
  ------ ORIGINAL SOURCE FROM ----

  DHT Temperature & Humidity Sensor library for Arduino & ESP32.

  Features:
  - Support for DHT11 and DHT22/AM2302/RHT03
  - Auto detect sensor model
  - Very low memory footprint
  - Very small code

  https://github.com/beegee-tokyo/arduino-DHTesp

  Written by Mark Ruys, mark@paracas.nl.
  Updated to work with ESP32 by Bernd Giesecke, bernd@giesecke.tk

  GNU General Public License, check LICENSE for more information.
  All text above must be included in any redistribution.
******************************************************************/

#include "DHT22.h"

const unsigned long INITIAL_TIMEOUT = 500;
const unsigned long READING_TIMEOUT = 2500; // minimum interval between readings

DHT22::DHT22(uint8_t pin) : _pin(pin), _updateTimeout(INITIAL_TIMEOUT) {}

bool DHT22::update() {
  if (!_updateTimeout.check()) return false;
  _updateTimeout.reset(READING_TIMEOUT);

  temp.clear();
  hum.clear();

  uint16_t rawHum = 0;
  uint16_t rawTemp = 0;
  uint16_t data = 0;

  // Request sample
  digitalWrite(_pin, LOW); // Send start signal
  pinMode(_pin, OUTPUT);
  delay(2);

  pinMode(_pin, INPUT);
  digitalWrite(_pin, HIGH); // Switch bus to receive data

  // We're going to read 83 edges:
  // - First a FALLING, RISING, and FALLING edge for the start bit
  // - Then 40 bits: RISING and then a FALLING edge per bit
  // To keep our code simple, we accept any HIGH or LOW reading if it's max 85 usecs long

  noInterrupts();

  for (int8_t i = -3; i < 2 * 40; i++) {
    unsigned long age;
    unsigned long startTime = micros();
    do {
      age = (unsigned long)(micros() - startTime);
      if (age > 90) {
        error = ERROR_TIMEOUT;
        interrupts();
        return true;
      }
    } while (digitalRead(_pin) == (i & 1) ? HIGH : LOW);

    if (i >= 0 && (i & 1)) {
      // Now we are being fed our 40 bits
      data <<= 1;
      // A zero max 30 usecs, a one at least 68 usecs.
      if (age > 30) {
        data |= 1; // we got a one
	    }
    }
    switch (i) {
      case 31:
        rawHum = data;
        data = 0;
        break;
      case 63:
        rawTemp = data;
        data = 0;
        break;
    }
  }
  interrupts();

  // Verify checksum
  if ((byte)(((byte)rawHum) + (rawHum >> 8) + ((byte)rawTemp) + (rawTemp >> 8)) != data) {
    error = ERROR_CHECKSUM;
    return true;
  }

  // Store readings
  hum = fixnum16_1(rawHum);
  temp = (rawTemp & 0x8000) ? fixnum16_1(-(int16_t)(rawTemp & 0x7FFF)) : fixnum16_1(rawTemp);
  error = ERROR_NONE;
  return true;
}

char DHT22::state() {
  return error == ERROR_CHECKSUM ? '!' : ' ';
}
