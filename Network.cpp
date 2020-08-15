
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "Network.h"
#include "NetworkConfig.h"

Network network;
WiFiUDP udp;

void Network::setup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(networkSsid, networkPass);  
}

bool Network::sendMcast(String& packet) {
  if (WiFi.status() != WL_CONNECTED) return false;
  udp.beginPacketMulticast(mcastAddr, mcastPort, WiFi.localIP());
  udp.print(packet);
  udp.endPacket();
  return true;
}

uint8_t Network::level() {
  if (WiFi.status() != WL_CONNECTED) return 0;
  int rssi = WiFi.RSSI();
  if (rssi >= -40) return 4;
  if (rssi >= -50) return 3;
  if (rssi >= -60) return 2;
  if (rssi >= -70) return 1;
  return 0; 
}

int16_t Network::addr() {
  if (WiFi.status() != WL_CONNECTED) return -1;
  return WiFi.localIP()[3];  
}
