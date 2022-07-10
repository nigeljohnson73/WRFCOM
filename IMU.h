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

    bool hasGyro() {
      return _has_gyro;
    };

    bool hasAcc() {
      return _has_acc;
    };

    bool hasMag() {
      return _has_mag;
    };

    bool hasTemp() {
      return _has_temp;
    };

  private:
    bool _enabled = false;
    bool _has_gyro = false;
    bool _has_acc = false;
    bool _has_mag = false;
    bool _has_temp = false;
};

extern TrIMU IMU;

#endif
