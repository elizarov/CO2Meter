#include "CO2.h"
#include "Timeout.h"

CO2 co2;

const unsigned long UPDATE_TIMEOUT = 10000;

uint8_t readCmd[9]             = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};
uint8_t calibrateCmd[9]        = {0xFF,0x01,0x87,0x00,0x00,0x00,0x00,0x00,0x78};
uint8_t autoCalibrateOnCmd[9]  = {0xFF,0x01,0x79,0xA0,0x00,0x00,0x00,0x00,0xE6};
uint8_t autoCalibrateOffCmd[9] = {0xFF,0x01,0x79,0x00,0x00,0x00,0x00,0x00,0x86};
uint8_t resp[9];
Timeout timeout(0);
bool wasCalibrated = false;

void CO2::setup() {
  Serial.begin(9600);
  Serial.swap();
  Serial.setTimeout(2000);
}

enum SendResult { SEND_OK, SEND_TIMEOUT, SEND_ERROR };

SendResult sendReceive(uint8_t* cmd) {
  while (Serial.available()) Serial.read();
  Serial.write(cmd, 9);
  size_t n = Serial.readBytes(resp, 9);
  if (n < 9) return SEND_TIMEOUT;
  uint8_t checksum = 0;
  for (int i = 1; i <= 8; i++) checksum += resp[i];
  return (resp[0] == 0xFF && resp[1] == cmd[2] && checksum == 0) ? SEND_OK : SEND_ERROR;
}

bool CO2::update() {
  if (!timeout.check()) return false;
  timeout.reset(UPDATE_TIMEOUT);
  ppm.clear();
  state = ' ';
  if (wasCalibrated) {
    wasCalibrated = false;
    state = '^';
    return true; // wait one more cycle before reading
  }
  switch (sendReceive(readCmd)) {
    case SEND_OK:
      ppm = fixnum16_0((((uint16_t)resp[2]) << 8) + (uint16_t)resp[3]);
      break;
    case SEND_ERROR:  
      state = '*';
  }
  return true;
}

bool CO2::calibrate() {
  timeout.reset(0);
  wasCalibrated = true;
  return sendReceive(calibrateCmd) == SEND_OK;
}

bool CO2::autoCalibrate(bool on) {
  return sendReceive(on ? autoCalibrateOnCmd : autoCalibrateOffCmd) == SEND_OK;
}
