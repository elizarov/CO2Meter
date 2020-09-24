#include <WString.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "Web.h"
#include "Config.h"
#include "Data.h"
#include "CO2.h"
#include "Display.h"

Web web;

ESP8266WebServer s(80);

void handleMain();
void handleConfig();
void handleCalibration();

typedef void (*handler_t)();

enum Page { MAIN, CONFIG, CALIBRATION, N_PAGES };
char* names[] = { "Main", "Config", "Calibration" };
char* links[]= { "/", "/config", "/calibration" };
handler_t handlers[] = { &handleMain, &handleConfig, &handleCalibration };

char* CFG_NODE_ID = "nodeId";
char* CFG_TEMP_CORRECTION = "tempCorrection";

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
  m += F("------ Sensors\n");
  m += F("  CO2: "); m += dd.co2ppm.format(); m += F(" ppm\n");
  m += F("    T: "); m += dd.temp.format(); m += F(" C\n");
  m += F("    H: "); m += dd.hum.format(); m += F("%\n"); 
  m += F("\n");
  m += F("------ Network\n");
  m += F("   IP: "); m += WiFi.localIP().toString(); m += '\n';
  m += F(" RSSI: "); m += WiFi.RSSI(); m += F(" dBm\n");
  m += F("\n");
  m += F("------ Config\n");
  m += F("   ID: "); m += config.nodeId; m += '\n';
  m += F("TCorr: "); m += config.tempCorrection.format(); m += F(" C\n");
  m += F("\n");
  m += F("------ Chip\n");
  m += F("   ID: "); m += String(ESP.getChipId(), HEX); m += '\n';
  m += F(" Core: "); m += ESP.getCoreVersion(); m += '\n';
  m += F("  SDK: "); m += ESP.getSdkVersion(); m += '\n';
  m += F(" Freq: "); m += ESP.getCpuFreqMHz(); m += F(" MHz\n");
  m += F("Flash: "); m += (ESP.getFlashChipSize() >> 20); m += F(" MiB\n");
  m += F("</pre>");
  s.send(200, "text/html", m);
}

String formInputText(char* name, char* label, char* value) {
  String m;
  m += F("<label for='"); m += name; m += F("'>"); m += label; m += F(":<label><br>");
  m += F("<input type='text' id='"); m += name; m += F("' name='"); m += name; m += F("' value='"); m += value; m += F("'><br>");
  return m;
}

void handleConfig() {
  String m = navigation(CONFIG);
  if (s.method() == HTTP_POST) {
    config.setNodeId(s.arg(CFG_NODE_ID));
    config.tempCorrection = fixnum16_1::parse(s.arg(CFG_TEMP_CORRECTION).c_str());
    config.commit();
    dd.updateDHT22();
    display.update();
  }
  m += F("<form method='post'>");
  m += formInputText(CFG_NODE_ID, "Node ID", config.nodeId);
  m += formInputText(CFG_TEMP_CORRECTION, "Temp Correction", config.tempCorrection.format());
  m += F("<p><input type='submit' value='Update'>");
  m += F("</form>");
  s.send(200, "text/html", m);
}

void handleCalibration() {
  String m = navigation(CALIBRATION);
  if (s.method() == HTTP_POST) {
    bool result = false;
    if (s.hasArg("cal")) {
      m += F("Manual Calibration"); 
      result = co2.calibrate();
    } else if (s.hasArg("on")) {
      m += F("Auto Calibration: ON");
      result = co2.autoCalibrate(true);
    } else if (s.hasArg("off")) {
      m += F("Auto Calibration: OFF");
      result = co2.autoCalibrate(false);
    } else {
      m += F("Unknown");
    }
    m += F(": ");
    if (result) {
      m += F("SUCCESS");
    } else {
      m += F("FAILURE");
    }
  } else {
    m += F("<form method='post'>");
    m += F("<input type='submit' name='cal' value='Manual Calibration'>");
    m += F("<p>Auto Calibration: ");
    m += F("<input type='submit' name='on'  value='ON'>&nbsp;");
    m += F("<input type='submit' name='off' value='OFF'>");
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
