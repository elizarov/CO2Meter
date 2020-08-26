 
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

bool mcast() {
  String packet = F("[");
  packet += config.nodeId;
  packet += ':';
  bool f = false;
  if (dd.co2ppm.valid()) {
    packet += 'c';
    packet += dd.co2ppm.format();
    f = true;
  }
  if (dd.temp.valid()) {
    packet += 't';
    packet += dd.temp.format();
    f = true;
  }
  if (dd.hum.valid()) {
    packet += 'h';
    packet += dd.hum.format();
    f = true;
  }
  packet += ']';
  if (!f) return false;
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
  dd.level = network.level;  
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
