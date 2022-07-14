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
#include <BLE2904.h>

bool deviceConnected = false;
bool oldDeviceConnected = false;

BLEServer* pServer = NULL;

BLECharacteristic* pGpsEnabled = NULL;
BLECharacteristic* pGpsLocked = NULL;
BLECharacteristic* pSiv = NULL;
BLECharacteristic* pLatitude = NULL;
BLECharacteristic* pLongitude = NULL;
BLECharacteristic* pAltitude = NULL;
BLECharacteristic* pBmpEnabled = NULL;
BLECharacteristic* pPressure = NULL;
BLECharacteristic* pTemperature = NULL;
BLECharacteristic* pBmsEnabled = NULL;
BLECharacteristic* pBattery = NULL;

BLE2904* pBle2904_gps_enabled = NULL;
BLE2904* pBle2904_gps_locked = NULL;
BLE2904* pBle2904_siv = NULL;
BLE2904* pBle2904_latitude = NULL;
BLE2904* pBle2904_longitude = NULL;
BLE2904* pBle2904_altitude = NULL;
BLE2904* pBle2904_bmp_enabled = NULL;
BLE2904* pBle2904_pressure = NULL;
BLE2904* pBle2904_temperature = NULL;
BLE2904* pBle2904_bms_enabled = NULL;
BLE2904* pBle2904_battery = NULL;

int bms_enabled_value = true ? 1 : 0;
int battery_value = int(12.34);
int gps_enabled_value = true ? 1 : 0;
int gps_locked_value = false ? 1 : 0;
int siv_value = 0;
int latitude_value = int(51.012323 * 10000000.0);
int longitude_value = (0.4612323 * 10000000.0);
int altitude_value = int(1.234 * 1000.0); //int(23.441*1000);
int bmp_enabled_value = true ? 1 : 0;
int pressure_value = int(1013.25 * 1000.0);
int temperature_value = int(18.12 * 100.0);

// Full GATT charactersitics:
// https://gist.github.com/sam016/4abe921b5a9ee27f67b3686910293026
//
// Format types:
// https://btprodspecificationrefs.blob.core.windows.net/assigned-numbers/Assigned%20Number%20Types/Format%20Types.pdf
//
// Characteristic and unit definitions:
// https://btprodspecificationrefs.blob.core.windows.net/assigned-values/16-bit%20UUID%20Numbers%20Document.pdf

#define WRFCOM_SERVICE_NAME             "WRFCOM"
#define WRFCOM_CORE_SERVICE_UUID        "9ddf3d45-ea85-467a-9b23-34a9e4900000"
#define WRFCOM_FLAGS_SERVICE_UUID       "9ddf3d45-ea85-467a-9b23-34a9e4900100"
#define WRFCOM_GPS_SERVICE_UUID         "9ddf3d45-ea85-467a-9b23-34a9e4900200"
#define WRFCOM_BMP_SERVICE_UUID         "9ddf3d45-ea85-467a-9b23-34a9e4900300"
#define WRFCOM_IMU_SERVICE_UUID         "9ddf3d45-ea85-467a-9b23-34a9e4900400"

#define BMSENABLED_CHARACTERISTIC_UUID  "9ddf3d45-ea85-467a-9b23-34a9e4900101"
#define GPSENABLED_CHARACTERISTIC_UUID  "9ddf3d45-ea85-467a-9b23-34a9e4900102"
#define GPSLOCKED_CHARACTERISTIC_UUID   "9ddf3d45-ea85-467a-9b23-34a9e4900103"
#define BMPENABLED_CHARACTERISTIC_UUID  "9ddf3d45-ea85-467a-9b23-34a9e4900104"

#define BATTERY_CHARACTERISTIC_UUID     BLEUUID((uint16_t)0x2A19)

#define SIV_CHARACTERISTIC_UUID         "9ddf3d45-ea85-467a-9b23-34a9e4900201"
#define LATITUDE_CHARACTERISTIC_UUID    BLEUUID((uint16_t)0x2AAE)
#define LONGITUDE_CHARACTERISTIC_UUID   BLEUUID((uint16_t)0x2AAF)
#define ALTITUDE_CHARACTERISTIC_UUID    BLEUUID((uint16_t)0x2AB3)

#define PRESSURE_CHARACTERISTIC_UUID    BLEUUID((uint16_t)0x2A6D)
#define TEMPERATURE_CHARACTERISTIC_UUID BLEUUID((uint16_t)0x2A6E)

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
//  BLEService *pService = pServer->createService(WRFCOM_SERVICE_UUID);
  BLEService *pFlagsService = pServer->createService(WRFCOM_FLAGS_SERVICE_UUID);
  BLEService *pCoreService = pServer->createService(WRFCOM_CORE_SERVICE_UUID);
  BLEService *pGpsService = pServer->createService(WRFCOM_GPS_SERVICE_UUID);
  BLEService *pBmpService = pServer->createService(WRFCOM_BMP_SERVICE_UUID);

  // Create BLE Characteristics
  pBmsEnabled = pFlagsService->createCharacteristic(
                  BMSENABLED_CHARACTERISTIC_UUID,
                  BLECharacteristic::PROPERTY_READ   |
                  BLECharacteristic::PROPERTY_NOTIFY |
                  BLECharacteristic::PROPERTY_INDICATE
                );
  pBle2904_bms_enabled = new BLE2904();
  pBle2904_bms_enabled->setFormat(0x01); // unsigned bit
  pBle2904_bms_enabled->setUnit( 0x2AE2); // Boolean
  pBmsEnabled->addDescriptor(pBle2904_bms_enabled);

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

  pGpsEnabled = pFlagsService->createCharacteristic(
                  GPSENABLED_CHARACTERISTIC_UUID,
                  BLECharacteristic::PROPERTY_READ   |
                  BLECharacteristic::PROPERTY_NOTIFY |
                  BLECharacteristic::PROPERTY_INDICATE
                );
  pBle2904_gps_enabled = new BLE2904();
  pBle2904_gps_enabled->setFormat(0x01); // unsigned bit
  pBle2904_gps_enabled->setUnit( 0x2AE2); // Boolean
  pGpsEnabled->addDescriptor(pBle2904_gps_enabled);

  pGpsLocked = pFlagsService->createCharacteristic(
                 GPSLOCKED_CHARACTERISTIC_UUID,
                 BLECharacteristic::PROPERTY_READ   |
                 BLECharacteristic::PROPERTY_NOTIFY |
                 BLECharacteristic::PROPERTY_INDICATE
               );
  pBle2904_gps_locked = new BLE2904();
  pBle2904_gps_locked->setFormat(0x01); // unsigned bit
  pBle2904_gps_locked->setUnit( 0x2AE2); // Boolean
  pGpsLocked->addDescriptor(pBle2904_gps_locked);

  pSiv = pGpsService->createCharacteristic(
           SIV_CHARACTERISTIC_UUID,
           BLECharacteristic::PROPERTY_READ   |
           BLECharacteristic::PROPERTY_NOTIFY |
           BLECharacteristic::PROPERTY_INDICATE
         );
  pBle2904_siv = new BLE2904();
  pBle2904_siv->setFormat(0x08); // unsigned int32
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

  pBmpEnabled = pFlagsService->createCharacteristic(
                  BMPENABLED_CHARACTERISTIC_UUID,
                  BLECharacteristic::PROPERTY_READ   |
                  BLECharacteristic::PROPERTY_NOTIFY |
                  BLECharacteristic::PROPERTY_INDICATE
                );
  pBle2904_bmp_enabled = new BLE2904();
  pBle2904_bmp_enabled->setFormat(0x01); // unsigned bit
  pBle2904_bmp_enabled->setUnit( 0x2AE2); // Boolean
  pBmpEnabled->addDescriptor(pBle2904_bmp_enabled);

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

  // Configure advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(WRFCOM_CORE_SERVICE_UUID);
  pAdvertising->addServiceUUID(WRFCOM_FLAGS_SERVICE_UUID);
  pAdvertising->addServiceUUID(WRFCOM_GPS_SERVICE_UUID);
  pAdvertising->addServiceUUID(WRFCOM_BMP_SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter

  // Start the service
  pCoreService->start();
  pFlagsService->start();
  pGpsService->start();
  pBmpService->start();
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

    if (pGpsEnabled) {
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: GPSE: ");
      Serial.print(gps_enabled_value ? "TRUE" : "FALSE");
      Serial.println();
#endif
      pGpsEnabled->setValue(gps_enabled_value);
      pGpsEnabled->notify();
      gps_enabled_value = !gps_enabled_value;
    }

    if (pGpsLocked) {
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: GPSL: ");
      Serial.print(gps_locked_value ? "TRUE" : "FALSE");
      Serial.println();
#endif
      pGpsLocked->setValue(gps_locked_value);
      pGpsLocked->notify();
      gps_locked_value = !gps_locked_value;
    }

    if (pSiv) {
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: SIV: ");
      Serial.print(siv_value);
      Serial.println();
#endif
      pSiv->setValue(siv_value);
      pSiv->notify();
      siv_value += 1;
      if (siv_value > 12) siv_value = 0;
    }

    if (pLatitude) {
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: LAT: ");
      Serial.print(latitude_value);
      Serial.print(" deg");
      Serial.println();
#endif
      pLatitude->setValue(latitude_value);
      pLatitude->notify();
      latitude_value += 1;
    }

    if (pLongitude) {
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: LNG: ");
      Serial.print(longitude_value);
      Serial.print(" deg");
      Serial.println();
#endif
      pLongitude->setValue(longitude_value);
      pLongitude->notify();
      longitude_value += 1;
    }

    if (pAltitude) {
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: ALT: ");
      Serial.print(altitude_value);
      Serial.print(" m");
      Serial.println();
#endif
      pAltitude->setValue(altitude_value);
      pAltitude->notify();
      altitude_value += 1;
    }

    if (pBmpEnabled) {
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: BMP: ");
      Serial.print(bmp_enabled_value ? "TRUE" : "FALSE");
      Serial.println();
#endif
      pBmpEnabled->setValue(bmp_enabled_value);
      pBmpEnabled->notify();
      bmp_enabled_value = !bmp_enabled_value;
    }

    if (pPressure) {
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: HPA: ");
      Serial.print(pressure_value);
      Serial.print(" Pa");
      Serial.println();
#endif
      pPressure->setValue(pressure_value);
      pPressure->notify();
      pressure_value += 1;
    }

    if (pTemperature) {
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: TMP: ");
      Serial.print(temperature_value);
      Serial.print(" C");
      Serial.println();
#endif
      pTemperature->setValue(temperature_value);
      pTemperature->notify();
      temperature_value += 1;
    }

    if (pBmsEnabled) {
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: BMSE: ");
      Serial.print(bms_enabled_value ? "TRUE" : "FALSE");
      Serial.println();
#endif
      pBmsEnabled->setValue(bms_enabled_value);
      pBmsEnabled->notify();
      bms_enabled_value = !bms_enabled_value;
    }

    if (pBattery) {
#if _DEBUG_ && _XDEBUG_
      Serial.print("Writing: BAT: ");
      Serial.print(battery_value);
      Serial.println();
#endif
      pBattery->setValue(battery_value);
      pBattery->notify();
      battery_value += 1;
      if (battery_value > 100) battery_value = 0;
    }
  }
}

#endif // _USE_BLE_
