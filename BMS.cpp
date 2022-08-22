#include "BMS.h"
TrBMS BMS;
TrBMS::TrBMS() {};

#if !_USE_BMS_

void TrBMS::begin() {
#if _DEBUG_ && _DISABLED_DEBUG_
  Serial.println("BMS initialised: disabled");
#endif
}

void TrBMS::loop() {}

void TrBMS::updateData() {}

double TrBMS::getCapacityPercent() {
  return 0.;
}

double TrBMS::getCapacityVoltage() {
  return 0.;
}


#else
#include "Adafruit_LC709203F.h"

Adafruit_LC709203F lc;

void TrBMS::updateData() {
  if (!isEnabled()) return;
  unsigned long now = millis();
  if ((now - _last_updated) >= 500) {
    _last_updated = now;
    // Smooth and cap to within limits
    _percentage = max(0.0, min(100.0, (_percentage + lc.cellPercent()) / 2.0)); 
    // Only smooth
    _voltage = (_voltage + lc.cellVoltage()) / 2.0;
  }


}
void TrBMS::begin() {
  if (!lc.begin()) {
#if _DEBUG_
    Serial.println("BMS initialised: disconnected");
#endif
    return;
  }

  lc.setPackSize(lc709203_adjustment_t(LIPO_SIZE));
  //lc.setAlarmVoltage(3.8);
  lc.setAlarmVoltage(0);
  lc.setAlarmRSOC(0);

#if _DEBUG_
  Serial.print("BMS initialised: ");
  switch (LIPO_SIZE) {
    //  LC709203F_APA_100MAH = 0x08,
    //  LC709203F_APA_200MAH = 0x0B,
    //  LC709203F_APA_500MAH = 0x10,
    //  LC709203F_APA_1000MAH = 0x19,
    //  LC709203F_APA_2000MAH = 0x2D,
    // LC709203F_APA_3000MAH = 0x36,

    case 0x08:
      Serial.print("100mAh");
      break;

    case 0x0B:
      Serial.print("200mAh");
      break;

    case 0x10:
      Serial.print("500mAh");
      break;

    case 0x19:
      Serial.print("1000mAh");
      break;

    case 0x2D:
      Serial.print("2000mAh");
      break;

    case 0x36:
      Serial.print("3000mAh");
      break;

    default:
      Serial.print("Unknown");
      break;
  }
  Serial.print(" capacity, IC version: 0x");
  Serial.print(lc.getICversion(), HEX);
  Serial.println();
#endif
  _enabled = true;
}

void TrBMS::loop() {
}

double TrBMS::getCapacityPercent() {
  if (!isEnabled()) return 0.;
  updateData();
  return _percentage;
}

double TrBMS::getCapacityVoltage() {
  if (!isEnabled()) return 0.;
  updateData();
  return _voltage;
}

#endif
