 
#include <FixNum.h>
#include <Timeout.h>

#include "Config.h"
#include "Data.h"
#include "Network.h"
#include "OTA.h"
#include "Web.h"
#include "Display.h"
#include "DHT22.h"
#include "CO2.h"

// ------------- blink -------------

const unsigned long BLINK_TIMEOUT = 1000;
Timeout blinkTimeout(BLINK_TIMEOUT);
bool sentPacket = false;

bool blinkUpdate() {
  if (!blinkTimeout.check()) return false;
  blinkTimeout.reset(BLINK_TIMEOUT);
  dd.blinkState = !dd.blinkState;
  if (dd.blinkState) {
    dd.blinkPacket = sentPacket;
    sentPacket = false;
  }
  return true;
}

// ------------- Multicast -------------

unsigned long daystart = 0;
int16_t updays = 0;
char uptimeBuf[11];
//   0123456789
//   ddddhhmmss

bool mcast() {
  String packet = F("[");
  packet += config.nodeId;
  packet += ':';
  if (dd.co2ppm.valid()) {
    packet += 'c';
    packet += dd.co2ppm.format();
  }
  if (dd.temp.valid()) {
    packet += 't';
    packet += dd.temp.format();
  }
  if (dd.hum.valid()) {
    packet += 'h';
    packet += dd.hum.format();
  }
  if (dd.rssi != 0) {
    packet += 'r';
    packet += String(dd.rssi, DEC);
  }
  // uptime
  packet += 'u';
  unsigned long time = millis();
  while (time - daystart > Timeout::DAY) {
    daystart += Timeout::DAY;
    updays++;
  }
  formatDecimal(updays, &uptimeBuf[0], 4, FMT_ZERO | FMT_RIGHT);
  time -= daystart;
  time /= 1000; // convert seconds
  formatDecimal((int8_t)(time % 60), &uptimeBuf[8], 2, FMT_ZERO | FMT_RIGHT);
  time /= 60; // minutes
  formatDecimal((int8_t)(time % 60), &uptimeBuf[6], 2, FMT_ZERO | FMT_RIGHT);
  time /= 60; // hours
  formatDecimal((int8_t)time, &uptimeBuf[4], 2, FMT_ZERO | FMT_RIGHT);
  uptimeBuf[10] = 0;
  packet += uptimeBuf;
  // done
  packet += ']';
  return network.sendMcast(packet);
}

// ------------- Main -------------

void setup() {
  digitalWrite(LED_BUILTIN, 1);
  config.setup();
  display.setup();
  display.show(SETUP_NETWORK);
  network.setup();
  display.show(SETUP_OTA);
  ota.setup();
  display.show(SETUP_WEB);
  web.setup();
  display.show(SETUP_CO2);
  co2.setup();
  display.show(SETUP_DONE);
}

bool firstLoop = true;

void loop() {
  // Blink
  bool blinkUpdated = blinkUpdate();

  // Networking subsystems
  bool networkUpdated = network.update();
  ota.update();
  web.update();

  // network state
  dd.rssi = network.rssi;  
  dd.addr = network.addr;
  
  // DHT22
  if (firstLoop) display.show(UPDATE_TEMP);
  bool dht22updated = dht22.update();
  if (dht22updated) dd.updateDHT22();

  // CO2
  if (firstLoop) display.show(UPDATE_CO2);
  bool co2updated = co2.update();
  if (co2updated) dd.updateCO2();

  // sensor(s) error state
  dd.state = dht22.state();
  if (dd.state == ' ') dd.state = co2.state;

  // send packet on each CO2 data update or network appearance
  if (co2updated || networkUpdated && network.addr >= 0) {
    sentPacket = mcast();
  }

  // Display
  if (firstLoop || networkUpdated || blinkUpdated || dht22updated || co2updated) {
    display.update();
  }
  if (firstLoop) {
    firstLoop = false;
    digitalWrite(LED_BUILTIN, 0);
  }
}
