#ifndef _IMU_h
#define _IMU_h

#include "App.h"

class TrIMU {
  public:
    TrIMU();

    void begin();
    void loop();

    double getTemperature(); // in C
    double getGyroX(); // in degrees per second
    double getGyroY(); // in degrees per second
    double getGyroZ(); // in degrees per second
    double getAccelerationX(); // in metres per second per second
    double getAccelerationY(); // in metres per second per second
    double getAccelerationZ(); // in metres per second per second
    double getMagX(); // in ... something
    double getMagY(); // in ... something
    double getMagZ(); // in ... something

    bool isEnabled() {
      return _enabled;
    };

  private:
    bool _enabled = false;
};

extern TrIMU IMU;

#endif
