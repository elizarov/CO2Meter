#include <Arduino.h>
#include <ESP_EEPROM.h>
#include "Config.h"

Config config;

Config::Config() : tempCorrection(0) {}

void Config::setup() {
  EEPROM.begin(sizeof(Config));
  if (EEPROM.percentUsed() >= 0) {
    EEPROM.get(0, config);
  } else {
    String id(ESP.getChipId() & 0xFFFF, HEX);
    setNodeId(id);
  }
}

void Config::commit() {
  EEPROM.put(0, config);
  EEPROM.commit();
}


void Config::setNodeId(String s) {
  strncpy(nodeId, s.c_str(), NODE_ID_MAX_LEN);
  nodeId[NODE_ID_MAX_LEN] = 0;  
}
