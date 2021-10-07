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

// periodic short display of static labels (less burn in)
const unsigned long STATIC_DISPLAY_TIME = 3000;
Timeout staticTimeout(0);
bool staticDisplay = false;


// random shift (less burn in) -- for use with new screens
uint8_t dx;
uint8_t dy;
const unsigned long SHIFT_TIMEOUT = 3000;
Timeout shiftTimeout(SHIFT_TIMEOUT);

void randomize() {
  if (!shiftTimeout.check()) return;
  shiftTimeout.reset(SHIFT_TIMEOUT);
  dx = random(2);
  dy = random(2);  
}

void Display::setup() {
  d.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  d.setRotation(2);
  // decrease brightness & contrast (less burn in)
  d.ssd1306_command(SSD1306_SETVCOMDETECT);
  d.ssd1306_command(0x20); 
  d.ssd1306_command(SSD1306_SETCONTRAST);
  d.ssd1306_command(0x20); 
  show(SETUP_DISPLAY);
}

void displayNodeId() {
  d.setCursor(dx + 0, dy + 0);
  d.print(config.nodeId);
}

void Display::show(SetupPhase phase) {
  d.clearDisplay();
  d.setTextColor(WHITE); 
  displayNodeId(); 
  d.setCursor(0, 16);
  d.print(F("SETUP:"));
  d.setCursor(0, 24);
  switch (phase) {
    case SETUP_DISPLAY: d.print(F("DISPLAY")); break;
    case SETUP_NETWORK: d.print(F("NETWORK")); break;
    case SETUP_OTA:     d.print(F("OTA")); break;
    case SETUP_WEB:     d.print(F("WEB")); break;
    case SETUP_CO2:     d.print(F("CO2")); break;
    case SETUP_DONE:    d.print(F("DONE")); break;
  }
  d.setCursor(0, 32);
  d.print(phase, DEC);
  d.print('/');
  d.print(SETUP_DONE - 1, DEC);
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
    d.setCursor(dx + 2, dy + 35);
    d.print(dd.co2ppm.format());
    d.setFont();

    // CO2 PPM label
    if (staticDisplay) {
      d.setCursor(dx + 0, dy + 10);
      d.print(F("CO2"));
      d.setCursor(dx + 40, dy + 10);
      d.print(F("ppm"));
    }  
  }
  
  // temperature & humidity
  if (dd.temp.valid()) {
    d.setCursor(dx + 6, dy + 40);
    d.print(dd.temp.format());
  }
  if (dd.hum.valid()) {
    d.setCursor(dx + 40, dy + 40);
    d.print(dd.hum.format());
    d.print('%');
  }

  // Blinking dot or sensor state char
  if (dd.blinkState) { 
    if (dd.blinkPacket) {
      d.drawCircle(dx + 2, dy + 43, 2, WHITE);
    } else {
      d.fillCircle(dx + 2, dy + 44, 2, WHITE);
    }
  } else {
    d.setCursor(dx + 0, dy + 40);
    d.print(dd.state);
  }
}

void displayOTA() {
  d.setCursor(dx + 0, dy + 16);
  d.print(F("FLASH:"));
  d.setCursor(dx + 0, dy + 24);
  d.print(ota.progress, DEC);
  d.print('%');
  d.setCursor(dx + 0, dy + 32);
  switch (ota.status) {
    case OTA_DONE: d.print(F("DONE")); break;
    case OTA_ERROR: d.print(F("ERROR")); break;
  }
}

void Display::update(bool updated) {
// use this with fresh display (not yet burned in)  
//  randomize();

  if (updated) {
    staticTimeout.reset(STATIC_DISPLAY_TIME);
    staticDisplay = true;
  } else if (staticTimeout.check()) {
    staticDisplay = false;
  }
  
  d.clearDisplay();
  d.setTextColor(WHITE);

  // Dipslay OTA status or sensors
  if (ota.status != OTA_NONE) {
    displayOTA();
  } else {
    displaySensors();
  }
  
  // addr
  if (staticDisplay && dd.addr >= 0) {
    d.setCursor(dx + 25, dy + 0);
    d.print('.');
    d.print(dd.addr, DEC);
  }

  // RSSI
  if (staticDisplay) {
    displayNodeId();
    drawRSSI(d, dx + 53, dy + 0, dd.rssi, WHITE);
  }

  d.display();
}
