#ifndef _BUT_h
#define _BUT_h

#include "App.h"

class TrBUT {
  public:
    TrBUT();

    void begin();
    void loop();

    bool isEnabled() {
      return _enabled;
    };

  private:
    bool _enabled = false;

    bool _last_pressed = false;
    bool _button_held = false;
    unsigned long _last_pressed_at;
    const long _held_trigger = 1000;

    uint8_t _brightness = 250; //The maximum brightness of the pulsing LED. Can be between 0 (min) and 255 (max)
    uint16_t _cycleTime = 1000;//The total time for the pulse to take. Set to a bigger number for a slower pulse, or a smaller number for a faster pulse
    uint16_t _offTime = 0;     //The total time to stay off between pulses. Set to 0 to be pulsing continuously.

};

extern TrBUT BUT;

#endif
