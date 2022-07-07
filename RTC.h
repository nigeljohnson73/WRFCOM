#ifndef _RTC_h
#define _RTC_h

#include "App.h"

class TrRTC {
  public:
    TrRTC();

    void begin();
    void loop();
    String getTimestamp();
    bool setTimestamp(String iso); // Called by the GPS every now and then


    bool isEnabled() {
      return _enabled;
    };

  private:
    bool _enabled = false;
};

extern TrRTC RTC;

#endif
