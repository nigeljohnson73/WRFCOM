#ifndef _NOW_h
#define _NOW_h

#include "App.h"

class TrNOW {
  public:
    TrNOW();

    void begin();
    void loop();

    bool isConnected();
    bool isEnabled() {
      return _enabled;
    };

  private:
    bool _enabled = false;

};

extern TrNOW NOW;

#endif
