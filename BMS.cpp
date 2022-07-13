#include "BMS.h"
TrBMS BMS;
TrBMS::TrBMS() {};

#if !_USE_BMS_

void TrBMS::begin() {}

void TrBMS::loop() {}

double TrBMS::getCapacityPercent() {
  return 0.;
}

double TrBMS::getCapacityVoltage() {
  return 0.;
}

#else
#include "Adafruit_LC709203F.h"

Adafruit_LC709203F lc;

void TrBMS::begin() {
  if (!lc.begin()) {
#if _DEBUG_
    Serial.println("BMS disconnected");
#endif
    return;
  }

  lc.setPackSize(lc709203_adjustment_t(LIPO_SIZE));
  //lc.setAlarmVoltage(3.8);
  lc.setAlarmVoltage(0);
  lc.setAlarmRSOC(0);

#if _DEBUG_
  Serial.print("BMS initialised: 0x");
  Serial.println(lc.getICversion(), HEX);
#endif
  _enabled = true;
}

void TrBMS::loop() {
}

double TrBMS::getCapacityPercent() {
  if (!isEnabled()) return 0.;

  return lc.cellPercent();
}

double TrBMS::getCapacityVoltage() {
  if (!isEnabled()) return 0.;

  return lc.cellVoltage();
}

#endif
