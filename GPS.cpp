#include "GPS.h"

#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

SFE_UBLOX_GNSS myGNSS;
TrGPS GPS;

#define smooth_max 3
static double lat_buff[smooth_max];
static double lng_buff[smooth_max];
static double alt_buff[smooth_max];
static int smooth_count = 0;
static int smooth_points = 0;

static double last_lat = DUFF_VALUE;
static double last_lng = DUFF_VALUE;
static double last_alt = DUFF_VALUE;
static unsigned long last_track = 0;
static unsigned long _connection_at = 0;

void callbackPVT(UBX_NAV_PVT_data_t *ubxDataStruct) {
  //  Serial.println(F("Hey! The NAV PVT callback has been called!"));
}

// Useful things to look at:
// uint8_t getFixType(uint16_t maxWait = defaultMaxWait); // Returns the type of fix: 0=no, 3=3D, 4=GNSS+Deadreckoning
// bool getGnssFixOk(uint16_t maxWait = defaultMaxWait); // Get whether we have a valid fix (i.e within DOP & accuracy masks)
// bool getDiffSoln(uint16_t maxWait = defaultMaxWait);  // Get whether differential corrections were applied
// // Enable/Disable NMEA High Precision Mode - include extra decimal places in the Lat and Lon
// bool setHighPrecisionMode(bool enable = true, uint16_t maxWait = defaultMaxWait);

// This might be better just to query the module directly
// https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library/blob/main/examples/Example3_GetPosition/Example3_GetPosition.ino
// https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library/blob/main/examples/Example13_PVT/Example1_AutoPVT/Example1_AutoPVT.ino
// https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library/blob/main/examples/Example5_SpeedHeadingPrecision/Example5_SpeedHeadingPrecision.ino

// refresh rate an max wait: setHighPrecisionMode()
// https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library/blob/main/src/SparkFun_u-blox_GNSS_Arduino_Library.h#L991
// https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library/blob/main/src/SparkFun_u-blox_GNSS_Arduino_Library.h#L1450

// Speeding up polling rates:
// https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library/blob/main/examples/Example25_MeasurementAndNavigationRate/Example25_MeasurementAndNavigationRate.ino

// Some how calibrate the sensor ofr IMU and dead reckoning... dont' think I got it on the ZOE-M8Q or the SAM-M8Q
// https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library/blob/main/examples/Dead_Reckoning/Example1_calibrateSensor/Example1_calibrateSensor.ino
// https://github.com/sparkfun/SparkFun_u-blox_GNSS_Arduino_Library/blob/main/examples/Dead_Reckoning/Example4_vehicleDynamics/Example4_vehicleDynamics.ino


/************************************************************************************************************************************************************
                              888888ba                             oo
                              88    `8b
  .d8888b. 88d888b. .d8888b. a88aaaa8P' .d8888b. .d8888b. 88d888b. dP 88d888b. .d8888b.
  88'  `88 88'  `88 Y8ooooo.  88   `8b. 88ooood8 88'  `88 88'  `88 88 88'  `88 88'  `88
  88.  .88 88.  .88       88  88    .88 88.  ... 88.  .88 88       88 88    88 88.  .88
  `8888P88 88Y888P' `88888P'  88888888P `88888P' `88888P8 dP       dP dP    dP `8888P88
       .88 88                                                                       .88
   d8888P  dP                                                                   d8888P
*/
double gpsBearing(double lat1, double lng1, double lat2, double lng2) {
  // https://towardsdatascience.com/calculating-the-bearing-between-two-geospatial-coordinates-66203f57e4b4
  double dL = lng2 - lng1;
  double X = cos(lat2) * sin(dL);
  double Y = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dL);
  double bearing = fmod((atan2(X, Y) * (180 / PI)) + 360, 360);

  return bearing;
}

/************************************************************************************************************************************************************
                             888888ba  oo            dP
                             88    `8b               88
  .d8888b. 88d888b. .d8888b. 88     88 dP .d8888b. d8888P .d8888b. 88d888b. .d8888b. .d8888b.
  88'  `88 88'  `88 Y8ooooo. 88     88 88 Y8ooooo.   88   88'  `88 88'  `88 88'  `"" 88ooood8
  88.  .88 88.  .88       88 88    .8P 88       88   88   88.  .88 88    88 88.  ... 88.  ...
  `8888P88 88Y888P' `88888P' 8888888P  dP `88888P'   dP   `88888P8 dP    dP `88888P' `88888P'
       .88 88
   d8888P  dP
*/
double gpsDistance(double lat1, double lng1, double lat2, double lng2) {
  double haversine;
  double temp;
  double dist;

  lat1 = lat1  * (PI / 180.);
  lng1 = lng1 * (PI / 180.);
  lat2 = lat2  * (PI / 180.);
  lng2 = lng2 * (PI / 180.);

  haversine = (pow(sin((1.0 / 2) * (lat2 - lat1)), 2)) + ((cos(lat1)) * (cos(lat2)) * (pow(sin((1.0 / 2) * (lng2 - lng1)), 2)));
  temp = 2 * asin(min(1.0, sqrt(haversine)));
  dist = EARTH_RADIUS * temp;

  return dist;
}

/************************************************************************************************************************************************************
                                                                 888888ba             dP
                                                                 88    `8b            88
  88d888b. 88d888b. .d8888b. .d8888b. .d8888b. .d8888b. .d8888b. 88     88 .d8888b. d8888P .d8888b.
  88'  `88 88'  `88 88'  `88 88'  `"" 88ooood8 Y8ooooo. Y8ooooo. 88     88 88'  `88   88   88'  `88
  88.  .88 88       88.  .88 88.  ... 88.  ...       88       88 88    .8P 88.  .88   88   88.  .88
  88Y888P' dP       `88888P' `88888P' `88888P' `88888P' `88888P' 8888888P  `88888P8   dP   `88888P8
  88
  dP
*/
void TrGPS::processData() {
  if (isEnabled() && isConnected()) {
    unsigned long track = millis();

    if (last_track > 0) {
      // If there was a last time, we can do deltas

      double fall = _alt - last_alt;
      double crawl = gpsDistance(last_lat, last_lng, _lat, _lng);
      double linear_distance = sqrt(pow(fall, 2) + pow(crawl, 2));

      if (linear_distance >= 0.01) {
        // But only do deltas if we have actually moved more than a centimeter

        double t_delta = double(track - last_track) / 1000.0;
        _ground_speed = crawl / t_delta;
        _travel_speed = linear_distance / t_delta;
        _travel_bearing = gpsBearing(last_lat, last_lng, _lat, _lng);
        _travel_elevation = atan2(fall, crawl) * (180 / PI);

        _moved = true;
      } else {
        _moved = false;
        _travel_speed = 0;
      } // linear distance check
    } // last track > 0

    last_lat = _lat;
    last_lng = _lng;
    last_alt = _alt;
    last_track = track;
  } else {
    _moved = false;
    _travel_speed = 0;
    _travel_bearing = 0;
    _travel_elevation = 0;
    last_track = 0;
  }// Connected
}


/************************************************************************************************************************************************************
  dP                         oo
  88
  88d888b. .d8888b. .d8888b. dP 88d888b.
  88'  `88 88ooood8 88'  `88 88 88'  `88
  88.  .88 88.  ... 88.  .88 88 88    88
  88Y8888' `88888P' `8888P88 dP dP    dP
                         .88
                     d8888P
*/
void TrGPS::begin() {

  //myGNSS.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  if (!myGNSS.begin()) {
#if _DEBUG_
    Serial.println("GPS init: disconnected");
#endif
    return;
  }

  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR

  //myGNSS.setNavigationFrequency(SENSOR_HZ); //Produce X solutions per second
  if (myGNSS.setNavigationFrequency(SENSOR_HZ)) {
    _refresh_hz = SENSOR_HZ;
#if _DEBUG && _XDEBUG_
    Serial.print("GPS: Refresh rate set to ");
    Serial.print(_refresh_hz);
    Serial.print(" Hz");
    Serial.println();
#endif
  } else {
#if _DEBUG_
    Serial.print("GPS init: ");
    Serial.print("failed to set ");
    Serial.print(SENSOR_HZ);
    Serial.print(" Hz refresh");
    Serial.println();
#endif
    return;
  }

  myGNSS.setAutoPVTcallbackPtr(&callbackPVT); // Enable automatic NAV PVT messages with callback to callbackPVT

#if _DEBUG_
  Serial.print("GPS init: ");
  Serial.print(_refresh_hz);
  Serial.print(" Hz");
#endif

  if (myGNSS.setHighPrecisionMode()) {
#if _DEBUG_
    Serial.print(", High Precision mode");
  } else {
    Serial.print(", Regular Precision mode");
#endif
  }

#if _DEBUG_
  Serial.println();
#endif


  _enabled = true;
}

/************************************************************************************************************************************************************
  dP
  88
  88 .d8888b. .d8888b. 88d888b.
  88 88'  `88 88'  `88 88'  `88
  88 88.  .88 88.  .88 88.  .88
  dP `88888P' `88888P' 88Y888P'
                       88
                       dP
*/
void TrGPS::loop() {
  if (!isEnabled()) return;
  bool dirty = false;
  unsigned long now = millis();

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
    //    _lat = double(lat) / (10000000.); // degrees *10^7 to degrees
    lat_buff[smooth_count] = double(lat) / (10000000.); // degrees *10^7 to degrees

    long lng = myGNSS.packetUBXNAVPVT->callbackData->lon;
    //    _lng = double(lng) / (10000000.); // degrees *10^7 to degrees
    lng_buff[smooth_count] = double(lng) / (10000000.); // degrees *10^7 to degrees

    long alt = myGNSS.packetUBXNAVPVT->callbackData->hMSL;
    //    _alt = double(alt) / (1000.); // mm to metres
    alt_buff[smooth_count] = double(alt) / (1000.); // mm to metres

    smooth_points = min(smooth_max, smooth_points + 1);
    smooth_count += 1;
    if (smooth_count == smooth_max) {
      smooth_count = 0;
    }

    //Serial.print("--------------------------");
    //Serial.println();
    //Serial.print("smooth_points: ");
    //Serial.print(smooth_points);
    //Serial.print(", read lat: ");
    //Serial.print(lat_buff[smooth_count],7);
    //Serial.println();

    double c = 0.;

    c = 0.;
    for (int i = 0; i < smooth_points; i++) {
      c += lat_buff[i];
      //		Serial.print("i: ");
      //		Serial.print(i);
      //		Serial.print(", lat[]: ");
      //		Serial.print(lat_buff[i],7);
      //		Serial.print(", c: ");
      //		Serial.print(c, 7);
      //		Serial.println();
    }
    _lat = c / double(smooth_points);
    //		Serial.print("Smoothed _lat: ");
    //		Serial.print(_lat, 7);
    //		Serial.println();

    c = 0.;
    for (int i = 0; i < smooth_points; i++) {
      c += lng_buff[i];
    }
    _lng = c / double(smooth_points);

    c = 0.;
    for (int i = 0; i < smooth_points; i++) {
      c += alt_buff[i];
    }
    _alt = c / double(smooth_points);

    processData();

    //    // Not supported for some reason, possibly want the HNR PVT blob
    //    //    long speed = myGNSS.packetUBXNAVPVT->callbackData->speed; // mm/s
    //    //    _speed = double(speed) / (1000.); // mm to metres
    //
    //    long g_speed = myGNSS.packetUBXNAVPVT->callbackData->gSpeed; // mm/s
    //    _ground_speed = double(g_speed) / (1000.); // mm to metres

    // Accelerations and stuff - Line 905: https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library/blob/master/src/SparkFun_Ublox_Arduino_Library.h
  }

  int t = myGNSS.getSIV();
  if (t < 50 && t != _siv) {
    dirty = true;
    _siv = t;
  }

  if (_siv > 3) {
    if (!_connection_at == 0 ) {
      _connection_at = now;
      if (RTC.isEnabled()) {
#if _DEBUG_
        Serial.println("GPS::updateRtcTimestamp()");
#endif //_DEBUG_
        RTC.setTimestamp(_timestamp);
      }
      if (EMU.isEnabled()) {
        Serial.println("GPS::updateEmuAltitude()");
        EMU.setAltitude(getAltitude());
      }
    }
  } else {
    _connection_at = 0;
  }
  _connected = ((_connection_at - now) >= _confirmed_lock_millis) && (_siv > 3); // You need at least 3 satellites to ensure 3D accuracy, so wait for 4, and ensure the connection is stable

#if _DEBUG_
  if (false && dirty) {
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


/************************************************************************************************************************************************************
  .d8888b. .d8888b. 88d888b. .d8888b.
  88'  `"" 88'  `88 88'  `88 88ooood8
  88.  ... 88.  .88 88       88.  ...
  `88888P' `88888P' dP       `88888P'
*/
TrGPS::TrGPS() {};

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

double TrGPS::getGroundSpeed() {
  if (!isEnabled() || !isConnected()) return 0.;
  return _ground_speed;
}

double TrGPS::getTravelSpeed() {
  if (!isEnabled() || !isConnected()) return 0.;
  return _travel_speed;
}

double TrGPS::getTravelBearing() {
  if (!isEnabled() || !isConnected()) return 0.;
  return _travel_bearing;
}

double TrGPS::getTravelElevation() {
  if (!isEnabled() || !isConnected()) return 0.;
  return _travel_elevation;
}

int TrGPS::getSatsInView() {
  if (!isEnabled() || !isConnected()) return 0;
  return _siv;
}
