#ifndef _RTC_h
#define _RTC_h

#include "App.h"

class TrRTC {
  public:
    TrRTC();

    void begin();
    void loop();
    String getTimestamp();
    boolean setTimestamp(String iso); // Called by the GPS every now and then


    boolean isEnabled() {
      return _enabled;
    };

  private:
    boolean _enabled = false;
};

extern TrRTC RTC;

#endif
