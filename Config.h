#ifndef _CONFIG_H
#define _CONFIG_H

#include <Arduino.h>
#include <FixNum.h>

const size_t NODE_ID_MAX_LEN = 4;

struct Config {
  char nodeId[NODE_ID_MAX_LEN + 1];
  char reserved1[11];
  fixnum16_1 tempCorrection;
  char reserved2[14];

  Config();
  void setup();
  void commit();
  void setNodeId(String s);
};

extern Config config;

#endif
