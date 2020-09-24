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
  int32_t rssi = 0; // 0 -> No connection
  int16_t addr = -1; // last byte
  bool blinkState = true;
  bool blinkPacket = false;

  void updateCO2();
  void updateDHT22();
};

extern Data dd;

#endif
