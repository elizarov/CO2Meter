#ifndef _CO2_H
#define _CO2_H

#include <Arduino.h>
#include <FixNum.h>

// MH-Z19B CO2 sensor connected to ESP8266 Serial2 (RX GPIO13, TX GPIO15)

class CO2 {
public:
  fixnum16_0 ppm;
  char state = ' ';

  void setup();
  bool update();
  bool calibrate();  
  bool autoCalibrate(bool on);
};

extern CO2 co2;

#endif
