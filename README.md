# CO2 Meter

DIY simple CO2 concentration + Temperature & Humidity sensor with OLED display, HTTP web server and UDP data multicast.

Hardware:

* WeMos D1 mini ESP8266 board.
* DHT22 Shield (patched by cutting D4 data trace).
* 48x64 OLED Shield.
* MH-Z19B CO2 Sensor.

Conections:

```text
                     +--------------+
                     | WeMos D1mini | RESET
                     |               \
  MH-Z19B power ---- + 5V       3.3V + ---- OLED & DHT22 power
  common ground ---- + G          D8 + ---- Serial2 TX  \ connects to MH-Z19B UART
                     + D4         D7 + ---- Serial2 RX  /
                     + D3         D6 +
 OLED I2C / SDA ---- + D2         D5 +
          \ SDL ---- + D1         D0 + ---- Patched to DHT22 Shield data
                     + RX         A0 +
                     + TX        RST +
                     |               |
                     +---------------+
```

Note: my board is "LOLIN(WEMOS) D1 mini (clone)" is Arduino EPS core 3.1.2