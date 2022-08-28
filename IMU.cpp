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
  Serial.println("IMU initialised: disabled");
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
#if IMU_TYPE == IMU_LSM6DSO32
#include <Adafruit_LSM6DSO32.h>

Adafruit_LSM6DSO32 dso32;
sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t temp;

TrIMU::TrIMU() {};

void TrIMU::begin() {
  if (!dso32.begin_I2C()) {
    // if (!dso32.begin_SPI(LSM_CS)) {
    // if (!dso32.begin_SPI(LSM_CS, LSM_SCK, LSM_MISO, LSM_MOSI)) {
#if _DEBUG_
    Serial.println("IMU initialised: disconnected");
#endif
    return;
  }
  _enabled = true;
  // 032 has temp, acc and gyrp
  _has_temp = true;
  _has_gyro = true;
  _has_acc = true;

  for (int x = 0; x < 5; x++) {
    dso32.getEvent(&accel, &gyro, &temp);
    delay(50);
  }

#if _DEBUG_
  //Serial.println("LSM6DSO32 Found!");
  Serial.print("IMU initialised: LSM6DSO32, ");

  dso32.setAccelRange(LSM6DSO32_ACCEL_RANGE_4_G);
  //Serial.print("Accelerometer: ");
  switch (dso32.getAccelRange()) {
    case LSM6DSO32_ACCEL_RANGE_4_G:
      Serial.print("+-4G");
      break;
    case LSM6DSO32_ACCEL_RANGE_8_G:
      Serial.print("+-8G");
      break;
    case LSM6DSO32_ACCEL_RANGE_16_G:
      Serial.print("+-16G");
      break;
    case LSM6DSO32_ACCEL_RANGE_32_G:
      Serial.print("+-32G");
      break;
  }
  dso32.setAccelDataRate(LSM6DS_RATE_26_HZ);
  Serial.print(" @ ");
  switch (dso32.getAccelDataRate()) {
    case LSM6DS_RATE_SHUTDOWN:
      Serial.print("0 Hz");
      break;
    case LSM6DS_RATE_12_5_HZ:
      Serial.print("12.5 Hz");
      break;
    case LSM6DS_RATE_26_HZ:
      Serial.print("26 Hz");
      break;
    case LSM6DS_RATE_52_HZ:
      Serial.print("52 Hz");
      break;
    case LSM6DS_RATE_104_HZ:
      Serial.print("104 Hz");
      break;
    case LSM6DS_RATE_208_HZ:
      Serial.print("208 Hz");
      break;
    case LSM6DS_RATE_416_HZ:
      Serial.print("416 Hz");
      break;
    case LSM6DS_RATE_833_HZ:
      Serial.print("833 Hz");
      break;
    case LSM6DS_RATE_1_66K_HZ:
      Serial.print("1.66 KHz");
      break;
    case LSM6DS_RATE_3_33K_HZ:
      Serial.print("3.33 KHz");
      break;
    case LSM6DS_RATE_6_66K_HZ:
      Serial.print("6.66 KHz");
      break;
  }

  dso32.setGyroRange(LSM6DS_GYRO_RANGE_125_DPS );
  Serial.print(", ");
  switch (dso32.getGyroRange()) {
    case LSM6DS_GYRO_RANGE_125_DPS:
      Serial.print("125 d/s");
      break;
    case LSM6DS_GYRO_RANGE_250_DPS:
      Serial.print("250 d/s");
      break;
    case LSM6DS_GYRO_RANGE_500_DPS:
      Serial.print("500 d/s");
      break;
    case LSM6DS_GYRO_RANGE_1000_DPS:
      Serial.print("1000 d/s");
      break;
    case LSM6DS_GYRO_RANGE_2000_DPS:
      Serial.print("2000 d/s");
      break;
    case ISM330DHCX_GYRO_RANGE_4000_DPS:
      break; // unsupported range for the DSO32
  }

  dso32.setGyroDataRate(LSM6DS_RATE_26_HZ);
  Serial.print(" @ ");
  switch (dso32.getGyroDataRate()) {
    case LSM6DS_RATE_SHUTDOWN:
      Serial.print("0 Hz");
      break;
    case LSM6DS_RATE_12_5_HZ:
      Serial.print("12.5 Hz");
      break;
    case LSM6DS_RATE_26_HZ:
      Serial.print("26 Hz");
      break;
    case LSM6DS_RATE_52_HZ:
      Serial.print("52 Hz");
      break;
    case LSM6DS_RATE_104_HZ:
      Serial.print("104 Hz");
      break;
    case LSM6DS_RATE_208_HZ:
      Serial.print("208 Hz");
      break;
    case LSM6DS_RATE_416_HZ:
      Serial.print("416 Hz");
      break;
    case LSM6DS_RATE_833_HZ:
      Serial.print("833 Hz");
      break;
    case LSM6DS_RATE_1_66K_HZ:
      Serial.print("1.66 KHz");
      break;
    case LSM6DS_RATE_3_33K_HZ:
      Serial.print("3.33 KHz");
      break;
    case LSM6DS_RATE_6_66K_HZ:
      Serial.print("6.66 KHz");
      break;
  }
  //  Serial.print(", ");
  //  Serial.print(getTemperature());
  //  Serial.print(" C");

  Serial.println();
#endif

}

void TrIMU::loop() {
  if (!isEnabled()) return;
  dso32.getEvent(&accel, &gyro, &temp);
}

double TrIMU::getTemperature() {
  if (!isEnabled()) return 0.;
  return temp.temperature;
}

double TrIMU::getGyroX() {
  if (!isEnabled()) return 0.;
  return (180 / PI) * gyro.gyro.x;
}

double TrIMU::getGyroY() {
  if (!isEnabled()) return 0.;
  return (180 / PI) * gyro.gyro.y;
}

double TrIMU::getGyroZ() {
  if (!isEnabled()) return 0.;
  return (180 / PI) * gyro.gyro.z;
}

double TrIMU::getAccX() {
  if (!isEnabled()) return 0.;
  return accel.acceleration.x;
}

double TrIMU::getAccY() {
  if (!isEnabled()) return 0.;
  return accel.acceleration.y;
}

double TrIMU::getAccZ() {
  if (!isEnabled()) return 0.;
  return accel.acceleration.z;
}

double TrIMU::getMagX() {
  if (!isEnabled()) return 0.;
  return 0;
}

double TrIMU::getMagY() {
  if (!isEnabled()) return 0.;
  return 0;
}

double TrIMU::getMagZ() {
  if (!isEnabled()) return 0.;
  return 0;
}
#endif // IMU_TYPE == IMU_LSM6DSO32
