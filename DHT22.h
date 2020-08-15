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

#ifndef _DTH22_H
#define _DTH22_H

#include <Arduino.h>
#include <FixNum.h>
#include <Timeout.h>

class DHT22 {
public:
  DHT22(uint8_t pin);
  
  typedef enum {
    ERROR_NONE = 0,
    ERROR_TIMEOUT,
    ERROR_CHECKSUM
  }
  DHT_ERROR_t;

  DHT_ERROR_t error = ERROR_NONE;

  fixnum16_1 temp;
  fixnum16_1 hum;

  bool update();
  char state();

private:
  uint8_t _pin;
  Timeout _updateTimeout;
};

extern DHT22 dht22;

#endif
