#ifndef _EMU_h
#define _EMU_h

#include "App.h"

class TrEMU {
  public:
    TrEMU();

    void setAltitude(double m); // will calculate sea level pressure based on this passed value
    void begin();
    void loop();
    double getPressure(); // in hPa
    double getTemperature(); // in C
    double getAltitude(); // in m

    bool isEnabled() {
      return _enabled;
    };

    bool hasPressure() {
      return _has_pressure;
    };

    bool hasTemperature() {
      return _has_temperature;
    };

    double getSeaLevelPressure() {
      return sea_level_pressure;
    }; // in hPa

  private:
    bool _enabled = false;
    bool _has_pressure = false;
    bool _has_temperature = false;
    double sea_level_pressure = 1013.25;
};

extern TrEMU EMU;

#endif
