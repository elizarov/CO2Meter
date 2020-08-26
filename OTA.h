#ifndef _OTA_H
#define _OTA_H

enum OTAStatus {
  OTA_NONE,
  OTA_ACTIVE,
  OTA_ERROR,
  OTA_DONE
};

class OTA {
public:
  void setup();
  void update();
  OTAStatus status = OTA_NONE;
  int progress = -1;
private:
  bool connected = false;
};

extern OTA ota;

#endif
