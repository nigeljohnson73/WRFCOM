#ifndef _BMP_h
#define _BMP_h

#include "App.h"

class TrBMP {
  public:
    TrBMP();

    void setAltitude(double m); // will calculate sea level pressure based on this passed value
    void begin();
    void loop();
    double getPressure(); // in hPa
    double getTemperature(); // in C
    double getAltitude(); // in m

    bool isEnabled() {
      return _enabled;
    };

    double getSeaLevelPressure() {
      return sea_level_pressure;
    }; // in hPa

  private:
    bool _enabled = false;
    double sea_level_pressure = 1013.25;
};

extern TrBMP BMP;

#endif
