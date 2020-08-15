 
#include <FixNum.h>
#include <Timeout.h>

#include "Data.h"
#include "Network.h"
#include "Display.h"
#include "DHT22.h"

DHT22 dht22(D0);

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

// ------------- CO2 -------------

const unsigned long CO2_INITIAL_DELAY = 500;
const unsigned long CO2_TIMEOUT = 10000;

uint8_t co2cmd[9] = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};
uint8_t co2resp[9];
fixnum16_0 co2ppm;
char co2state;
Timeout co2Timeout(CO2_INITIAL_DELAY);

bool co2Update() {
  if (!co2Timeout.check()) return false;
  co2Timeout.reset(CO2_TIMEOUT);
  co2ppm.clear();
  co2state = ' ';
  while (Serial.available()) Serial.read();
  Serial.write(co2cmd, 9);
  size_t n = Serial.readBytes(co2resp, 9);
  if (n < 9) return true;
  uint8_t checksum = 0;
  for (int i = 1; i <= 8; i++) checksum += co2resp[i];
  if (co2resp[0] == 0xFF && co2resp[1] == 0x86 && checksum == 0) {
    co2ppm = fixnum16_0((((uint16_t)co2resp[2]) << 8) + (uint16_t)co2resp[3]);
  } else {
    co2state = '*';
  }
  return true;
}

// ------------- Main -------------

void setup() {
  display.setup();
  network.setup();
  Serial.begin(9600);
  Serial.swap();
  Serial.setTimeout(2000);
}

bool mcast() {
  String packet = "[";
  if (dd.co2ppm.valid()) {
    packet += "c";
    packet += dd.co2ppm.format();
  }
  if (dd.temp.valid()) {
    packet += "t";
    packet += dd.temp.format();
  }
  if (dd.hum.valid()) {
    packet += "h";
    packet += dd.hum.format();
  }
  packet += "]";
  if (packet == String("[]")) return false;
  return network.sendMcast(packet);
}

void loop() {
  // Blink
  bool blinkUpdated = blinkUpdate();

  // DHT22
  bool dht22updated = dht22.update();
  if (dht22updated) {
    dd.temp = dht22.temp;
    dd.hum = dht22.hum;
  }

  // CO2
  bool co2updated = co2Update();
  if (co2updated) {
    dd.co2ppm = co2ppm; 
  }

  // sensor(s) error state
  char state = dht22.state();
  if (state == ' ') state = co2state;
  dd.state = state;

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
