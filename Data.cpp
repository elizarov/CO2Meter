#include "Data.h"
#include "Config.h"
#include "CO2.h"
#include "DHT22.h"

Data dd;

void Data::updateCO2() {
  co2ppm = co2.ppm;
}

void Data::updateDHT22() {
  temp = dht22.temp + config.tempCorrection;
  hum = dht22.hum;
}
