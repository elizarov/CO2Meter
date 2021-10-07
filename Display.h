#pragma once

enum SetupPhase {
  SETUP_DISPLAY,
  SETUP_NETWORK,
  SETUP_OTA,
  SETUP_WEB,
  SETUP_CO2,
  SETUP_DONE,
};

class Display {
public:
  void setup();
  void show(SetupPhase phase);
  void update(bool updated = true);
};

extern Display display;
