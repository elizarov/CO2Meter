#ifndef _DATA_H
#define _DATA_H

#include <Arduino.h>
#include <FixNum.h>

class Data {
public:
  fixnum16_0 co2ppm;
  fixnum16_1 temp;
  fixnum16_0 hum;
  char state = ' ';
  uint8_t level = 0; // 0-4
  int16_t addr = -1; // last byte
  bool blinkState = true;
  bool blinkPacket = false;
};

extern Data dd;

#endif
