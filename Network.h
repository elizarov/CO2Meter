#ifndef _NETWORK_H
#define _NETWORK_H

#include <Arduino.h>

class Network {
public:
  void setup();
  bool update();
  bool sendMcast(String& packet);                    
  int32_t rssi = 0; // 0 -> No connection
  int16_t addr = -1; // last byte or -1
};

extern Network network;

#endif
