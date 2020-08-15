#include <WString.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "Web.h"
#include "Data.h"
#include "CO2.h"

Web web;

ESP8266WebServer s(80);

void handleMain();
void handleCalibration();

typedef void (*handler_t)();

enum Page { MAIN, CALIBRATION, N_PAGES };
char* names[] = { "Main", "Calibration" };
char* links[]= { "/", "/calibration" };
handler_t handlers[] = { &handleMain, &handleCalibration };

String navigation(Page page) {
  String m = F("<!DOCTYPE html>");
  m += F("<h1>CO2 Meter - "); m += names[page]; m += F("</h1>\n");
  for (int i = 0; i < N_PAGES; i++) {
    m += F("[ "); 
    if (i != page) { m += F("<a href='"); m += links[i]; m += F("'>"); }
    m += names[i];
    if (i != page) { m += F("</a>"); }
    m += F(" ] ");
  }
  m += "<br>\n<p>";
  return m;
}

void handleMain() {
  String m = navigation(MAIN);
  m += F("<pre>\n");
  m += F(" CO2: "); m += dd.co2ppm.format(); m += F(" ppm\n");
  m += F("   T: "); m += dd.temp.format(); m += F(" C\n");
  m += F("   H: "); m += dd.hum.format(); m += F("%\n"); 
  m += F("\n");
  m += F("  IP: "); m += WiFi.localIP().toString(); m += F("\n");
  m += F("RSSI: "); m += WiFi.RSSI(); m += F(" dBm\n");
  m += F("</pre>");
  s.send(200, "text/html", m);
}

void handleCalibration() {
  String m = navigation(CALIBRATION);
  if (s.method() == HTTP_POST) {
    m += F("Calibration: "); 
    if (co2.calibrate()) {
      m += F("SUCCESS");
    } else {
      m += F("FAILURE");
    }
  } else {
    m += F("<form method='post'>");
    m += F("<input type='submit' value='Calibrate'>");
    m += F("</form>");
  }
  s.send(200, "text/html", m);
}

void handleNotFound() {
  s.send(404, "text/plain", "Not found");
}


void Web::setup() {
  for (int i = 0; i < N_PAGES; i++) s.on(links[i], handlers[i]);
  s.onNotFound(handleNotFound);
  s.begin();
}

void Web::update() {
  s.handleClient();
}
