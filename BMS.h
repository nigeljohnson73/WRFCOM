#ifndef _BMS_h
#define _BMS_h

#include "App.h"

class TrBMS {
  public:
    TrBMS();

    void begin();
    void loop();

    double getCapacityPercent();
    double getCapacityVoltage();

    bool isEnabled() {
      return _enabled;
    };

  private:
    bool _enabled = false;
};

extern TrBMS BMS;

#endif
