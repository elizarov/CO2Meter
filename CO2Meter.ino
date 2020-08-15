 
#include <FixNum.h>
#include <Timeout.h>

#include "Config.h"
#include "Data.h"
#include "Network.h"
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
  config.setup();
  display.setup();
  network.setup();
  web.setup();
  co2.setup();
}

void loop() {
  // Web server
  web.update();
  
  // Blink
  bool blinkUpdated = blinkUpdate();

  // DHT22
  bool dht22updated = dht22.update();
  if (dht22updated) dd.updateDHT22();

  // CO2
  bool co2updated = co2.update();
  if (co2updated) dd.updateCO2();

  // sensor(s) error state
  dd.state = dht22.state();
  if (dd.state == ' ') dd.state = co2.state;

  // network
  uint8_t oldLevel = dd.level;
  dd.level = network.level();  
  dd.addr = network.addr();
  bool networkOn = oldLevel == 0 && dd.level != 0;

  // send packet on each CO2 data update or network appearance
  if (co2updated || networkOn) {
    sentPacket = mcast();
  }

  // Display
  if (blinkUpdated || dht22updated || co2updated || networkOn) {
    display.update();
  }
}
