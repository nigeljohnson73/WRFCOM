#ifndef _SRV_h
#define _SRV_h

#include "App.h"

class TrSRV {
  public:
    TrSRV();

    void begin();
    void loop();

    bool isEnabled() {
      return _enabled;
    };

    bool isArmed() {
      return _armed;
    };

    void arm(bool tf);

  private:
    bool _enabled = false;
    bool _armed = true;
};

extern TrSRV SRV;

#endif
