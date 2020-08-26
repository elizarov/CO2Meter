
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <Timeout.h>

#include "Network.h"
#include "NetworkConfig.h"

Network network;
WiFiUDP udp;

// reset if not connected after timeout
const unsigned long NETWORK_TIMEOUT = 10000; 
Timeout networkTimeout;

void Network::setup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(networkSsid, networkPass);  
  networkTimeout.reset(NETWORK_TIMEOUT);
}

uint8_t getLevel() {
  int rssi = WiFi.RSSI();
  if (rssi >= -40) return 4;
  if (rssi >= -50) return 3;
  if (rssi >= -60) return 2;
  if (rssi >= -70) return 1;
  return 0; 
}

bool Network::update() {
  bool wasConnected = addr >= 0;
  bool isConnected = WiFi.status() == WL_CONNECTED;
  if (isConnected) {
    level = getLevel();
    addr = WiFi.localIP()[3]; 
  } else {
    level = 0;
    addr = -1;
    if (wasConnected) {
      networkTimeout.reset(NETWORK_TIMEOUT);
    }
    if (networkTimeout.check()) {
      ESP.reset();
    }
  }
  return wasConnected != isConnected;
}

bool Network::sendMcast(String& packet) {
  if (WiFi.status() != WL_CONNECTED) return false;
  udp.beginPacketMulticast(mcastAddr, mcastPort, WiFi.localIP());
  udp.print(packet);
  udp.endPacket();
  return true;
}
