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

  private:
    boolean _enabled = false;
};

extern TrSRV SRV;

#endif
