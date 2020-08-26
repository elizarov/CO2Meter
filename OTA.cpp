#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "OTA.h"
#include "Display.h"

OTA ota;

void OTA::setup() {
  ArduinoOTA.onStart([&]() {
    status = OTA_ACTIVE;
    progress = 0;
    display.update();
  });
  ArduinoOTA.onEnd([&]() {
    status = OTA_DONE;
    display.update();
  });
  ArduinoOTA.onProgress([&](unsigned int done, unsigned int total) {
    int newProgress = ((unsigned long)done * 100) / total;
    if (newProgress != progress) {
      progress = newProgress;
      display.update();
    }
  });
  ArduinoOTA.onError([&](ota_error_t error) {
    status = OTA_ERROR;
    display.update();
    ESP.restart();
  });
}

void OTA::update() {
  if (connected) {
    ArduinoOTA.handle();      
  } else {
    if (WiFi.status() == WL_CONNECTED) {
      connected = true;
      ArduinoOTA.begin();
    }
  }
}
