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
    void updateData();

    bool _enabled = false;
    double _percentage;
    double _voltage;
    unsigned long _last_updated;
};

extern TrBMS BMS;

#endif
