#ifndef _NETWORK_H
#define _NETWORK_H

#include <Arduino.h>

class Network {
public:
  void setup();
  bool update();
  bool sendMcast(String& packet);                    
  uint8_t level = 0; // 0-4
  int16_t addr = -1; // last byte or -1
};

extern Network network;

#endif
