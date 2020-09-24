#pragma once

enum SetupPhase {
  SETUP_DISPLAY,
  SETUP_NETWORK,
  SETUP_OTA,
  SETUP_WEB,
  SETUP_CO2,
  SETUP_DONE,
  UPDATE_TEMP,
  UPDATE_CO2,
  UPDATE_DONE
};

class Display {
public:
  void setup();
  void show(SetupPhase phase);
  void update();
};

extern Display display;
