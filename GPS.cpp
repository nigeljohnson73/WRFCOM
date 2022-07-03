#include "GPS.h"

#include <Wire.h> //Needed for I2C to GPS
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

SFE_UBLOX_GNSS myGNSS;
TrGPS GPS;

void callbackPVT(UBX_NAV_PVT_data_t *ubxDataStruct) {
  Serial.println(F("Hey! The NAV PVT callback has been called!"));
  //  Serial.println("GPS initialised: conecting");
  //  Serial.println("  GPS connected: TBD");
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
  Wire.begin();

  //myGNSS.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  if (!myGNSS.begin()) {
#if _DEBUG_
    Serial.println("GPS disconnected");
#endif
    return;
  }
#if _DEBUG_
  Serial.println("GPS initialised: connecting");
#endif

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR

  myGNSS.setNavigationFrequency(SENSOR_HZ); //Produce X solutions per second

  myGNSS.setAutoPVTcallbackPtr(&callbackPVT); // Enable automatic NAV PVT messages with callback to callbackPVT
}

void TrGPS::loop() {
  if (!isEnabled() || !isConnected()) return;

  myGNSS.checkUblox(); // Check for the arrival of new data and process it.

  // Check if new NAV PVT data has been received:
  // If myGNSS.packetUBXNAVPVT->automaticFlags.flags.bits.callbackCopyValid is true, it indicates new PVT data has been received and has been copied.
  // automaticFlags.flags.bits.callbackCopyValid will be cleared automatically when the callback is called.

  if (myGNSS.packetUBXNAVPVT->automaticFlags.flags.bits.callbackCopyValid == true) {
    // But, we can manually clear the callback flag too. This will prevent the callback from being called!
    //myGNSS.packetUBXNAVPVT->automaticFlags.flags.bits.callbackCopyValid = false; // Comment this line if you still want the callback to be called

    Serial.println();

    uint16_t hms16 = 0;
    uint8_t hms = 0;
    //String timestamp = "";
    _timestamp = "";

    hms16 = myGNSS.packetUBXNAVPVT->callbackData->year;
    if (hms < 1000) _timestamp += F("0");
    if (hms < 100) _timestamp += F("0");
    if (hms < 10) _timestamp += F("0");
    _timestamp += hms;
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

    Serial.print(F("Time: "));
    Serial.print(_timestamp);
    //    Serial.print(F(", "));

    //    Serial.print(F("."));
    //    unsigned long millisecs = myGNSS.packetUBXNAVPVT->callbackData->iTOW % 1000; // Print the milliseconds
    //    if (millisecs < 100) Serial.print(F("0")); // Print the trailing zeros correctly
    //    if (millisecs < 10) Serial.print(F("0"));
    //    Serial.print(millisecs);
    //    Serial.print(F("Z"));

    long lat = myGNSS.packetUBXNAVPVT->callbackData->lat;
    _lat = double(lat) / (10000000.); // degrees *10^7 to degrees
    Serial.print(F(", "));
    Serial.print(F("Lat: "));
    Serial.print(_lat);
    Serial.print(F("N"));

    //    Serial.print(F(" Lat: "));
    //    Serial.print(latitude);

    long lng = myGNSS.packetUBXNAVPVT->callbackData->lon;
    _lat = double(lat) / (10000000.); // degrees *10^7 to degrees
    Serial.print(F(", "));
    Serial.print(F("Lng: "));
    Serial.print(_lng);
    Serial.print(F("E"));
    //    Serial.print(F(" Lng: "));
    //    Serial.print(longitude);
    //    Serial.print(F(" (degrees * 10^-7)"));

    long alt = myGNSS.packetUBXNAVPVT->callbackData->hMSL; // Print the height above mean sea level
    _lat = double(lat) / (1000.); // mm to metres
    Serial.print(F(", "));
    Serial.print(F("Alt: "));
    Serial.print(_alt);
    Serial.print(F(" m"));
    //    Serial.print(F(" Alt: "));
    //    Serial.print(altitude);
    //    Serial.println(F(" (mm)"));
  }

  _siv = myGNSS.getSIV();
  Serial.print(F(", "));
  Serial.print(F("SIV: "));
  Serial.print(_siv);

  _connected = _siv > 3; // Can't rememember what a good lock is
  Serial.print(F(", "));
  Serial.print(F("lock: "));
  Serial.print(_connected?"Yes":"No");


  Serial.println();

  myGNSS.checkCallbacks(); // Check if any callbacks are waiting to be processed. There will not be any in this example, unless you commented the line above
  //  delay(50);
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
