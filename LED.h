#ifndef _LED_h
#define _LED_h

#include "App.h"

class TrLED {
  public:
    TrLED();

    void begin();
    void loop();

    bool isEnabled() {
      return _enabled;
    };

	void blink(unsigned n) {
		_n_blips = n + 1;
	}
  private:
    bool _enabled = false;
    unsigned long _sequence_started;
    unsigned _n_blips;
    bool _blip;
    unsigned long _blip_started;
    unsigned long _sequence_duration = 1000;

    unsigned _blip_on_duration = 150;
    unsigned _blip_off_duration = 50;
    unsigned long _blip_duration;
};

extern TrLED LED;

#endif
