#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMonoBold12pt7b.h>

#include <Timeout.h>

#include "Data.h"
#include "Display.h"

// SCL GPIO5
// SDA GPIO4
Adafruit_SSD1306 d;

Display display;

void Display::setup() {
  d.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  d.setRotation(2);
  d.clearDisplay();
  d.display();
}

void Display::update() {
  d.clearDisplay();
  d.setTextColor(WHITE);
  
  // CO2 PPM num
  if (dd.co2ppm.valid()) {  
    d.setFont(&FreeMonoBold12pt7b);
    d.setCursor(2, 35);
    d.print(dd.co2ppm.format());
    d.setFont();

    // CO2 PPM label
    d.setCursor(0, 10);
    d.print("CO2");
    d.setCursor(40, 10);
    d.print("ppm");
  }
  
  // temperature & humidity
  if (dd.temp.valid()) {
    d.setCursor(6, 40);
    d.print(dd.temp.format());
  }
  if (dd.hum.valid()) {
    d.setCursor(40, 40);
    d.print(dd.hum.format());
    d.print("%");
  }

  // Sensor state char
  d.setCursor(0, 40);
  d.print(dd.state);

  // RSSI
  for (uint8_t i = 1; i <= dd.level; i++) {
    d.fillRect(3 * i - 3, 8 - 2 * i, 2, 2 * i, WHITE);
  }

  // addr
  if (dd.addr >= 0) {
    d.setCursor(32, 0);
    d.print(".");
    d.print(dd.addr, DEC);
  }

  // blinking dot
  if (dd.blinkState) {
    if (dd.blinkPacket) {
      d.drawCircle(60, 3, 2, WHITE);
    } else {
      d.fillCircle(60, 3, 2, WHITE);
    }
  }

  d.display();
}
