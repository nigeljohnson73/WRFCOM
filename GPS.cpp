#include "GPS.h"

#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

SFE_UBLOX_GNSS myGNSS;
TrGPS GPS;

void callbackPVT(UBX_NAV_PVT_data_t *ubxDataStruct) {
  //  Serial.println(F("Hey! The NAV PVT callback has been called!"));
}

// This might be better judt to query the module directly
// https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library/blob/main/examples/Example3_GetPosition/Example3_GetPosition.ino
// https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library/blob/main/examples/Example13_PVT/Example1_AutoPVT/Example1_AutoPVT.ino
// https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library/blob/main/examples/Example5_SpeedHeadingPrecision/Example5_SpeedHeadingPrecision.ino

// refresh rate an max wait: setHighPrecisionMode()
// https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library/blob/main/src/SparkFun_u-blox_GNSS_Arduino_Library.h#L991
// https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library/blob/main/src/SparkFun_u-blox_GNSS_Arduino_Library.h#L1450

// Speeding up polling rates:
// https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library/blob/main/examples/Example25_MeasurementAndNavigationRate/Example25_MeasurementAndNavigationRate.ino

// Some how calibrate the sensor ofr IMU and dead reckoning... dont' think I got it on the ZOE-M8Q
// https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library/blob/main/examples/Dead_Reckoning/Example1_calibrateSensor/Example1_calibrateSensor.ino
// https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library/blob/main/examples/Dead_Reckoning/Example4_vehicleDynamics/Example4_vehicleDynamics.ino


TrGPS::TrGPS() {};

void TrGPS::begin() {
  //  Wire.begin();

  //myGNSS.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  if (!myGNSS.begin()) {
#if _DEBUG_
    Serial.println("GPS disconnected");
#endif
    return;
  }

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR

  //myGNSS.setNavigationFrequency(SENSOR_HZ); //Produce X solutions per second
  if (myGNSS.setNavigationFrequency(SENSOR_HZ)) {
    _refresh_hz = SENSOR_HZ;
#if _DEBUG && _XDEBUG_
    Serial.print(F("GPS: Refresh rate set to "));
    Serial.print(SENSOR_HZ);
    Serial.print(F(" Hz"));
    Serial.println();
#endif
#if _DEBUG_
  } else {
    Serial.print(F("GPS: failed to set "));
    Serial.print(SENSOR_HZ);
    Serial.print(F(" fps"));
    Serial.println();
#endif
  }

  myGNSS.setAutoPVTcallbackPtr(&callbackPVT); // Enable automatic NAV PVT messages with callback to callbackPVT

#if _DEBUG_
  Serial.print("GPS initialised: ");
  Serial.print(_refresh_hz);
  Serial.print(" Hz");
  Serial.println();
#endif
  _enabled = true;
}

void TrGPS::loop() {
  if (!isEnabled()) return;
  bool dirty = false;

  myGNSS.checkUblox(); // Check for the arrival of new data and process it.

  if (myGNSS.packetUBXNAVPVT->automaticFlags.flags.bits.callbackCopyValid == true) {
#if _DEBUG_
    String o_timestamp = _timestamp;
#endif

    uint16_t hms16 = 0;
    uint8_t hms = 0;

    _timestamp = "";

    hms16 = myGNSS.packetUBXNAVPVT->callbackData->year;
    if (hms16 < 1000) _timestamp += F("0");
    if (hms16 < 100) _timestamp += F("0");
    if (hms16 < 10) _timestamp += F("0");
    _timestamp += hms16;
    _timestamp += F("-");

    hms = myGNSS.packetUBXNAVPVT->callbackData->month;
    if (hms < 10) _timestamp += F("0");
    _timestamp += hms;
    _timestamp += F("-");

    hms = myGNSS.packetUBXNAVPVT->callbackData->day;
    if (hms < 10) _timestamp += F("0");
    _timestamp += hms;
    _timestamp += F("T");

    hms = myGNSS.packetUBXNAVPVT->callbackData->hour;
    if (hms < 10) _timestamp += F("0");
    _timestamp += hms;
    _timestamp += F(":");

    hms = myGNSS.packetUBXNAVPVT->callbackData->min;
    if (hms < 10) _timestamp += F("0");
    _timestamp += hms;
    _timestamp += F(":");

    hms = myGNSS.packetUBXNAVPVT->callbackData->sec;
    if (hms < 10) _timestamp += F("0");
    _timestamp += hms;
    _timestamp += F("Z");

#if _DEBUG_ && _XDEBUG_
    if (_timestamp.length() > 0 && _timestamp != o_timestamp) dirty = true;
#endif

    long lat = myGNSS.packetUBXNAVPVT->callbackData->lat;
    _lat = double(lat) / (10000000.); // degrees *10^7 to degrees

    long lng = myGNSS.packetUBXNAVPVT->callbackData->lon;
    _lng = double(lng) / (10000000.); // degrees *10^7 to degrees

    long alt = myGNSS.packetUBXNAVPVT->callbackData->hMSL;
    _alt = double(alt) / (1000.); // mm to metres
  }

  int t = myGNSS.getSIV();
  if (t < 50 && t != _siv) {
    dirty = true;
    _siv = t;
  }

  _connected = _siv > 3; // You need at least 3 satellites to ensure 3D accuracy

#if _DEBUG_
  if (dirty) {
    Serial.print(F("Time: "));
    Serial.print(_timestamp);
    Serial.print(F(", "));
    Serial.print(F("Lat: "));
    Serial.print(_lat);
    Serial.print(F("N"));
    Serial.print(F(", "));
    Serial.print(F("Lng: "));
    Serial.print(_lng);
    Serial.print(F("E"));
    Serial.print(F(", "));
    Serial.print(F("Alt: "));
    Serial.print(_alt);
    Serial.print(F(" m"));
    Serial.print(F(", "));
    Serial.print(F("SIV: "));
    Serial.print(_siv);
    Serial.print(F(", "));
    Serial.print(F("lock: "));
    Serial.print(_connected ? "Yes" : "No");
    Serial.println();
  }
#endif

  // Reset the indicator as read, and wait for the next batch
  myGNSS.checkCallbacks();
}


String TrGPS::getTimestamp() {
  if (!isEnabled() || !isConnected()) return "";
  return _timestamp;
}

double TrGPS::getLatitude() {
  if (!isEnabled() || !isConnected()) return 0.;
  return _lat;
}

double TrGPS::getLongitude() {
  if (!isEnabled() || !isConnected()) return 0.;
  return _lng;
}

double TrGPS::getAltitude() {
  if (!isEnabled() || !isConnected()) return 0.;
  return _alt;
}

int TrGPS::getSatsInView() {
  if (!isEnabled() || !isConnected()) return 0;
  return _siv;
}
