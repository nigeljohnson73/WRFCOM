#include "BLE.h"
#include "BLE_SVC.h"

TrBLE BLE;
TrBLE::TrBLE() {};

#if !_USE_BLE_

void TrBLE::begin() {}
void TrBLE::loop() {}

#else

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
//#include <BLE2902.h>
#include <BLE2904.h>

// Used for connection monitoring
bool deviceConnected = false;
bool oldDeviceConnected = false;

// The main server
BLEServer* pServer = NULL;

// Core characteristics storage
BLECharacteristic* pWifi = NULL;
BLECharacteristic* pIpaddr = NULL;
BLECharacteristic* pBattery = NULL;
BLECharacteristic* pArmed = NULL;
BLECharacteristic* pLogging = NULL;

// Capability characteristics storage
BLECharacteristic* pCPressure = NULL;
BLECharacteristic* pCTemperature = NULL;
BLECharacteristic* pCAccelerometer = NULL;
BLECharacteristic* pCGyroscope = NULL;
BLECharacteristic* pCMagnetometer = NULL;
BLECharacteristic* pBmsEnabled = NULL;
BLECharacteristic* pGpsEnabled = NULL;
BLECharacteristic* pGpsLocked = NULL;
BLECharacteristic* pEmuEnabled = NULL;
BLECharacteristic* pImuEnabled = NULL;
BLECharacteristic* pSrvEnabled = NULL;
BLECharacteristic* pRtcEnabled = NULL;

// GPS characteristics storage
BLECharacteristic* pSiv = NULL;
BLECharacteristic* pLatitude = NULL;
BLECharacteristic* pLongitude = NULL;
BLECharacteristic* pAltitude = NULL;

// EMU characteristics storage
BLECharacteristic* pPressure = NULL;
BLECharacteristic* pTemperature = NULL;

// IMU characteristics storage
BLECharacteristic* pAccX = NULL;
BLECharacteristic* pAccY = NULL;
BLECharacteristic* pAccZ = NULL;
BLECharacteristic* pGyroX = NULL;
BLECharacteristic* pGyroY = NULL;
BLECharacteristic* pGyroZ = NULL;
BLECharacteristic* pMagX = NULL;
BLECharacteristic* pMagY = NULL;
BLECharacteristic* pMagZ = NULL;

// Core descriptors storage
BLE2904* pBle2904_wifi = NULL;
BLE2904* pBle2904_ipaddr = NULL;
BLE2904* pBle2904_battery = NULL;

// Capability descriptors storage
BLE2904* pBle2904_ctemperature = NULL;
BLE2904* pBle2904_cpressure = NULL;
BLE2904* pBle2904_caccelerometer = NULL;
BLE2904* pBle2904_cgyroscope = NULL;
BLE2904* pBle2904_cmagnetometer = NULL;
BLE2904* pBle2904_bms_enabled = NULL;
BLE2904* pBle2904_gps_enabled = NULL;
BLE2904* pBle2904_gps_locked = NULL;
BLE2904* pBle2904_emu_enabled = NULL;
BLE2904* pBle2904_imu_enabled = NULL;
BLE2904* pBle2904_srv_enabled = NULL;
BLE2904* pBle2904_rtc_enabled = NULL;

// GPS descriptors storage
BLE2904* pBle2904_siv = NULL;
BLE2904* pBle2904_latitude = NULL;
BLE2904* pBle2904_longitude = NULL;
BLE2904* pBle2904_altitude = NULL;

// EMU descriptors storage
BLE2904* pBle2904_pressure = NULL;
BLE2904* pBle2904_temperature = NULL;

// IMU descriptors storage
BLE2904* pBle2904_acc_x = NULL;
BLE2904* pBle2904_acc_y = NULL;
BLE2904* pBle2904_acc_z = NULL;
BLE2904* pBle2904_gyro_x = NULL;
BLE2904* pBle2904_gyro_y = NULL;
BLE2904* pBle2904_gyro_z = NULL;
BLE2904* pBle2904_mag_x = NULL;
BLE2904* pBle2904_mag_y = NULL;
BLE2904* pBle2904_mag_z = NULL;

// Core data values
String wifi_value = "STA";
String ipaddr_value = "192.168.999.999";
uint8_t battery_value = int(12.34);
uint8_t armed_value = true ? 1 : 0;
uint8_t logging_value = true ? 1 : 0;

// Capability data values
uint8_t ctemperature_value = true ? 1 : 0;
uint8_t cpressure_value = true ? 1 : 0;
uint8_t caccelerometer_value = false ? 1 : 0;
uint8_t cgyroscope_value = true ? 1 : 0;
uint8_t cmagnetometer_value = false ? 1 : 0;
uint8_t bms_enabled_value = true ? 1 : 0;
uint8_t gps_enabled_value = true ? 1 : 0;
uint8_t gps_locked_value = false ? 1 : 0;
uint8_t emu_enabled_value = true ? 1 : 0;
uint8_t imu_enabled_value = true ? 1 : 0;
uint8_t srv_enabled_value = true ? 1 : 0;
uint8_t rtc_enabled_value = true ? 1 : 0;

// GPS data values
uint8_t siv_value = 0;
int latitude_value = int(51.012323 * 10000000.0);
int longitude_value = (0.4612323 * 10000000.0);
int altitude_value = int(1.234 * 1000.0); //int(23.441*1000);

// EMU data values
int pressure_value = int(1013.25 * 1000.0);
int temperature_value = int(18.12 * 100.0);

// IMU data values
int accx_value = int(0.1 * 10000.);
int accy_value = int(-0.1 * 10000.);
int accz_value = int(9.79 * 10000.);
int gyrox_value = int(0.1 * 10000.);
int gyroy_value = int(-0.1 * 10000.);
int gyroz_value = int(0.0 * 10000.);
int magx_value = int(123.45 * 100.);
int magy_value = int(-123.45 * 100.);
int magz_value = int(1.234 * 100.);

// Class to handle writes to the arm control value
class MyArmCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("BLE::armCallback():");
        Serial.print(value[0] ? "TRUE" : "FALSE");
        Serial.println();
        SRV.arm(value[0] ? true : false);
      }
    }
};

// Class to handle writes to the logging control value
class MyLogCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("BLE::logCallback():");
        Serial.print(value[0] ? "TRUE" : "FALSE");
        Serial.println();
        LOG.capture(value[0] ? true : false);
      }
    }
};

// Class to hand the connection events
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      BLEDevice::startAdvertising();
      Serial.println("Device connected");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Device disconnected");
    }
};

void TrBLE::begin() {

  //  String server_name = DEVICE_NAME;//String(DEVICE_NAME) + "-" + espChipId();

  BLEDevice::init(DEVICE_NAME.c_str());
  if (false) {
    Serial.println("BLE disconnected");
    return;
  }

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pCoreService = pServer->createService(BLEUUID(WRFCOM_CORE_SERVICE_UUID), 60);
  BLEService *pCapabilityService = pServer->createService(BLEUUID(WRFCOM_CAPABILITY_SERVICE_UUID), 60);
  BLEService *pGpsService = pServer->createService(BLEUUID(WRFCOM_GPS_SERVICE_UUID), 60);
  BLEService *pEmuService = pServer->createService(BLEUUID(WRFCOM_EMU_SERVICE_UUID), 60);
  BLEService *pImuService = pServer->createService(BLEUUID(WRFCOM_IMU_SERVICE_UUID), 60);

  // Create BLE Characteristics
  // Core
  pWifi = pCoreService->createCharacteristic(
            WIFI_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ   |
            BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE
          );
  pBle2904_wifi = new BLE2904();
  pBle2904_wifi->setFormat(0x19); // utf8s
  pBle2904_wifi->setUnit(0x2700); // Unitless
  pWifi->addDescriptor(pBle2904_wifi);

  pIpaddr = pCoreService->createCharacteristic(
              IPADDR_CHARACTERISTIC_UUID,
              BLECharacteristic::PROPERTY_READ   |
              BLECharacteristic::PROPERTY_NOTIFY |
              BLECharacteristic::PROPERTY_INDICATE
            );
  pBle2904_ipaddr = new BLE2904();
  pBle2904_ipaddr->setFormat(0x19); // utf8s
  pBle2904_ipaddr->setUnit(0x2700); // Unitless
  pIpaddr->addDescriptor(pBle2904_ipaddr);


  pBattery = pCoreService->createCharacteristic(
               BATTERY_CHARACTERISTIC_UUID,
               BLECharacteristic::PROPERTY_READ   |
               BLECharacteristic::PROPERTY_NOTIFY |
               BLECharacteristic::PROPERTY_INDICATE
             );
  pBle2904_battery = new BLE2904();
  pBle2904_battery->setFormat(0x10); // int32
  pBle2904_battery->setUnit(0x27AD); // Percentage
  pBattery->addDescriptor(pBle2904_battery);

  pArmed = pCoreService->createCharacteristic(
             ARMED_CHARACTERISTIC_UUID,
             BLECharacteristic::PROPERTY_READ   |
             BLECharacteristic::PROPERTY_NOTIFY |
             BLECharacteristic::PROPERTY_INDICATE |
             BLECharacteristic::PROPERTY_WRITE
           );
  pArmed->setCallbacks(new MyArmCallbacks());

  pLogging = pCoreService->createCharacteristic(
               LOGGING_CHARACTERISTIC_UUID,
               BLECharacteristic::PROPERTY_READ   |
               BLECharacteristic::PROPERTY_NOTIFY |
               BLECharacteristic::PROPERTY_INDICATE |
               BLECharacteristic::PROPERTY_WRITE
             );
  pLogging->setCallbacks(new MyLogCallbacks());





  //Capability
  pCTemperature = pCapabilityService->createCharacteristic(
                    CTEMPERATURE_CHARACTERISTIC_UUID,
                    BLECharacteristic::PROPERTY_READ   |
                    BLECharacteristic::PROPERTY_NOTIFY |
                    BLECharacteristic::PROPERTY_INDICATE
                  );
  pBle2904_ctemperature = new BLE2904();
  pBle2904_ctemperature->setFormat(0x01); // Boolean
  pBle2904_ctemperature->setUnit(0x2700); // Unitless
  pCTemperature->addDescriptor(pBle2904_ctemperature);

  pCPressure = pCapabilityService->createCharacteristic(
                 CPRESSURE_CHARACTERISTIC_UUID,
                 BLECharacteristic::PROPERTY_READ   |
                 BLECharacteristic::PROPERTY_NOTIFY |
                 BLECharacteristic::PROPERTY_INDICATE
               );
  pBle2904_cpressure = new BLE2904();
  pBle2904_cpressure->setFormat(0x01); // Boolean
  pBle2904_cpressure->setUnit(0x2700); // Unitless
  pCPressure->addDescriptor(pBle2904_cpressure);

  pCAccelerometer = pCapabilityService->createCharacteristic(
                      CACC_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  pBle2904_caccelerometer = new BLE2904();
  pBle2904_caccelerometer->setFormat(0x01); // Boolean
  pBle2904_caccelerometer->setUnit(0x2700); // Unitless
  pCAccelerometer->addDescriptor(pBle2904_caccelerometer);

  pCGyroscope = pCapabilityService->createCharacteristic(
                  CGYRO_CHARACTERISTIC_UUID,
                  BLECharacteristic::PROPERTY_READ   |
                  BLECharacteristic::PROPERTY_NOTIFY |
                  BLECharacteristic::PROPERTY_INDICATE
                );
  pBle2904_cgyroscope = new BLE2904();
  pBle2904_cgyroscope->setFormat(0x01); // Boolean
  pBle2904_cgyroscope->setUnit(0x2700); // Unitless
  pCGyroscope->addDescriptor(pBle2904_cgyroscope);

  pCMagnetometer = pCapabilityService->createCharacteristic(
                     CMAG_CHARACTERISTIC_UUID,
                     BLECharacteristic::PROPERTY_READ   |
                     BLECharacteristic::PROPERTY_NOTIFY |
                     BLECharacteristic::PROPERTY_INDICATE
                   );
  pBle2904_cmagnetometer = new BLE2904();
  pBle2904_cmagnetometer->setFormat(0x01); // Boolean
  pBle2904_cmagnetometer->setUnit(0x2700); // Unitless
  pCMagnetometer->addDescriptor(pBle2904_cmagnetometer);






  // Flags
  pGpsEnabled = pCapabilityService->createCharacteristic(
                  GPSENABLED_CHARACTERISTIC_UUID,
                  BLECharacteristic::PROPERTY_READ   |
                  BLECharacteristic::PROPERTY_NOTIFY |
                  BLECharacteristic::PROPERTY_INDICATE
                );
  pBle2904_bms_enabled = new BLE2904();
  pBle2904_bms_enabled->setFormat(0x01); // Boolean
  pBle2904_bms_enabled->setUnit(0x2700); // Unitless
  pGpsEnabled->addDescriptor(pBle2904_bms_enabled);

  pGpsLocked = pCapabilityService->createCharacteristic(
                 GPSLOCKED_CHARACTERISTIC_UUID,
                 BLECharacteristic::PROPERTY_READ   |
                 BLECharacteristic::PROPERTY_NOTIFY |
                 BLECharacteristic::PROPERTY_INDICATE
               );
  pBle2904_gps_enabled = new BLE2904();
  pBle2904_gps_enabled->setFormat(0x01); // Boolean
  pBle2904_gps_enabled->setUnit(0x2700); // Unitless
  pGpsLocked->addDescriptor(pBle2904_gps_enabled);

  pBmsEnabled = pCapabilityService->createCharacteristic(
                  BMSENABLED_CHARACTERISTIC_UUID,
                  BLECharacteristic::PROPERTY_READ   |
                  BLECharacteristic::PROPERTY_NOTIFY |
                  BLECharacteristic::PROPERTY_INDICATE
                );
  pBle2904_gps_locked = new BLE2904();
  pBle2904_gps_locked->setFormat(0x01); // Boolean
  pBle2904_gps_locked->setUnit(0x2700); // Unitless
  pBmsEnabled->addDescriptor(pBle2904_gps_locked);

  pEmuEnabled = pCapabilityService->createCharacteristic(
                  EMUENABLED_CHARACTERISTIC_UUID,
                  BLECharacteristic::PROPERTY_READ   |
                  BLECharacteristic::PROPERTY_NOTIFY |
                  BLECharacteristic::PROPERTY_INDICATE
                );
  pBle2904_emu_enabled = new BLE2904();
  pBle2904_emu_enabled->setFormat(0x01); // Boolean
  pBle2904_emu_enabled->setUnit(0x2700); // Unitless
  pEmuEnabled->addDescriptor(pBle2904_emu_enabled);

  pImuEnabled = pCapabilityService->createCharacteristic(
                  IMUENABLED_CHARACTERISTIC_UUID,
                  BLECharacteristic::PROPERTY_READ   |
                  BLECharacteristic::PROPERTY_NOTIFY |
                  BLECharacteristic::PROPERTY_INDICATE
                );
  pBle2904_imu_enabled = new BLE2904();
  pBle2904_imu_enabled->setFormat(0x01); // Boolean
  pBle2904_imu_enabled->setUnit(0x2700); // Unitless
  pImuEnabled->addDescriptor(pBle2904_imu_enabled);

  pSrvEnabled = pCapabilityService->createCharacteristic(
                  SRVENABLED_CHARACTERISTIC_UUID,
                  BLECharacteristic::PROPERTY_READ   |
                  BLECharacteristic::PROPERTY_NOTIFY |
                  BLECharacteristic::PROPERTY_INDICATE
                );
  pBle2904_srv_enabled = new BLE2904();
  pBle2904_srv_enabled->setFormat(0x01); // Boolean
  pBle2904_srv_enabled->setUnit(0x2700); // Unitless
  pSrvEnabled->addDescriptor(pBle2904_srv_enabled);

  pRtcEnabled = pCapabilityService->createCharacteristic(
                  RTCENABLED_CHARACTERISTIC_UUID,
                  BLECharacteristic::PROPERTY_READ   |
                  BLECharacteristic::PROPERTY_NOTIFY |
                  BLECharacteristic::PROPERTY_INDICATE
                );
  pBle2904_rtc_enabled = new BLE2904();
  pBle2904_rtc_enabled->setFormat(0x01); // Boolean
  pBle2904_rtc_enabled->setUnit(0x2700); // Unitless
  pRtcEnabled->addDescriptor(pBle2904_rtc_enabled);







  // GPS
  pSiv = pGpsService->createCharacteristic(
           SIV_CHARACTERISTIC_UUID,
           BLECharacteristic::PROPERTY_READ   |
           BLECharacteristic::PROPERTY_NOTIFY |
           BLECharacteristic::PROPERTY_INDICATE
         );
  pBle2904_siv = new BLE2904();
  pBle2904_siv->setFormat(0x04); // unsigned int8
  pBle2904_siv->setUnit(0x2700); // Unitless
  pSiv->addDescriptor(pBle2904_siv);

  pLatitude = pGpsService->createCharacteristic(
                LATITUDE_CHARACTERISTIC_UUID,
                BLECharacteristic::PROPERTY_READ   |
                BLECharacteristic::PROPERTY_NOTIFY |
                BLECharacteristic::PROPERTY_INDICATE
              );
  pBle2904_latitude = new BLE2904();
  pBle2904_latitude->setFormat(0x10); // int32
  pBle2904_latitude->setUnit(0x2763); // Plane angle (degree)
  pBle2904_latitude->setExponent(-7);
  pLatitude->addDescriptor(pBle2904_latitude);

  pLongitude = pGpsService->createCharacteristic(
                 LONGITUDE_CHARACTERISTIC_UUID,
                 BLECharacteristic::PROPERTY_READ   |
                 BLECharacteristic::PROPERTY_NOTIFY |
                 BLECharacteristic::PROPERTY_INDICATE
               );
  pBle2904_longitude = new BLE2904();
  pBle2904_longitude->setFormat(0x10); // int32
  pBle2904_longitude->setUnit(0x2763); // Plane angle (degree)
  pBle2904_longitude->setExponent(-7);
  pLongitude->addDescriptor(pBle2904_longitude);

  pAltitude = pGpsService->createCharacteristic(
                ALTITUDE_CHARACTERISTIC_UUID,
                BLECharacteristic::PROPERTY_READ   |
                BLECharacteristic::PROPERTY_NOTIFY |
                BLECharacteristic::PROPERTY_INDICATE
              );
  pBle2904_altitude = new BLE2904();
  pBle2904_altitude->setFormat(0x10);
  pBle2904_altitude->setUnit(0x2701); // length (meter)
  pBle2904_altitude->setExponent(-3);
  pAltitude->addDescriptor(pBle2904_altitude);






  // EMU
  pPressure = pEmuService->createCharacteristic(
                PRESSURE_CHARACTERISTIC_UUID,
                BLECharacteristic::PROPERTY_READ   |
                BLECharacteristic::PROPERTY_NOTIFY |
                BLECharacteristic::PROPERTY_INDICATE
              );
  pBle2904_pressure = new BLE2904();
  pBle2904_pressure->setFormat(0x10); // int32
  pBle2904_pressure->setUnit(0x2724); // Pascal
  pBle2904_pressure->setExponent(-1); // Tenths of a pascal
  pPressure->addDescriptor(pBle2904_pressure);

  pTemperature = pEmuService->createCharacteristic(
                   TEMPERATURE_CHARACTERISTIC_UUID,
                   BLECharacteristic::PROPERTY_READ   |
                   BLECharacteristic::PROPERTY_NOTIFY |
                   BLECharacteristic::PROPERTY_INDICATE
                 );
  pBle2904_temperature = new BLE2904();
  pBle2904_temperature->setFormat(0x10); // int32
  pBle2904_temperature->setUnit(0x272F); // Celcius
  pBle2904_temperature->setExponent(-2); // Hundreths of a degree
  pTemperature->addDescriptor(pBle2904_temperature);






  // IMU Acc
  pAccX = pImuService->createCharacteristic(
            ACCX_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ   |
            BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE
          );
  pBle2904_acc_x = new BLE2904();
  pBle2904_acc_x->setFormat(0x10); // int32
  pBle2904_acc_x->setUnit(0x2713); // acceleration (metres per second squared)
  pBle2904_acc_x->setExponent(-3); // mm/s2
  pAccX->addDescriptor(pBle2904_acc_x);

  pAccY = pImuService->createCharacteristic(
            ACCY_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ   |
            BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE
          );
  pBle2904_acc_y = new BLE2904();
  pBle2904_acc_y->setFormat(0x10); // int32
  pBle2904_acc_y->setUnit(0x2713); // acceleration (metres per second squared)
  pBle2904_acc_y->setExponent(-3); // mm/s2
  pAccY->addDescriptor(pBle2904_acc_y);

  pAccZ = pImuService->createCharacteristic(
            ACCZ_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ   |
            BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE
          );
  pBle2904_acc_z = new BLE2904();
  pBle2904_acc_z->setFormat(0x10); // int32
  pBle2904_acc_z->setUnit(0x2713); // acceleration (metres per second squared)
  pBle2904_acc_z->setExponent(-3); // mm/s2
  pAccZ->addDescriptor(pBle2904_acc_z);






  // IMU Gyro
  pGyroX = pImuService->createCharacteristic(
             GYROX_CHARACTERISTIC_UUID,
             BLECharacteristic::PROPERTY_READ   |
             BLECharacteristic::PROPERTY_NOTIFY |
             BLECharacteristic::PROPERTY_INDICATE
           );
  pBle2904_gyro_x = new BLE2904();
  pBle2904_gyro_x->setFormat(0x10); // int32
  pBle2904_gyro_x->setUnit(0x2743); // angular velocity (radians per second)
  pBle2904_gyro_x->setExponent(-4); // 10 thousandths of a R/s
  pGyroX->addDescriptor(pBle2904_gyro_x);

  pGyroY = pImuService->createCharacteristic(
             GYROY_CHARACTERISTIC_UUID,
             BLECharacteristic::PROPERTY_READ   |
             BLECharacteristic::PROPERTY_NOTIFY |
             BLECharacteristic::PROPERTY_INDICATE
           );
  pBle2904_gyro_y = new BLE2904();
  pBle2904_gyro_y->setFormat(0x10); // int32
  pBle2904_gyro_y->setUnit(0x2743); // angular velocity (radians per second)
  pBle2904_gyro_y->setExponent(-4); // 10 thousandths of a R/s
  pGyroY->addDescriptor(pBle2904_gyro_y);

  pGyroZ = pImuService->createCharacteristic(
             GYROZ_CHARACTERISTIC_UUID,
             BLECharacteristic::PROPERTY_READ   |
             BLECharacteristic::PROPERTY_NOTIFY |
             BLECharacteristic::PROPERTY_INDICATE
           );
  pBle2904_gyro_z = new BLE2904();
  pBle2904_gyro_z->setFormat(0x10); // int32
  pBle2904_gyro_z->setUnit(0x2743); // angular velocity (radians per second)
  pBle2904_gyro_z->setExponent(-4); // 10 thousandths of a R/s
  pGyroZ->addDescriptor(pBle2904_gyro_z);






  // IMU Mag
  pMagX = pImuService->createCharacteristic(
            MAGX_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ   |
            BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE
          );
  pBle2904_mag_x = new BLE2904();
  pBle2904_mag_x->setFormat(0x10); // int32
  pBle2904_mag_x->setUnit(0x272D); // magnetic flux density (tesla)
  pBle2904_mag_x->setExponent(-8); // hundredths of a uTesla
  pMagX->addDescriptor(pBle2904_mag_x);

  pMagY = pImuService->createCharacteristic(
            MAGX_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ   |
            BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE
          );
  pBle2904_mag_y = new BLE2904();
  pBle2904_mag_y->setFormat(0x10); // int32
  pBle2904_mag_y->setUnit(0x272D); // magnetic flux density (tesla)
  pBle2904_mag_y->setExponent(-8); // hundredths of a uTesla
  pMagY->addDescriptor(pBle2904_mag_y);

  pMagZ = pImuService->createCharacteristic(
            MAGX_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ   |
            BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE
          );
  pBle2904_mag_z = new BLE2904();
  pBle2904_mag_z->setFormat(0x10); // int32
  pBle2904_mag_z->setUnit(0x272D); // magnetic flux density (tesla)
  pBle2904_mag_z->setExponent(-8); // hundredths of a uTesla
  pMagZ->addDescriptor(pBle2904_mag_z);





  // Configure advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(WRFCOM_CORE_SERVICE_UUID);
  pAdvertising->addServiceUUID(WRFCOM_CAPABILITY_SERVICE_UUID);
  pAdvertising->addServiceUUID(WRFCOM_GPS_SERVICE_UUID);
  pAdvertising->addServiceUUID(WRFCOM_EMU_SERVICE_UUID);
  pAdvertising->addServiceUUID(WRFCOM_IMU_SERVICE_UUID);
  //pAdvertising->setScanResponse(false);
  //pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter

  // Start the services
  pCoreService->start();
  pCapabilityService->start();
  pGpsService->start();
  pEmuService->start();
  pImuService->start();

  // Startup the device
  BLEDevice::startAdvertising();

#if _DEBUG_
  Serial.print("BLE initialised: ");
  Serial.print(DEVICE_NAME);
  Serial.println();
#endif

  _enabled = true;
}

void TrBLE::loop() {
  unsigned long now = millis();

  // disconnecting time to heal
  if (_disconnecting && (now - _last_disconnect) < 500) {
    return;
  }

  if (_disconnecting) {
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising for a new punter");
    oldDeviceConnected = deviceConnected;
    _disconnecting = false;
  } else if (!deviceConnected && oldDeviceConnected) {
    Serial.println("Handling a disconnect event");
    _last_disconnect = now;
    _disconnecting = true;
  }

  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    Serial.println("stop advertising - we have an active listener");
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }

  // notify changed value
  if (deviceConnected) {

    if ((now - _refresh_last) < _refresh_millis) {
      // too soon
      return;
    }
    _refresh_last = now;






    // Core
    if (pWifi) {
      if (NET.isApMode()) {
        wifi_value = "AP";
      } else if (NET.isEnabled()) {
        wifi_value = "STA";
      } else {
        wifi_value = "NA";
      }
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: WIFI: '");
      Serial.print(wifi_value);
      Serial.print("'");
      Serial.println();
#endif
      pWifi->setValue(wifi_value.c_str());
      pWifi->notify();
    }

    if (pIpaddr) {
      if (NET.isEnabled()) {
        ipaddr_value = NET.getIpAddress();
      } else {
        ipaddr_value = "";
      }
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: IPAD: '");
      Serial.print(ipaddr_value);
      Serial.print("'");
      Serial.println();
#endif
      pIpaddr->setValue(ipaddr_value.c_str());
      pIpaddr->notify();
    }

    if (pBattery) {
      battery_value = round(BMS.getCapacityPercent());
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: BAT: ");
      Serial.print(battery_value);
      Serial.println();
#endif
      pBattery->setValue(&battery_value, 1);
      pBattery->notify();
    }

    if (pArmed) {
      armed_value = SRV.isArmed() ? 1 : 0;
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: ARM: ");
      Serial.print(armed_value ? "TRUE" : "FALSE");
      Serial.println();
#endif
      pArmed->setValue(&armed_value, 1);
      pArmed->notify();
    }

    if (pLogging) {
      logging_value = LOG.isCapturing() ? 1 : 0;
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: LOG: ");
      Serial.print(logging_value ? "TRUE" : "FALSE");
      Serial.println();
#endif
      pLogging->setValue(&logging_value, 1);
      pLogging->notify();
    }





    // Capability
    if (pCPressure) {
      cpressure_value = EMU.hasPressure() ? 1 : 0;
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: CPRESSURE: ");
      Serial.print(cpressure_value ? "TRUE" : "FALSE");
      Serial.println();
#endif
      pCPressure->setValue(&cpressure_value, 1);
      pCPressure->notify();
    }

    if (pCTemperature) {
      ctemperature_value = EMU.hasTemperature() ? 1 : 0;
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: CTEMP: ");
      Serial.print(ctemperature_value ? "TRUE" : "FALSE");
      Serial.println();
#endif
      pCTemperature->setValue(&ctemperature_value, 1);
      pCTemperature->notify();
    }

    if (pCAccelerometer) {
      caccelerometer_value = IMU.hasAcc() ? 1 : 0;
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: CACC: ");
      Serial.print(caccelerometer_value ? "TRUE" : "FALSE");
      Serial.println();
#endif
      pCAccelerometer->setValue(&caccelerometer_value, 1);
      pCAccelerometer->notify();
    }

    if (pCGyroscope) {
      cgyroscope_value = IMU.hasAcc() ? 1 : 0;
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: CGYRO: ");
      Serial.print(cgyroscope_value ? "TRUE" : "FALSE");
      Serial.println();
#endif
      pCGyroscope->setValue(&cgyroscope_value, 1);
      pCGyroscope->notify();
    }

    if (pCMagnetometer) {
      cmagnetometer_value = IMU.hasMag() ? 1 : 0;
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: CMAG: ");
      Serial.print(cmagnetometer_value ? "TRUE" : "FALSE");
      Serial.println();
#endif
      pCMagnetometer->setValue(&cmagnetometer_value, 1);
      pCMagnetometer->notify();
    }

    if (pBmsEnabled) {
      bms_enabled_value = BMS.isEnabled() ? 1 : 0;
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: BMSE: ");
      Serial.print(bms_enabled_value ? "TRUE" : "FALSE");
      Serial.println();
#endif
      pBmsEnabled->setValue(&bms_enabled_value, 1);
      pBmsEnabled->notify();
    }

    if (pGpsEnabled) {
      gps_enabled_value = GPS.isEnabled() ? 1 : 0;
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: GPSE: ");
      Serial.print(gps_enabled_value ? "TRUE" : "FALSE");
      Serial.println();
#endif
      pGpsEnabled->setValue(&gps_enabled_value, 1);
      pGpsEnabled->notify();
    }

    if (pGpsLocked) {
      gps_locked_value = GPS.isConnected() ? 1 : 0;
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: GPSL: ");
      Serial.print(gps_locked_value ? "TRUE" : "FALSE");
      Serial.println();
#endif
      pGpsLocked->setValue(&gps_locked_value, 1);
      pGpsLocked->notify();
    }

    if (pImuEnabled) {
      imu_enabled_value = IMU.isEnabled() ? 1 : 0;
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: IMU: ");
      Serial.print(imu_enabled_value ? "TRUE" : "FALSE");
      Serial.println();
#endif
      pImuEnabled->setValue(&imu_enabled_value, 1);
      pImuEnabled->notify();
    }

    if (pEmuEnabled) {
      emu_enabled_value = EMU.isEnabled() ? 1 : 0;
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: EMU: ");
      Serial.print(emu_enabled_value ? "TRUE" : "FALSE");
      Serial.println();
#endif
      pEmuEnabled->setValue(&emu_enabled_value, 1);
      pEmuEnabled->notify();
    }

    if (pSrvEnabled) {
      srv_enabled_value = SRV.isEnabled() ? 1 : 0;
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: SRV: ");
      Serial.print(srv_enabled_value ? "TRUE" : "FALSE");
      Serial.println();
#endif
      pSrvEnabled->setValue(&srv_enabled_value, 1);
      pSrvEnabled->notify();
    }

    if (pRtcEnabled) {
      rtc_enabled_value = RTC.isEnabled() ? 1 : 0;
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: RTC: ");
      Serial.print(rtc_enabled_value ? "TRUE" : "FALSE");
      Serial.println();
#endif
      pRtcEnabled->setValue(&rtc_enabled_value, 1);
      pRtcEnabled->notify();
    }






    // GPS
    if (pSiv) {
      siv_value = GPS.getSatsInView();
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: SIV: ");
      Serial.print(siv_value);
      Serial.println();
#endif
      pSiv->setValue(&siv_value, 1);
      pSiv->notify();
    }

    if (pLatitude) {
      latitude_value = int(GPS.getLatitude() * 10000000.0);
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: LAT: ");
      Serial.print(latitude_value);
      Serial.print(" deg");
      Serial.println();
#endif
      pLatitude->setValue(latitude_value);
      pLatitude->notify();
    }

    if (pLongitude) {
      longitude_value = int(GPS.getLongitude() * 10000000.0);
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: LNG: ");
      Serial.print(longitude_value);
      Serial.print(" deg");
      Serial.println();
#endif
      pLongitude->setValue(longitude_value);
      pLongitude->notify();
    }

    if (pAltitude) {
      altitude_value = int(GPS.getAltitude() * 1000.0);
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: ALT: ");
      Serial.print(altitude_value);
      Serial.print(" m");
      Serial.println();
#endif
      pAltitude->setValue(altitude_value);
      pAltitude->notify();
    }






    // EMU
    if (pPressure) {
      pressure_value = int(EMU.getPressure() * 1000.0);
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: HPA: ");
      Serial.print(pressure_value);
      Serial.print(" Pa");
      Serial.println();
#endif
      pPressure->setValue(pressure_value);
      pPressure->notify();
    }

    if (pTemperature) {
      temperature_value = int(EMU.getTemperature() * 100.0);
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: TMP: ");
      Serial.print(temperature_value);
      Serial.print(" C");
      Serial.println();
#endif
      pTemperature->setValue(temperature_value);
      pTemperature->notify();
    }





    // IMU-Acc
    if (pAccX) {
      accx_value = int(IMU.getAccX() * 1000.0);
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: IMUAX: ");
      Serial.print(accx_value);
      Serial.print("*10^-3 m/s2");
      Serial.println();
#endif
      pAccX->setValue(accx_value);
      pAccX->notify();
    }

    if (pAccY) {
      accy_value = int(IMU.getAccY() * 1000.0);
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: IMUAY: ");
      Serial.print(accy_value);
      Serial.print("*10^-3 m/s2");
      Serial.println();
#endif
      pAccY->setValue(accy_value);
      pAccY->notify();
    }

    if (pAccZ) {
      accz_value = int(IMU.getAccZ() * 1000.0);
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: IMUAZ: ");
      Serial.print(accz_value);
      Serial.print("*10^-3 m/s2");
      Serial.println();
#endif
      pAccZ->setValue(accz_value);
      pAccZ->notify();
    }





    // IMU-Gyro
    if (pGyroX) {
      gyrox_value = int(IMU.getGyroX() * (PI / 180.) * 10000.0);
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: IMUGX: ");
      Serial.print(IMU.getGyroX());
      Serial.print(" d/s, ");
      Serial.print(gyrox_value);
      Serial.print("*10^-4 R/s");
      Serial.println();
#endif
      pGyroX->setValue(gyrox_value);
      pGyroX->notify();
    }

    if (pGyroY) {
      gyroy_value = int(IMU.getGyroY() * (PI / 180.) * 10000.0);
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: IMUGY: ");
      Serial.print(IMU.getGyroY());
      Serial.print(" d/s, ");
      Serial.print(gyroy_value);
      Serial.print("*10^-4 R/s");
      Serial.println();
#endif
      pGyroY->setValue(gyroy_value);
      pGyroY->notify();
    }

    if (pGyroX) {
      gyroz_value = int(IMU.getGyroZ() * (PI / 180.) * 10000.0);
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: IMUGY: ");
      Serial.print(IMU.getGyroZ());
      Serial.print(" d/s, ");
      Serial.print(gyroz_value);
      Serial.print("*10^-4 R/s");
      Serial.println();
#endif
      pGyroZ->setValue(gyroz_value);
      pGyroZ->notify();
    }





    // IMU-Mag
    if (pMagX) {
      magx_value = int(IMU.getMagX() * 100.0);
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: IMUMX: ");
      Serial.print(magx_value);
      Serial.print("*10^-2 uTesla");
      Serial.println();
#endif
      pMagX->setValue(magx_value);
      pMagX->notify();
    }

    // IMU-Mag
    if (pMagY) {
      magy_value = int(IMU.getMagX() * 100.0);
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: IMUMY: ");
      Serial.print(magy_value);
      Serial.print("*10^-2 uTesla");
      Serial.println();
#endif
      pMagY->setValue(magy_value);
      pMagY->notify();
    }

    // IMU-Mag
    if (pMagZ) {
      magz_value = int(IMU.getMagZ() * 100.0);
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: IMUMX: ");
      Serial.print(magz_value);
      Serial.print("*10^-2 uTesla");
      Serial.println();
#endif
      pMagZ->setValue(magz_value);
      pMagZ->notify();
    }
  }
}

#endif // _USE_BLE_
