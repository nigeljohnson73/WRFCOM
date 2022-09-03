#include "IMU.h"
TrIMU IMU;

/************************************************************************************************************************************************************
  888888ba   .88888.  888888ba   88888888b
  88    `8b d8'   `8b 88    `8b  88
  88     88 88     88 88     88 a88aaaa
  88     88 88     88 88     88  88
  88     88 Y8.   .8P 88     88  88
  dP     dP  `8888P'  dP     dP  88888888P
*/
#if IMU_TYPE == IMU_NONE
TrIMU::TrIMU() {}

void TrIMU::begin() {
#if _DEBUG_ && _DISABLED_DEBUG_
  Serial.println("IMU init: disabled");
#endif
}

void TrIMU::loop() {};

double TrIMU::getTemperature() {
  return 0.;
}
double TrIMU::getGyroX() {
  return 0.;
}
double TrIMU::getGyroY() {
  return 0.;
}
double TrIMU::getGyroZ() {
  return 0.;
}
double TrIMU::getAccX() {
  return 0.;
}
double TrIMU::getAccY() {
  return 0.;
}
double TrIMU::getAccZ() {
  return 0.;
}
double TrIMU::getMagX() {
  return 0.;
}
double TrIMU::getMagY() {
  return 0.;
}
double TrIMU::getMagZ() {
  return 0.;
}
#endif // IMU_TYPE == IMU_NONE

/************************************************************************************************************************************************************
  dP        .d88888b  8888ba.88ba  .d8888P 888888ba  .d88888b   .88888.  d8888b. d8888b.
  88        88.    "' 88  `8b  `8b 88'     88    `8b 88.    "' d8'   `8b     `88     `88
  88        `Y88888b. 88   88   88 88baaa. 88     88 `Y88888b. 88     88  aaad8' .aaadP'
  88              `8b 88   88   88 88` `88 88     88       `8b 88     88     `88 88'
  88        d8'   .8P 88   88   88 8b. .d8 88    .8P d8'   .8P Y8.   .8P     .88 88.
  88888888P  Y88888P  dP   dP   dP `Y888P' 8888888P   Y88888P   `8888P'  d88888P Y88888P
*/
#if IMU_TYPE == IMU_LSM6DSO32 || IMU_TYPE == IMU_LSM6DSOX_LIS3MDL

#if IMU_TYPE == IMU_LSM6DSOX_LIS3MDL
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_LSM6DSOX.h>
Adafruit_LIS3MDL lis3mdl;
Adafruit_LSM6DSOX lsm6dso;
#else
#include <Adafruit_LSM6DSO32.h>
Adafruit_LSM6DSO32 lsm6dso;

// Redefine these in code to be the ones needed by the library
#define LSM6DS_ACCEL_RANGE_4_G LSM6DSO32_ACCEL_RANGE_4_G
#define LSM6DS_ACCEL_RANGE_8_G LSM6DSO32_ACCEL_RANGE_8_G
#define LSM6DS_ACCEL_RANGE_16_G LSM6DSO32_ACCEL_RANGE_16_G

#endif

sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t temp;
sensors_event_t mag;

TrIMU::TrIMU() {};

void TrIMU::begin() {
  if (!lsm6dso.begin_I2C()) {
#if _DEBUG_
    Serial.println("IMU init: LSM6DSO: disconnected");
#endif
  } else {

    // 032 has temp, acc and gyrp
    _has_temp = true;
    _has_gyro = true;
    _has_acc = true;

    for (int x = 0; x < 5; x++) {
      lsm6dso.getEvent(&accel, &gyro, &temp);
      delay(50);
    }

    lsm6dso.setAccelRange(LSM6DS_ACCEL_RANGE_4_G);
    lsm6dso.setAccelDataRate(LSM6DS_RATE_26_HZ);
    lsm6dso.setGyroRange(LSM6DS_GYRO_RANGE_500_DPS );
    lsm6dso.setGyroDataRate(LSM6DS_RATE_26_HZ);

#if _DEBUG_
    //Serial.println("LSM6DSO32 Found!");
    Serial.print("IMU init: LSM6DSO, ");
#if _UPSIDE_DOWN_
    Serial.print("USD, ");
#endif

    Serial.print("+-");
    //Serial.print("Accelerometer: ");
    switch (lsm6dso.getAccelRange()) {
      case LSM6DS_ACCEL_RANGE_4_G: Serial.print("4"); break;
      case LSM6DS_ACCEL_RANGE_8_G: Serial.print("8"); break;
      case LSM6DS_ACCEL_RANGE_16_G: Serial.print("16"); break;
#if IMU_TYPE == IMU_LSM6DS
      case LSM6DSO32_ACCEL_RANGE_32_G: Serial.print("32"); break;
#endif
      default: Serial.print("--"); break;
    }
    Serial.print("G");

    Serial.print(" @ ");
    switch (lsm6dso.getAccelDataRate()) {
      case LSM6DS_RATE_SHUTDOWN: Serial.print("0"); break;
      case LSM6DS_RATE_12_5_HZ: Serial.print("12.5"); break;
      case LSM6DS_RATE_26_HZ: Serial.print("26"); break;
      case LSM6DS_RATE_52_HZ: Serial.print("52"); break;
      case LSM6DS_RATE_104_HZ: Serial.print("104"); break;
      case LSM6DS_RATE_208_HZ: Serial.print("208"); break;
      case LSM6DS_RATE_416_HZ: Serial.print("416"); break;
      case LSM6DS_RATE_833_HZ: Serial.print("833"); break;
      case LSM6DS_RATE_1_66K_HZ: Serial.print("1666"); break;
      case LSM6DS_RATE_3_33K_HZ: Serial.print("3333"); break;
      case LSM6DS_RATE_6_66K_HZ: Serial.print("6666"); break;
      default: Serial.print("--"); break;
    }
    Serial.print("Hz");

    Serial.print(", ");
    switch (lsm6dso.getGyroRange()) {
      case LSM6DS_GYRO_RANGE_125_DPS: Serial.print("125"); break;
      case LSM6DS_GYRO_RANGE_250_DPS: Serial.print("250"); break;
      case LSM6DS_GYRO_RANGE_500_DPS: Serial.print("500"); break;
      case LSM6DS_GYRO_RANGE_1000_DPS: Serial.print("1000"); break;
      case LSM6DS_GYRO_RANGE_2000_DPS: Serial.print("2000"); break;
      case ISM330DHCX_GYRO_RANGE_4000_DPS: Serial.print("USP"); break; // unsupported range for the DSO32
      default: Serial.print("--"); break;
    }
    Serial.print("d/s");

    Serial.print(" @ ");
    switch (lsm6dso.getGyroDataRate()) {
      case LSM6DS_RATE_SHUTDOWN: Serial.print("0"); break;
      case LSM6DS_RATE_12_5_HZ: Serial.print("12.5"); break;
      case LSM6DS_RATE_26_HZ: Serial.print("26"); break;
      case LSM6DS_RATE_52_HZ: Serial.print("52"); break;
      case LSM6DS_RATE_104_HZ: Serial.print("104"); break;
      case LSM6DS_RATE_208_HZ: Serial.print("208"); break;
      case LSM6DS_RATE_416_HZ: Serial.print("416"); break;
      case LSM6DS_RATE_833_HZ: Serial.print("833"); break;
      case LSM6DS_RATE_1_66K_HZ: Serial.print("1666"); break;
      case LSM6DS_RATE_3_33K_HZ: Serial.print("3333"); break;
      case LSM6DS_RATE_6_66K_HZ: Serial.print("6666"); break;
      default: Serial.print("--"); break;
    }
    Serial.print("Hz");
    Serial.println();
#endif // DEBUG
  }

#if IMU_TYPE == IMU_LSM6DSOX_LIS3MDL

  if (! lis3mdl.begin_I2C()) {
    Serial.println("IMU init: LIS3MDL: disconnected");
  } else {

    _has_mag = true;

    for (int x = 0; x < 5; x++) {
      lis3mdl.getEvent(&mag);
      delay(50);
    }

    lis3mdl.setOperationMode(LIS3MDL_CONTINUOUSMODE);
    lis3mdl.setPerformanceMode(LIS3MDL_MEDIUMMODE);
    lis3mdl.setDataRate(LIS3MDL_DATARATE_20_HZ);
    lis3mdl.setRange(LIS3MDL_RANGE_4_GAUSS);
    lis3mdl.setIntThreshold(500);
    lis3mdl.configInterrupt(false, false, true, // enable z axis
                            true, // polarity
                            false, // don't latch
                            true); // enabled!

#if _DEBUG_
    Serial.print("IMU init: LIS3MDL, ");
#if _UPSIDE_DOWN_
    Serial.print("USD, ");
#endif

    //    Serial.print("Op: ");
    // Single shot mode will complete conversion and go into power down
    switch (lis3mdl.getOperationMode()) {
      case LIS3MDL_CONTINUOUSMODE: Serial.print("Continuous"); break;
      case LIS3MDL_SINGLEMODE: Serial.print("Single mode"); break;
      case LIS3MDL_POWERDOWNMODE: Serial.print("Power-down"); break;
      default: Serial.print("--"); break;
    }
    Serial.print(" ");

    switch (lis3mdl.getPerformanceMode()) {
      case LIS3MDL_LOWPOWERMODE: Serial.print("Low"); break;
      case LIS3MDL_MEDIUMMODE: Serial.print("Medium"); break;
      case LIS3MDL_HIGHMODE: Serial.print("High"); break;
      case LIS3MDL_ULTRAHIGHMODE: Serial.print("Ultra-High"); break;
      default: Serial.print("--"); break;
    }
    Serial.print(" Performance");

    // You can check the datarate by looking at the frequency of the DRDY pin
    Serial.print(" @ ");
    switch (lis3mdl.getDataRate()) {
      case LIS3MDL_DATARATE_0_625_HZ: Serial.print("0.625"); break;
      case LIS3MDL_DATARATE_1_25_HZ: Serial.print("1.25"); break;
      case LIS3MDL_DATARATE_2_5_HZ: Serial.print("2.5"); break;
      case LIS3MDL_DATARATE_5_HZ: Serial.print("5"); break;
      case LIS3MDL_DATARATE_10_HZ: Serial.print("10"); break;
      case LIS3MDL_DATARATE_20_HZ: Serial.print("20"); break;
      case LIS3MDL_DATARATE_40_HZ: Serial.print("40"); break;
      case LIS3MDL_DATARATE_80_HZ: Serial.print("80"); break;
      case LIS3MDL_DATARATE_155_HZ: Serial.print("155"); break;
      case LIS3MDL_DATARATE_300_HZ: Serial.print("300"); break;
      case LIS3MDL_DATARATE_560_HZ: Serial.print("560"); break;
      case LIS3MDL_DATARATE_1000_HZ: Serial.print("1000"); break;
      default: Serial.print("--"); break;
    }
    Serial.print("Hz, ");

    Serial.print("+-");
    switch (lis3mdl.getRange()) {
      case LIS3MDL_RANGE_4_GAUSS: Serial.print("4"); break;
      case LIS3MDL_RANGE_8_GAUSS: Serial.print("8"); break;
      case LIS3MDL_RANGE_12_GAUSS: Serial.print("12"); break;
      case LIS3MDL_RANGE_16_GAUSS: Serial.print("16"); break;
      default: Serial.print("--");
    }
    Serial.print(" gauss");

    Serial.println();
#endif // DEBUG
  }
#endif // IMU_TYPE == IMU_LSM6DS_LIS3MD

  _enabled = _has_temp || _has_gyro || _has_acc || _has_mag;
}

void TrIMU::loop() {
  if (!isEnabled()) return;
  lsm6dso.getEvent(&accel, &gyro, &temp);

#if IMU_TYPE == IMU_LSM6DSOX_LIS3MDL
  lis3mdl.getEvent(&mag);
#endif
}

double TrIMU::getTemperature() {
  if (!isEnabled()) return 0.;
  return temp.temperature;
}

double TrIMU::getGyroX() {
  if (!isEnabled()) return 0.;
  double dir = (_UPSIDE_DOWN_) ? -1 : 1;
  return dir * (180 / PI) * gyro.gyro.x;
}

double TrIMU::getGyroY() {
  if (!isEnabled()) return 0.;
  return (180 / PI) * gyro.gyro.y;
}

double TrIMU::getGyroZ() {
  if (!isEnabled()) return 0.;
  double dir = (_UPSIDE_DOWN_) ? -1 : 1;
  return dir * (180 / PI) * gyro.gyro.z;
}

double TrIMU::getAccX() {
  if (!isEnabled()) return 0.;
  double dir = (_UPSIDE_DOWN_) ? -1 : 1;
  return dir * accel.acceleration.x;
}

double TrIMU::getAccY() {
  if (!isEnabled()) return 0.;
  return accel.acceleration.y;
}

double TrIMU::getAccZ() {
  if (!isEnabled()) return 0.;
  double dir = (_UPSIDE_DOWN_) ? -1 : 1;
  return dir * accel.acceleration.z;
}

double TrIMU::getMagX() {
  if (!isEnabled()) return 0.;
  double dir = (_UPSIDE_DOWN_) ? -1 : 1;
  return dir * mag.magnetic.x;
}

double TrIMU::getMagY() {
  if (!isEnabled()) return 0.;
  return mag.magnetic.y;
}

double TrIMU::getMagZ() {
  if (!isEnabled()) return 0.;
  double dir = (_UPSIDE_DOWN_) ? -1 : 1;
  return dir * mag.magnetic.z;
}
#endif // IMU_TYPE == (IMU_LSM6DSO32 || IMU_LSM6DSOX_LIS3MDL)
