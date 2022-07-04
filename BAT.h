#ifndef _BAT_h
#define _BAT_h

#include "App.h"

class TrBAT {
  public:
    TrBAT();

    void begin();
    void loop();

	double getFill(); // as a percentage

    boolean isEnabled() {
      return _enabled;
    };

  private:
    boolean _enabled = false;
};

extern TrBAT BAT;

#endif
