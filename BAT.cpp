#include "BAT.h"

#include "Adafruit_LC709203F.h"

Adafruit_LC709203F lc;
TrBAT BAT;

TrBAT::TrBAT() {};

void TrBAT::begin() {
  if (!lc.begin()) {
#if _DEBUG_
    Serial.println("BAT disconnected");
#endif
    return;
  }
  //  lc.setThermistorB(3950);
  //  Serial.print("Thermistor B = "); Serial.println(lc.getThermistorB());

  lc.setPackSize(LIPO_SIZE);
  //lc.setAlarmVoltage(3.8);
  lc.setAlarmVoltage(0);
  lc.setAlarmRSOC(0);

#if _DEBUG_
  Serial.print("BAT initialised: 0x");
  Serial.println(lc.getICversion(), HEX);
#endif
  _enabled = true;
}

void TrBAT::loop() {
}

double TrBAT::getCapacityPercent() {
  if (!isEnabled()) return 0.;

  return lc.cellPercent();
}

double TrBAT::getCapacityVoltage() {
  if (!isEnabled()) return 0.;

  return lc.cellVoltage();
}
