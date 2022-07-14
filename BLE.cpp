#include "BLE.h"
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

bool deviceConnected = false;
bool oldDeviceConnected = false;

BLEServer* pServer = NULL;

// Core
BLECharacteristic* pWifi = NULL;
BLECharacteristic* pIpaddr = NULL;
BLECharacteristic* pBattery = NULL;
BLECharacteristic* pArmed = NULL;
BLECharacteristic* pLogging = NULL;

//// Capability
//BLECharacteristic* pCPressure = NULL;
//BLECharacteristic* pCTemperature = NULL;
//BLECharacteristic* pCAccelerometer = NULL;
//BLECharacteristic* pCGyroscope = NULL;
//BLECharacteristic* pCMagnetometer = NULL;

// Flags
BLECharacteristic* pBmsEnabled = NULL;
BLECharacteristic* pGpsEnabled = NULL;
BLECharacteristic* pGpsLocked = NULL;
BLECharacteristic* pBmpEnabled = NULL;
BLECharacteristic* pImuEnabled = NULL;

// GPS
BLECharacteristic* pSiv = NULL;
BLECharacteristic* pLatitude = NULL;
BLECharacteristic* pLongitude = NULL;
BLECharacteristic* pAltitude = NULL;

// BMP
BLECharacteristic* pPressure = NULL;
BLECharacteristic* pTemperature = NULL;

// IMU
BLECharacteristic* pAccX = NULL;
BLECharacteristic* pAccY = NULL;
BLECharacteristic* pAccZ = NULL;
BLECharacteristic* pGyroX = NULL;
BLECharacteristic* pGyroY = NULL;
BLECharacteristic* pGyroZ = NULL;
BLECharacteristic* pMagX = NULL;
BLECharacteristic* pMagY = NULL;
BLECharacteristic* pMagZ = NULL;

// Core
BLE2904* pBle2904_wifi = NULL;
BLE2904* pBle2904_ipaddr = NULL;
BLE2904* pBle2904_battery = NULL;

//// Flags
//BLE2904* pBle2904_bms_enabled = NULL;
//BLE2904* pBle2904_gps_enabled = NULL;
//BLE2904* pBle2904_gps_locked = NULL;
//BLE2904* pBle2904_bmp_enabled = NULL;
//BLE2904* pBle2904_imu_enabled = NULL;

// GPS
BLE2904* pBle2904_siv = NULL;
BLE2904* pBle2904_latitude = NULL;
BLE2904* pBle2904_longitude = NULL;
BLE2904* pBle2904_altitude = NULL;

// BMP
BLE2904* pBle2904_pressure = NULL;
BLE2904* pBle2904_temperature = NULL;

// IMU
BLE2904* pBle2904_acc_x = NULL;
BLE2904* pBle2904_acc_y = NULL;
BLE2904* pBle2904_acc_z = NULL;
BLE2904* pBle2904_gyro_x = NULL;
BLE2904* pBle2904_gyro_y = NULL;
BLE2904* pBle2904_gyro_z = NULL;
//BLE2904* pBle2904_mag_x = NULL;
//BLE2904* pBle2904_mag_y = NULL;
//BLE2904* pBle2904_mag_z = NULL;

//// Capability
//BLE2904* pBle2904_ctemperature = NULL;
//BLE2904* pBle2904_cpressure = NULL;
//BLE2904* pBle2904_caccelerometer = NULL;
//BLE2904* pBle2904_cgyroscope = NULL;
//BLE2904* pBle2904_cmagnetometer = NULL;

// Core
String wifi_value = "STA";
String ipaddr_value = "192.168.999.999";
uint8_t battery_value = int(12.34);
uint8_t armed_value = true ? 1 : 0;
uint8_t logging_value = true ? 1 : 0;

// Flags
uint8_t bms_enabled_value = true ? 1 : 0;
uint8_t gps_enabled_value = true ? 1 : 0;
uint8_t gps_locked_value = false ? 1 : 0;
uint8_t bmp_enabled_value = true ? 1 : 0;
uint8_t imu_enabled_value = true ? 1 : 0;

// GPS
uint8_t siv_value = 0;
int latitude_value = int(51.012323 * 10000000.0);
int longitude_value = (0.4612323 * 10000000.0);
int altitude_value = int(1.234 * 1000.0); //int(23.441*1000);

// BMP
int pressure_value = int(1013.25 * 1000.0);
int temperature_value = int(18.12 * 100.0);

// IMU
int accx_value = int(0.1 * 10000.);
int accy_value = int(-0.1 * 10000.);
int accz_value = int(9.79 * 10000.);
int gyrox_value = int(0.1 * 10000.);
int gyroy_value = int(-0.1 * 10000.);
int gyroz_value = int(0.0 * 10000.);
//int magx_value = int(123.45 * 100.);
//int magy_value = int(-0.2 * 100.);
//int magz_value = int(0.3 * 100.);

//// Capability
//uint8_t cpressure_value = true ? 1 : 0;
//uint8_t ctemperature_value = true ? 1 : 0;
//uint8_t caccelerometer_value = true ? 1 : 0;
//uint8_t cgyroscope_value = true ? 1 : 0;
//uint8_t cmagnetometer_value = false ? 1 : 0;

// Full GATT charactersitics:
// https://gist.github.com/sam016/4abe921b5a9ee27f67b3686910293026
//
// Format types:
// https://btprodspecificationrefs.blob.core.windows.net/assigned-numbers/Assigned%20Number%20Types/Format%20Types.pdf
//
// Characteristic and unit definitions:
// https://btprodspecificationrefs.blob.core.windows.net/assigned-values/16-bit%20UUID%20Numbers%20Document.pdf

#define WRFCOM_SERVICE_NAME              "WRFCOM"
#define WRFCOM_CORE_SERVICE_UUID         "9ddf3d45-ea85-467a-9b23-34a9e4900000"
#define WRFCOM_FLAGS_SERVICE_UUID        "9ddf3d45-ea85-467a-9b23-34a9e4900100"
#define WRFCOM_GPS_SERVICE_UUID          "9ddf3d45-ea85-467a-9b23-34a9e4900200"
#define WRFCOM_BMP_SERVICE_UUID          "9ddf3d45-ea85-467a-9b23-34a9e4900300"
//#define WRFCOM_IMU_SERVICE_UUID          "9ddf3d45-ea85-467a-9b23-34a9e4900400"
//#define WRFCOM_CAPABILITY_SERVICE_UUID   "9ddf3d45-ea85-467a-9b23-34a9e4900500"
#define WRFCOM_IMUACC_SERVICE_UUID       "9ddf3d45-ea85-467a-9b23-34a9e4900600"
#define WRFCOM_IMUGYRO_SERVICE_UUID      "9ddf3d45-ea85-467a-9b23-34a9e4900700"
//#define WRFCOM_IMUMAG_SERVICE_UUID       "9ddf3d45-ea85-467a-9b23-34a9e4900800"

// Core
//#define NAME_CHARACTERISTIC_UUID         "9ddf3d45-ea85-467a-9b23-34a9e4900001"
#define WIFI_CHARACTERISTIC_UUID         "9ddf3d45-ea85-467a-9b23-34a9e4900002"
#define IPADDR_CHARACTERISTIC_UUID       "9ddf3d45-ea85-467a-9b23-34a9e4900003"
#define BATTERY_CHARACTERISTIC_UUID      BLEUUID((uint16_t)0x2A19)
#define ARMED_CHARACTERISTIC_UUID        "9ddf3d45-ea85-467a-9b23-34a9e4900004"
#define LOGGING_CHARACTERISTIC_UUID      "9ddf3d45-ea85-467a-9b23-34a9e4900005"

//// Capability
//#define CTEMPERATURE_CHARACTERISTIC_UUID "9ddf3d45-ea85-467a-9b23-34a9e4900501"
//#define CPRESSURE_CHARACTERISTIC_UUID    "9ddf3d45-ea85-467a-9b23-34a9e4900502"
//#define CACC_CHARACTERISTIC_UUID         "9ddf3d45-ea85-467a-9b23-34a9e4900503"
//#define CGYRO_CHARACTERISTIC_UUID        "9ddf3d45-ea85-467a-9b23-34a9e4900504"
//#define CMAG_CHARACTERISTIC_UUID         "9ddf3d45-ea85-467a-9b23-34a9e4900505"

// Flags
#define BMSENABLED_CHARACTERISTIC_UUID   "9ddf3d45-ea85-467a-9b23-34a9e4900101"
#define GPSENABLED_CHARACTERISTIC_UUID   "9ddf3d45-ea85-467a-9b23-34a9e4900102"
#define GPSLOCKED_CHARACTERISTIC_UUID    "9ddf3d45-ea85-467a-9b23-34a9e4900103"
#define BMPENABLED_CHARACTERISTIC_UUID   "9ddf3d45-ea85-467a-9b23-34a9e4900104"
#define IMUENABLED_CHARACTERISTIC_UUID   "9ddf3d45-ea85-467a-9b23-34a9e4900105"

// GPS
#define SIV_CHARACTERISTIC_UUID          "9ddf3d45-ea85-467a-9b23-34a9e4900201"
#define LATITUDE_CHARACTERISTIC_UUID     BLEUUID((uint16_t)0x2AAE)
#define LONGITUDE_CHARACTERISTIC_UUID    BLEUUID((uint16_t)0x2AAF)
#define ALTITUDE_CHARACTERISTIC_UUID     BLEUUID((uint16_t)0x2AB3)

// BMP
#define PRESSURE_CHARACTERISTIC_UUID     BLEUUID((uint16_t)0x2A6D)
#define TEMPERATURE_CHARACTERISTIC_UUID  BLEUUID((uint16_t)0x2A6E)

// IMU Acc
#define ACCX_CHARACTERISTIC_UUID         "9ddf3d45-ea85-467a-9b23-34a9e4900601"
#define ACCY_CHARACTERISTIC_UUID         "9ddf3d45-ea85-467a-9b23-34a9e4900602"
#define ACCZ_CHARACTERISTIC_UUID         "9ddf3d45-ea85-467a-9b23-34a9e4900603"

// IMU Gyro
#define GYROX_CHARACTERISTIC_UUID        "9ddf3d45-ea85-467a-9b23-34a9e4900701"
#define GYROY_CHARACTERISTIC_UUID        "9ddf3d45-ea85-467a-9b23-34a9e4900702"
#define GYROZ_CHARACTERISTIC_UUID        "9ddf3d45-ea85-467a-9b23-34a9e4900703"

// IMU Mag
//#define MAGX_CHARACTERISTIC_UUID         "9ddf3d45-ea85-467a-9b23-34a9e4900801"
//#define MAGY_CHARACTERISTIC_UUID         "9ddf3d45-ea85-467a-9b23-34a9e4900802"
//#define MAGZ_CHARACTERISTIC_UUID         "9ddf3d45-ea85-467a-9b23-34a9e4900803"

class MyArmCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("*********\nMyArmCallbacks()");
        Serial.print("New value: ");
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i]?"TRUE":"FALSE");

        Serial.println();
        Serial.println("*********");
      }
    }
};

class MyLogCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("*********\nMyLogCallbacks()");
        Serial.print("New value: ");
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i]?"TRUE":"FALSE");

        Serial.println();
        Serial.println("*********");
      }
    }
};

// Can't get rid of this - it handles connection requests
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

  BLEDevice::init(WRFCOM_SERVICE_NAME);
  if (false) {
    Serial.println("BLE disconnected");
    return;
  }

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pCoreService = pServer->createService(WRFCOM_CORE_SERVICE_UUID);
  //  BLEService *pCapabilityService = pServer->createService(WRFCOM_CAPABILITY_SERVICE_UUID);
  BLEService *pFlagsService = pServer->createService(WRFCOM_FLAGS_SERVICE_UUID);
  BLEService *pGpsService = pServer->createService(WRFCOM_GPS_SERVICE_UUID);
  BLEService *pBmpService = pServer->createService(WRFCOM_BMP_SERVICE_UUID);
  ////BLEService *pImuService = pServer->createService(WRFCOM_IMU_SERVICE_UUID);
  BLEService *pImuAccService = pServer->createService(WRFCOM_IMUACC_SERVICE_UUID);
  BLEService *pImuGyroService = pServer->createService(WRFCOM_IMUGYRO_SERVICE_UUID);
  //BLEService *pImuMagService = pServer->createService(WRFCOM_IMUMAG_SERVICE_UUID);

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





  // Flags
  pGpsEnabled = pFlagsService->createCharacteristic(
                  GPSENABLED_CHARACTERISTIC_UUID,
                  BLECharacteristic::PROPERTY_READ   |
                  BLECharacteristic::PROPERTY_NOTIFY |
                  BLECharacteristic::PROPERTY_INDICATE
                );
  pGpsLocked = pFlagsService->createCharacteristic(
                 GPSLOCKED_CHARACTERISTIC_UUID,
                 BLECharacteristic::PROPERTY_READ   |
                 BLECharacteristic::PROPERTY_NOTIFY |
                 BLECharacteristic::PROPERTY_INDICATE
               );
  pBmsEnabled = pFlagsService->createCharacteristic(
                  BMSENABLED_CHARACTERISTIC_UUID,
                  BLECharacteristic::PROPERTY_READ   |
                  BLECharacteristic::PROPERTY_NOTIFY |
                  BLECharacteristic::PROPERTY_INDICATE
                );
  pBmpEnabled = pFlagsService->createCharacteristic(
                  BMPENABLED_CHARACTERISTIC_UUID,
                  BLECharacteristic::PROPERTY_READ   |
                  BLECharacteristic::PROPERTY_NOTIFY |
                  BLECharacteristic::PROPERTY_INDICATE
                );
  pImuEnabled = pFlagsService->createCharacteristic(
                  IMUENABLED_CHARACTERISTIC_UUID,
                  BLECharacteristic::PROPERTY_READ   |
                  BLECharacteristic::PROPERTY_NOTIFY |
                  BLECharacteristic::PROPERTY_INDICATE
                );






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






  // BMP
  pPressure = pBmpService->createCharacteristic(
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

  pTemperature = pBmpService->createCharacteristic(
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
  pAccX = pImuAccService->createCharacteristic(
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

  pAccY = pImuAccService->createCharacteristic(
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

  pAccZ = pImuAccService->createCharacteristic(
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
  pGyroX = pImuGyroService->createCharacteristic(
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

  pGyroY = pImuGyroService->createCharacteristic(
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

  pGyroZ = pImuGyroService->createCharacteristic(
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

  // IMU Gyro
  //  pMagX = pImuMagService->createCharacteristic(
  //            MAGX_CHARACTERISTIC_UUID,
  //            BLECharacteristic::PROPERTY_READ   |
  //            BLECharacteristic::PROPERTY_NOTIFY |
  //            BLECharacteristic::PROPERTY_INDICATE
  //          );
  //  pBle2904_mag_x = new BLE2904();
  //  pBle2904_mag_x->setFormat(0x10); // int32
  //  pBle2904_mag_x->setUnit(0x272D); // magnetic flux density (tesla)
  //  pBle2904_gyro_x->setExponent(-8); // hundredths of a uTesla
  //  pGyroX->addDescriptor(pBle2904_gyro_x);


  // Capability
  //  pCTemperature = pCapabilityService->createCharacteristic(
  //                    CTEMPERATURE_CHARACTERISTIC_UUID,
  //                    BLECharacteristic::PROPERTY_READ   |
  //                    BLECharacteristic::PROPERTY_NOTIFY |
  //                    BLECharacteristic::PROPERTY_INDICATE
  //                  );
  //  pCPressure = pCapabilityService->createCharacteristic(
  //                    CPRESSURE_CHARACTERISTIC_UUID,
  //                    BLECharacteristic::PROPERTY_READ   |
  //                    BLECharacteristic::PROPERTY_NOTIFY |
  //                    BLECharacteristic::PROPERTY_INDICATE
  //                  );
  //  pCAccelerometer = pCapabilityService->createCharacteristic(
  //                    CACC_CHARACTERISTIC_UUID,
  //                    BLECharacteristic::PROPERTY_READ   |
  //                    BLECharacteristic::PROPERTY_NOTIFY |
  //                    BLECharacteristic::PROPERTY_INDICATE
  //                  );
  //  pCGyroscope = pCapabilityService->createCharacteristic(
  //                    CGYRO_CHARACTERISTIC_UUID,
  //                    BLECharacteristic::PROPERTY_READ   |
  //                    BLECharacteristic::PROPERTY_NOTIFY |
  //                    BLECharacteristic::PROPERTY_INDICATE
  //                  );
  //  pCMagnetometer = pCapabilityService->createCharacteristic(
  //                    CMAG_CHARACTERISTIC_UUID,
  //                    BLECharacteristic::PROPERTY_READ   |
  //                    BLECharacteristic::PROPERTY_NOTIFY |
  //                    BLECharacteristic::PROPERTY_INDICATE
  //                  );




  // Configure advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(WRFCOM_CORE_SERVICE_UUID);
  //  pAdvertising->addServiceUUID(WRFCOM_CAPABILITY_SERVICE_UUID);
  pAdvertising->addServiceUUID(WRFCOM_FLAGS_SERVICE_UUID);
  pAdvertising->addServiceUUID(WRFCOM_GPS_SERVICE_UUID);
  pAdvertising->addServiceUUID(WRFCOM_BMP_SERVICE_UUID);
  ////  pAdvertising->addServiceUUID(WRFCOM_IMU_SERVICE_UUID);
  pAdvertising->addServiceUUID(WRFCOM_IMUACC_SERVICE_UUID);
  pAdvertising->addServiceUUID(WRFCOM_IMUGYRO_SERVICE_UUID);
  //  pAdvertising->addServiceUUID(WRFCOM_IMUMAG_SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter

  // Start the service
  pCoreService->start();
  //pCapabilityService->start();
  pFlagsService->start();
  pGpsService->start();
  pBmpService->start();
  ////pImuService->start();
  pImuAccService->start();
  pImuGyroService->start();
  //pImuMagService->start();

  BLEDevice::startAdvertising();

#if _DEBUG_
  Serial.print("BLE initialised: OK");
  //  Serial.print(button.getI2Caddress(), HEX);
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
      //too soon? Yup
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
        wifi_value = "X";
      }
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: WIFI: ");
      Serial.print(wifi_value);
      Serial.println();
#endif
      pWifi->setValue(wifi_value.c_str());
      pWifi->notify();
    }

    if (pIpaddr) {
      if (NET.isEnabled()) {
        ipaddr_value = NET.getIpAddress();
      } else {
        ipaddr_value = "X";
      }
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: IPAD: ");
      Serial.print(ipaddr_value);
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





    //    // Capability
    //    if (pCPressure) {
    //      //int cpressure_value = true ? 1 : 0;
    //      cpressure_value = BMP.hasPressure() ? 1 : 0;
    //#if _DEBUG_ && _XDEBUG_
    //      Serial.print("Writing: CPRESSURE: ");
    //      Serial.print(cpressure_value ? "TRUE" : "FALSE");
    //      Serial.println();
    //#endif
    //      pCPressure->setValue(&cpressure_value,1);
    //      pCPressure->notify();
    //    }
    //
    //    if (pCTemperature) {
    //    //int ctemperature_value = true ? 1 : 0;
    //      ctemperature_value = BMP.hasTemperature() ? 1 : 0;
    //#if _DEBUG_ && _XDEBUG_
    //      Serial.print("Writing: CTEMP: ");
    //      Serial.print(ctemperature_value ? "TRUE" : "FALSE");
    //      Serial.println();
    //#endif
    //      pCTemperature->setValue(&ctemperature_value,1);
    //      pCTemperature->notify();
    //    }
    //
    //    if (pCAccelerometer) {
    //    //int caccelerometer_value = true ? 1 : 0;
    //      caccelerometer_value = IMU.hasAcc() ? 1 : 0;
    //#if _DEBUG_ && _XDEBUG_
    //      Serial.print("Writing: CACC: ");
    //      Serial.print(caccelerometer_value ? "TRUE" : "FALSE");
    //      Serial.println();
    //#endif
    //      pCAccelerometer->setValue(&caccelerometer_value,1);
    //      pCAccelerometer->notify();
    //    }
    //
    //    if (pCGyroscope) {
    //    //int cgyroscope_value = true ? 1 : 0;
    //      cgyroscope_value = IMU.hasAcc() ? 1 : 0;
    //#if _DEBUG_ && _XDEBUG_
    //      Serial.print("Writing: CGYRO: ");
    //      Serial.print(cgyroscope_value ? "TRUE" : "FALSE");
    //      Serial.println();
    //#endif
    //      pCGyroscope->setValue(&cgyroscope_value,1);
    //      pCGyroscope->notify();
    //    }
    //
    //    if (pCMagnetometer) {
    //    //int cmagnetometer_value = false ? 1 : 0;
    //      cmagnetometer_value = IMU.hasMag() ? 1 : 0;
    //#if _DEBUG_ && _XDEBUG_
    //      Serial.print("Writing: CMAG: ");
    //      Serial.print(cmagnetometer_value ? "TRUE" : "FALSE");
    //      Serial.println();
    //#endif
    //      pCMagnetometer->setValue(&cmagnetometer_value,1);
    //      pCMagnetometer->notify();
    //    }






    // Flags
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

    if (pBmpEnabled) {
      bmp_enabled_value = BMP.isEnabled() ? 1 : 0;
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: BMP: ");
      Serial.print(bmp_enabled_value ? "TRUE" : "FALSE");
      Serial.println();
#endif
      pBmpEnabled->setValue(&bmp_enabled_value, 1);
      pBmpEnabled->notify();
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
      //int latitude_value = int(51.012323 * 10000000.0);
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
      //int longitude_value = (0.4612323 * 10000000.0);
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
      //int altitude_value = int(1.234 * 1000.0); //int(23.441*1000);
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






    // BMP
    if (pPressure) {
      //int pressure_value = int(1013.25 * 1000.0);
      pressure_value = int(BMP.getPressure() * 1000.0);
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
      //int temperature_value = int(18.12 * 100.0);
      temperature_value = int(BMP.getTemperature() * 100.0);
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
      //int accx_value = int(0.1 * 10000.);
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
      //int accx_value = int(0.1 * 10000.);
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
      //int accx_value = int(0.1 * 10000.);
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
      //int accx_value = int(0.1 * 10000.);
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
      //int accy_value = int(0.1 * 10000.);
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
      //int gyroz_value = int(0.1 * 10000.);
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
  }
}

#endif // _USE_BLE_
