#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306_Wemos_Mini_OLED.h>
#include <Fonts/FreeMonoBold12pt7b.h>

#include <Timeout.h>
#include <DrawRSSI.h>

#include "Data.h"
#include "Config.h"
#include "OTA.h"
#include "Display.h"

// SCL GPIO5
// SDA GPIO4
Adafruit_SSD1306 d;

Display display;

const unsigned long MIN_SHOW_TIME = 100;
unsigned long prevShowTime = 0;

void Display::setup() {
  d.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  d.setRotation(2);
  show(SETUP_DISPLAY);
}

void displayNodeId() {
  d.setCursor(0, 0);
  d.print(config.nodeId);
}

void Display::show(SetupPhase phase) {
  d.clearDisplay();
  d.setTextColor(WHITE); 
  displayNodeId(); 
  d.setCursor(0, 16);
  if (phase <= SETUP_DONE) {
    d.print(F("SETUP:"));
  } else {
    d.print(F("UPDATE: "));
  }
  d.setCursor(0, 24);
  switch (phase) {
    case SETUP_DISPLAY: d.print(F("DISPLAY")); break;
    case SETUP_NETWORK: d.print(F("NETWORK")); break;
    case SETUP_OTA:     d.print(F("OTA")); break;
    case SETUP_WEB:     d.print(F("WEB")); break;
    case SETUP_CO2:     d.print(F("CO2")); break;
    case SETUP_DONE:    d.print(F("DONE")); break;
    case UPDATE_TEMP:   d.print(F("TEMP")); break;
    case UPDATE_CO2:    d.print(F("CO2")); break;
  }
  d.setCursor(0, 32);
  d.print(phase, DEC);
  d.print('/');
  d.print(UPDATE_DONE - 1, DEC);
  d.display();
  unsigned long now = millis();
  unsigned long since = now - prevShowTime;
  if (since < MIN_SHOW_TIME) delay(MIN_SHOW_TIME - since);
  prevShowTime = now;
}

void displaySensors() {
  // CO2 PPM num
  if (dd.co2ppm.valid()) {  
    d.setFont(&FreeMonoBold12pt7b);
    d.setCursor(2, 35);
    d.print(dd.co2ppm.format());
    d.setFont();

    // CO2 PPM label
    d.setCursor(0, 10);
    d.print(F("CO2"));
    d.setCursor(40, 10);
    d.print(F("ppm"));
  }
  
  // temperature & humidity
  if (dd.temp.valid()) {
    d.setCursor(6, 40);
    d.print(dd.temp.format());
  }
  if (dd.hum.valid()) {
    d.setCursor(40, 40);
    d.print(dd.hum.format());
    d.print('%');
  }

  // Blinking dot or sensor state char
  if (dd.blinkState) { 
    if (dd.blinkPacket) {
      d.drawCircle(2, 43, 2, WHITE);
    } else {
      d.fillCircle(2, 44, 2, WHITE);
    }
  } else {
    d.setCursor(0, 40);
    d.print(dd.state);
  }
}

void displayOTA() {
  d.setCursor(0, 16);
  d.print(F("FLASH:"));
  d.setCursor(0, 24);
  d.print(ota.progress, DEC);
  d.print('%');
  d.setCursor(0, 32);
  switch (ota.status) {
    case OTA_DONE: d.print(F("DONE")); break;
    case OTA_ERROR: d.print(F("ERROR")); break;
  }
}

void Display::update() {
  d.clearDisplay();
  d.setTextColor(WHITE);
  displayNodeId();

  // Dipslay OTA status or sensors
  if (ota.status != OTA_NONE) {
    displayOTA();
  } else {
    displaySensors();
  }
  
  // addr
  if (dd.addr >= 0) {
    d.setCursor(25, 0);
    d.print('.');
    d.print(dd.addr, DEC);
  }

  // RSSI
  drawRSSI(d, 53, 0, dd.rssi, WHITE);

  d.display();
}
