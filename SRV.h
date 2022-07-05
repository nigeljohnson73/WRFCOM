#ifndef _SRV_h
#define _SRV_h

#include "App.h"

class TrSRV {
  public:
    TrSRV();

    void begin();
    void loop();

    boolean isEnabled() {
      return _enabled;
    };

    boolean isArmed() {
      return _armed;
    };

    void arm(boolean tf);

  private:
    boolean _enabled = false;
    boolean _armed = true;
};

extern TrSRV SRV;

#endif
