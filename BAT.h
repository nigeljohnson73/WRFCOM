#ifndef _BAT_h
#define _BAT_h

#include "App.h"

class TrBAT {
  public:
    TrBAT();

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

extern TrBAT BAT;

#endif
