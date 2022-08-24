#include "LOG.h"

#include "SparkFun_Qwiic_OpenLog_Arduino_Library.h"
OpenLog myLog;

TrLOG LOG;

#ifndef PI
#define PI 3.14159265358979323846
#endif

#define EARTH_RADIUS 6372797.56085
#define ONE_G 9.80665
#define DUFF_VALUE -999999

static String comma = ",";

static String bms_percent;
static String bms_voltage;

static String emu_temperature;
static String emu_local_temperature;
static String emu_pressure;
static String emu_pressure_msl;
static String emu_altitude;

static String imu_temperature;
static String imu_acc;
static String imu_acc_x;
static String imu_acc_y;
static String imu_acc_z;
static String imu_gyro_x;
static String imu_gyro_y;
static String imu_gyro_z;
static String imu_mag_x;
static String imu_mag_y;
static String imu_mag_z;

static String gps_siv;
static String gps_latitude;
static String gps_longitude;
static String gps_altitude;
static String gps_distance;
static String gps_speed;
static String gps_bearing;
static String gps_elevation;

static bool _in_flight;
static bool _chute_deployed;
static String _reason;

static bool _launch_detected;
static bool _landing_detected;
static double _peak_speed;
static double _peak_acc;
static double _peak_gps_altitude;
static double _launch_altitude;
static double _launch_latitude;
static double _launch_longitude;
static double _max_height;
static double lat_raw;
static double lng_raw;
//static double _start_latitude;
//static double _start_longitude;
static double _final_latitude;
static double _final_longitude;
static double _final_ground_distance;
static double _furthest_ground_distance;

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
                      oo   dP            dP     dP                          dP
                           88            88     88                          88
  dP  dP  dP 88d888b. dP d8888P .d8888b. 88aaaaa88a .d8888b. .d8888b. .d888b88 .d8888b. 88d888b.
  88  88  88 88'  `88 88   88   88ooood8 88     88  88ooood8 88'  `88 88'  `88 88ooood8 88'  `88
  88.88b.88' 88       88   88   88.  ... 88     88  88.  ... 88.  .88 88.  .88 88.  ... 88
  8888P Y8P  dP       dP   dP   `88888P' dP     dP  `88888P' `88888P8 `88888P8 `88888P' dP
*/
void TrLOG::writeHeader() {
  String line = GPS.getTimestamp() + "\nmillis";
  line += comma + "bms_percent";
  line += comma + "bms_voltage";

  line += comma + "emu_temperature";
  line += comma + "emu_local_temperature";
  line += comma + "emu_pressure";
  line += comma + "emu_pressure_msl";
  line += comma + "emu_altitude";

  line += comma + "imu_temperature";
  line += comma + "imu_acc";
  line += comma + "imu_acc_x";
  line += comma + "imu_acc_y";
  line += comma + "imu_acc_z";
  line += comma + "imu_gyro_x";
  line += comma + "imu_gyro_y";
  line += comma + "imu_gyro_z";
  line += comma + "imu_mag_x";
  line += comma + "imu_mag_y";
  line += comma + "imu_mag_z";

  line += comma + "gps_siv";
  line += comma + "gps_latitude";
  line += comma + "gps_longitude";
  line += comma + "gps_altitude";
  line += comma + "gps_distance";
  line += comma + "gps_speed";
  line += comma + "gps_bearing";
  line += comma + "gps_elevation";

  line += comma + "_in_flight";
  line += comma + "_chute_deployed";
  line += comma + "_reason";
  myLog.println(line);
}

/************************************************************************************************************************************************************
                      oo   dP            888888ba             dP
                           88            88    `8b            88
  dP  dP  dP 88d888b. dP d8888P .d8888b. 88     88 .d8888b. d8888P .d8888b.
  88  88  88 88'  `88 88   88   88ooood8 88     88 88'  `88   88   88'  `88
  88.88b.88' 88       88   88   88.  ... 88    .8P 88.  .88   88   88.  .88
  8888P Y8P  dP       dP   dP   `88888P' 8888888P  `88888P8   dP   `88888P8
*/
void TrLOG::writeData() {
  String line = "";
  line += String(millis() - logging_started);

  line += comma + bms_percent;
  line += comma + bms_voltage;

  line += comma + emu_temperature;
  line += comma + emu_local_temperature;
  line += comma + emu_pressure;
  line += comma + emu_pressure_msl;
  line += comma + emu_altitude;

  line += comma + imu_temperature;
  line += comma + imu_acc;
  line += comma + imu_acc_x;
  line += comma + imu_acc_y;
  line += comma + imu_acc_z;
  line += comma + imu_gyro_x;
  line += comma + imu_gyro_y;
  line += comma + imu_gyro_z;
  line += comma + imu_mag_x;
  line += comma + imu_mag_y;
  line += comma + imu_mag_z;

  line += comma + gps_siv;
  line += comma + gps_latitude;
  line += comma + gps_longitude;
  line += comma + gps_altitude;
  line += comma + gps_distance;
  line += comma + gps_speed;
  line += comma + gps_bearing;
  line += comma + gps_elevation;

  line += comma + (_in_flight ? "Yes" : "No");
  line += comma + (_chute_deployed ? "Yes" : "No");
  line += comma + _reason;

  myLog.println(line);
}

/************************************************************************************************************************************************************
                                        dP   888888ba             dP
                                        88   88    `8b            88
  88d888b. .d8888b. .d8888b. .d8888b. d8888P 88     88 .d8888b. d8888P .d8888b.
  88'  `88 88ooood8 Y8ooooo. 88ooood8   88   88     88 88'  `88   88   88'  `88
  88       88.  ...       88 88.  ...   88   88    .8P 88.  .88   88   88.  .88
  dP       `88888P' `88888P' `88888P'   dP   8888888P  `88888P8   dP   `88888P8
*/
void TrLOG::resetData() {
  bms_percent = "";
  bms_voltage = "";

  emu_temperature = "";
  emu_local_temperature = "";
  emu_pressure = "";
  emu_pressure_msl = "";
  emu_altitude = "";

  imu_acc = "";
  imu_acc_x = "";
  imu_acc_y = "";
  imu_acc_z = "";
  imu_gyro_x = "";
  imu_gyro_y = "";
  imu_gyro_z = "";
  imu_mag_x = "";
  imu_mag_y = "";
  imu_mag_z = "";

  gps_siv = "";
  gps_latitude = "";
  gps_longitude = "";
  gps_altitude = "";
  gps_distance = "";
  gps_speed = "";
  gps_bearing = "";
  gps_elevation = "";

  _in_flight = false;
  _launch_detected = false;
  _landing_detected = false;
  _chute_deployed = false;
  _reason = "";

  _peak_speed = DUFF_VALUE;
  _peak_acc = DUFF_VALUE;
  _peak_gps_altitude = DUFF_VALUE;
  //  _start_latitude = DUFF_VALUE;
  //  _start_longitude = DUFF_VALUE;
  _launch_latitude = DUFF_VALUE;
  _launch_longitude = DUFF_VALUE;
  _launch_altitude = DUFF_VALUE;
  _max_height = DUFF_VALUE;
  _final_latitude = DUFF_VALUE;
  _final_longitude = DUFF_VALUE;
  _final_ground_distance = DUFF_VALUE;
  _furthest_ground_distance = DUFF_VALUE;
}

/************************************************************************************************************************************************************
                      dP   888888ba             dP
                      88   88    `8b            88
  .d8888b. .d8888b. d8888P 88     88 .d8888b. d8888P .d8888b.
  88'  `88 88ooood8   88   88     88 88'  `88   88   88'  `88
  88.  .88 88.  ...   88   88    .8P 88.  .88   88   88.  .88
  `8888P88 `88888P'   dP   8888888P  `88888P8   dP   `88888P8
      .88
   d8888P
*/
void TrLOG::getData() {
  if (BMS.isEnabled()) {
    bms_percent = BMS.getCapacityPercent();
    bms_voltage = BMS.getCapacityVoltage();
  }

  if (EMU.isEnabled()) {
    if (EMU.hasTemperature()) {
      emu_temperature = EMU.getTemperature();
    }
    emu_local_temperature = EMU.getLocalTemperature();
    if (EMU.hasPressure()) {
      emu_pressure = EMU.getPressure();
      emu_pressure_msl = EMU.getSeaLevelPressure();
      emu_altitude = EMU.getAltitude();
    }
  }

  if (IMU.isEnabled()) {
    if (IMU.hasTemp()) {
      imu_temperature = IMU.getTemperature();
    }
    if (IMU.hasAcc()) {
      imu_acc_x = IMU.getAccX();
      imu_acc_y = IMU.getAccY();
      imu_acc_z = IMU.getAccZ();
      double acc = sqrt(pow(IMU.getAccX(), 2) + pow(IMU.getAccY(), 2) + pow(IMU.getAccZ(), 2));
      _peak_acc = max(acc, _peak_acc);
      imu_acc = acc;
    }
    if (IMU.hasGyro()) {
      imu_gyro_x = IMU.getGyroX();
      imu_gyro_y = IMU.getGyroY();
      imu_gyro_z = IMU.getGyroZ();
    }
    if (IMU.hasMag()) {
      imu_mag_x = IMU.getMagX();
      imu_mag_y = IMU.getMagY();
      imu_mag_z = IMU.getMagZ();
    }
  }

  if (GPS.isEnabled()) {
    gps_siv = GPS.getSatsInView();
    // Blank the values so they are lot logged - we specifically have to calulate them this frame
    gps_latitude = "";
    gps_longitude = "";
    gps_altitude = "";
    gps_speed = "";
    gps_bearing = "";
    gps_elevation = "";
    gps_distance = "";

    if (GPS.isConnected()) {
      static unsigned long last_track;
      static double last_lat;
      static double last_lng;
      static double last_alt;

      unsigned long track = millis();
      lat_raw = GPS.getLatitude();
      lng_raw = GPS.getLongitude();
      double alt = GPS.getAltitude();
      gps_latitude = String(lat_raw, 7);
      gps_longitude = String(lng_raw, 7);
      gps_altitude = alt;

      // Speed is needed outside of the loop where it's calculated, so define it here
      double spd = DUFF_VALUE;

      // Calculate all the detlas from the last cycle
      if (last_track > 0) {
        // If there is a last time
        double dst_delta = gpsDistance(last_lat, last_lng, lat_raw, lng_raw);
        double alt_delta = last_alt - alt;
        double lin_dst = sqrt(pow(dst_delta, 2) + pow(alt_delta, 2));

        if (lin_dst > 0.001) {
          // We have moved since the last recorded value
          gps_elevation = atan2(alt_delta, dst_delta) * (180 / PI); // radians from atan2()
          gps_bearing = gpsBearing(last_lat, last_lng, lat_raw, lng_raw);

          double t_delta = ((double)(track - last_track)) / 1000.0;
          spd = lin_dst / t_delta; // m/s
          _peak_speed = max(spd, _peak_speed);
          gps_speed = spd;

          if (SRV.isArmed() && /*!_in_flight &&*/ spd > _launch_detect_speed) {
            _in_flight = true;
            if (!_launch_detected) {
              const char* str = "## LAUNCH DETECTED";
              myLog.println(str);
#if _DEBUG_
              Serial.println(str);
#endif
            }
            _launch_detected = true;
          } else {
            _in_flight = false;
          }

          if (SRV.isArmed() && !LOG.isCapturing() && _in_flight) {
            const char* str = "## LAUNCH DETECT CAPTURE STARTED";
            myLog.println(str);
#if _DEBUG_
            Serial.println(str);
#endif
            startCapture();
          }

          if (_deploy_apogee_offset != 0 && SRV.isArmed() && LOG.isCapturing() && _launch_detected && !_chute_deployed && alt < (_peak_gps_altitude + _deploy_apogee_offset)) {
            const char* str = "## PARACHUTE DEPLOYED (APOGEE)";
            myLog.println(str);
#if _DEBUG_
            Serial.println(str);
#endif
            _chute_deployed = true;
            _reason = "GPS Apogee";
            SRV.arm(false);
          }

          // Set the last track values
          last_track = track;
          last_lat = lat_raw;
          last_lng = lng_raw;
          last_alt = alt;

          //          myLog.println("## MOVE COMPLETE");
        }
      } else {
        last_track = track;
        myLog.println("## TRACKING ENABLED");
      }

      // If we havn't taken off yet, keep updating these until we do
      if (!_launch_detected) {
        _launch_latitude = lat_raw;
        _launch_longitude = lng_raw;
        _launch_altitude = alt;
      } else {
        _max_height = max(alt - _launch_altitude, _max_height);
        // If we haven't landed yet, update the distance
        // TODO: determine whether we have landed
        if (_launch_detected && !_landing_detected && spd > DUFF_VALUE && spd < 0.5) {
          const char* str = "## POSSIBLE LANDIND DETECTED";
          myLog.println(str);
#if _DEBUG_
          Serial.println(str);
#endif
          _landing_detected = true;
        }
        //        if (_launch_detected && !_landing_detected) {
        if (_launch_detected) {
          double dst = gpsDistance(_launch_latitude, _launch_longitude, lat_raw, lng_raw);
          _furthest_ground_distance = max(dst, _furthest_ground_distance);
          gps_distance = dst;
          if (_deploy_distance_offset != 0 && SRV.isArmed() && LOG.isCapturing() && !_chute_deployed && dst >= _deploy_distance_offset) {
            const char* str = "## PARACHUTE DEPLOYED (DISTANCE)";
            myLog.println(str);
#if _DEBUG_
            Serial.println(str);
#endif
            _chute_deployed = true;
            _reason = "GPS Distance";
            SRV.arm(false);
          }
        }
      }

    }
  }
}

/************************************************************************************************************************************************************
                                      dP
                                      88
  .d8888b. dP    dP 88d888b. .d8888b. 88        .d8888b. .d8888b.
  Y8ooooo. 88    88 88'  `88 88'  `"" 88        88'  `88 88'  `88
        88 88.  .88 88    88 88.  ... 88        88.  .88 88.  .88
  `88888P' `8888P88 dP    dP `88888P' 88888888P `88888P' `8888P88
                .88                                           .88
            d8888P                                        d8888P

*/
void TrLOG::syncLog() {

  _last_sync = millis();
  // Don't snyc if we are moving fast enough to be 'in-flight'
  // Don't use _in_flight because that stops once the parachute is deployed
  // HANDLED ELSEWHERE FOR NOW!!!!
  //  if (speed > _launch_detect_speed) {
  //    Serial.print("LOG::sync() - Skipping during flight");
  //    return;
  //  }
  if (!myLog.syncFile()) {
    Serial.print("LOG::sync() - Failed to sync file '");
    Serial.print(_log_fn);
    Serial.print("'");
    Serial.println();
    return;
  }

  long log_size = myLog.size(_log_fn);

  if (!myLog.append(_log_fn)) {
    Serial.print("LOG::sync() - Failed to open file '");
    Serial.print((_log_dir.length()) ? "/" : "");
    Serial.print(_log_dir);
    Serial.print("/");
    Serial.print(_log_fn);
    Serial.print("' to append to");
    Serial.println();
    return;
  }

  Serial.print("LOG::sync() - sync completed for '");
  Serial.print((_log_dir.length()) ? "/" : "");
  Serial.print(_log_dir);
  Serial.print("/");
  Serial.print(_log_fn);
  Serial.print("' ");
  double fs = log_size;
  String sm = "KB";
  fs = fs / 1024.;
  if (fs >= 1000) {
    fs = fs / 1024.;
    sm = "MB";
  }
  Serial.print(fs);
  Serial.print(" ");
  Serial.print(sm);
  Serial.println();
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
void TrLOG::begin() {

  //  Wire.begin();
  if (!myLog.begin()) {
#if _DEBUG_
    //Serial.println("LOG disconnected (OpenLog begin() failure)");
    Serial.println("LOG initialised: disconnected");
#endif
    return;
  }
  String ver = myLog.getVersion();
  if (ver == "255.255") {
#if _DEBUG_
    //Serial.println("LOG disconnected (OpenLog getVersion() failure)");
    Serial.println("LOG initialised: disconnected");
#endif
    return;
  }

#if _DEBUG_
  Serial.print("LOG initialised: version ");
  Serial.print(myLog.getVersion());
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
void TrLOG::loop() {
  if (!isEnabled()) return;

  getData();

  if (!isCapturing()) return;

  if ((millis() - logging_started) > (1000 * MAX_LOG_DURATION_SECONDS)) {
#if _XDEBUG_
    Serial.print("TrLOG::loop(): maximum log period exceeded");
    Serial.println();
#endif
    capture(false);
    return;
  }

  if ((_sync_interval) > 0 && ((millis() - _last_sync) >= _sync_interval)) {
    syncLog();
  }
  writeData();
}

/************************************************************************************************************************************************************
             dP                       dP    a88888b.                     dP
             88                       88   d8'   `88                     88
  .d8888b. d8888P .d8888b. 88d888b. d8888P 88        .d8888b. 88d888b. d8888P dP    dP 88d888b. .d8888b.
  Y8ooooo.   88   88'  `88 88'  `88   88   88        88'  `88 88'  `88   88   88    88 88'  `88 88ooood8
        88   88   88.  .88 88         88   Y8.   .88 88.  .88 88.  .88   88   88.  .88 88       88.  ...
  `88888P'   dP   `88888P8 dP         dP    Y88888P' `88888P8 88Y888P'   dP   `88888P' dP       `88888P'
                                                              88
                                                              dP
*/
void TrLOG::startCapture() {
  if (GPS.isEnabled() && GPS.isConnected()) {
    if (RTC.isEnabled()) {
      // Set RTC time from GPS;
      RTC.setTimestamp(GPS.getTimestamp());
    }
    if (EMU.isEnabled()) {
      // Tell the EMU what the amtitude is, so it can calculate what it needs to
      EMU.setAltitude(GPS.getAltitude());
    }
  }
  resetData();

  // get GPS, RTC time or network time in that order
  String fn = "";
  String header = "";
  if (GPS.isEnabled() && GPS.isConnected()) {
    //    _start_latitude = lat_raw; //GPS.getLatitude();
    //    _start_longitude = lng_raw; //GPS.getLongitude();
    //
    fn = GPS.getTimestamp();
  } else if (RTC.isEnabled()) {
    fn = RTC.getTimestamp();
  } else {
    fn = NET.getTimestamp();
  }

  // Fall back to millis, but should not be here operationally
  if (fn.length() == 0) {
    fn = millis();
    _log_dir = "";
  } else {
    // Store the  for processing later
    _log_ts = fn;

    // Remove all the crap so that the date and timeis only 8 and 6 chars long respectively
    fn.replace("-", "");
    fn.replace(":", "");
    fn.replace("Z", "");

    _log_dir = fn.substring(0, 8);
    fn = fn.substring(9);

    // Store the timestamp so we can track it in the file
    header = _log_ts + "\n";
  }

  fn += ".csv";
  _log_fn = fn;

  myLog.changeDirectory(".."); // Make sure we are in the root (Don't care if this fails, cuz we are in the root in that case)
  myLog.makeDirectory(_log_dir);
  myLog.changeDirectory(_log_dir);

  // Now create our log file where we are.
  if (!myLog.append(_log_fn)) {
    Serial.print("LOG: Failed to file '");
    Serial.print((_log_dir.length() > 0) ? "/" : "");
    Serial.print(_log_dir);
    Serial.print("/");
    Serial.print(_log_fn);
    Serial.print("' to append to");
    Serial.println();
  } else {
    Serial.print("LOG: appending to log file '");
    Serial.print((_log_dir.length() > 0) ? "/" : "");
    Serial.print(_log_dir);
    Serial.print("/");
    Serial.print(_log_fn);
    Serial.print("'");
    Serial.println();
  }

  _last_sync = millis();
  writeHeader();

  logging_started = millis();
#if _DEBUG && _XDEBUG_
  Serial.print(GPS.getTimestamp());
  Serial.print(": ");
  Serial.print("Logging started");
  Serial.println();
#endif

  _logging = true;
}

/************************************************************************************************************************************************************
             dP                      a88888b.                     dP
             88                     d8'   `88                     88
  .d8888b. d8888P .d8888b. 88d888b. 88        .d8888b. 88d888b. d8888P dP    dP 88d888b. .d8888b.
  Y8ooooo.   88   88'  `88 88'  `88 88        88'  `88 88'  `88   88   88    88 88'  `88 88ooood8
        88   88   88.  .88 88.  .88 Y8.   .88 88.  .88 88.  .88   88   88.  .88 88       88.  ...
  `88888P'   dP   `88888P' 88Y888P'  Y88888P' `88888P8 88Y888P'   dP   `88888P' dP       `88888P'
                           88                          88
                           dP                          dP
*/
void TrLOG::stopCapture() {
  _logging = false;
  myLog.println("## END OF LOG");

  if (!myLog.syncFile()) {
#if _DEBUG_
    Serial.print("LOG: Failed to sync file");
    Serial.println();
#if _XDEBUG_
  } else {
    Serial.print("LOG: file sync complete");
    Serial.println();
#endif
#endif
  }

  long log_size = myLog.size(_log_fn);
#if _DEBUG_
  Serial.print(RTC.getTimestamp());
  Serial.print(": ");
  Serial.print("LOG::stopCapture() - file '");
  Serial.print((_log_dir.length()) ? "/" : "");
  Serial.print(_log_dir);
  Serial.print("/");
  Serial.print(_log_fn);
  Serial.print("' is ");
  double fs = log_size;
  String sm = "KB";
  fs = fs / 1024.;
  if (fs >= 1000) {
    fs = fs / 1024.;
    sm = "MB";
  }
  Serial.print(fs);
  Serial.print(" ");
  Serial.print(sm);
  Serial.println();
#endif

  //  tidy();

  if (GPS.isEnabled() && GPS.isConnected()) {
    _final_latitude = lat_raw; //GPS.getLatitude();
    _final_longitude = lng_raw; //GPS.getLongitude();
    // TODO: start broadcasting this
    //_final_ground_distance = max(gpsDistance(_final_latitude, _final_longitude, _start_latitude, _start_longitude), _furthest_ground_distance);
    //    _final_ground_distance = gpsDistance(_start_latitude, _start_longitude, _final_latitude, _final_longitude);
  }

#if _DEBUG_
  if (IMU.isEnabled()) {
    Serial.print("       Peak Acceleration: ");
    Serial.print(_peak_acc / ONE_G);
    Serial.print (" g");
    Serial.println();
  } else {
    Serial.print("                  Peak G: ");
    Serial.print("[not calculated]");
    Serial.println();
  }
  //  if (EMU.isEnabled()) {
  //    Serial.print("       Peak EMU Altitude: ");
  //    Serial.print(_peak_emu_altitude);
  //    Serial.print (" m");
  //    Serial.println();
  //  } else {
  //    Serial.print("       Peak EMU Altitude: ");
  //    Serial.print("[not calculated]");
  //    Serial.println();
  //  }
  if (GPS.isEnabled() && GPS.isConnected()) {
    Serial.print("       Peak GPS Altitude: ");
    Serial.print(_peak_gps_altitude);
    Serial.print (" m");
    Serial.println();
    Serial.print("Furthest ground distance: ");
    Serial.print(_furthest_ground_distance);
    Serial.print (" m");
    Serial.println();
    Serial.print("   Final ground distance: ");
    Serial.print(_final_ground_distance);
    Serial.print (" m");
    Serial.println();
  } else {
    Serial.print("       Peak GPS Altitude: ");
    Serial.print("[not calculated]");
    Serial.println();
    Serial.print("Furthest ground distance: ");
    Serial.print("[not calculated]");
    Serial.println();
    Serial.print("   Final ground distance: ");
    Serial.print("[not calculated]");
    Serial.println();
  }
#endif
}

/************************************************************************************************************************************************************
                               dP
                               88
  .d8888b. .d8888b. 88d888b. d8888P dP    dP 88d888b. .d8888b.
  88'  `"" 88'  `88 88'  `88   88   88    88 88'  `88 88ooood8
  88.  ... 88.  .88 88.  .88   88   88.  .88 88       88.  ...
  `88888P' `88888P8 88Y888P'   dP   `88888P' dP       `88888P'
                    88
                    dP
*/
void TrLOG::capture(bool tf) {
  if (!isEnabled()) return;

  if (isCapturing() == tf) {
#if _XDEBUG_
    Serial.print("TrLOG::enable(");
    Serial.print(tf ? "true" : "false");
    Serial.print("): already in that state, ignoring call");
    Serial.println();
#endif
    return;
  }

#if _XDEBUG_
  Serial.print("TrLOG::enable(");
  Serial.print(tf ? "true" : "false");
  Serial.print("): called");
  Serial.println();
#endif

  if (tf) {
    startCapture();
  } else {
    stopCapture();
  }
}

/************************************************************************************************************************************************************
                      dP   dP                          .d88888b
                      88   88                          88.    "'
  .d8888b. .d8888b. d8888P 88        .d8888b. .d8888b. `Y88888b. dP    dP 88d8b.d8b. 88d8b.d8b. .d8888b. 88d888b. dP    dP
  88'  `88 88ooood8   88   88        88'  `88 88'  `88       `8b 88    88 88'`88'`88 88'`88'`88 88'  `88 88'  `88 88    88
  88.  .88 88.  ...   88   88        88.  .88 88.  .88 d8'   .8P 88.  .88 88  88  88 88  88  88 88.  .88 88       88.  .88
  `8888P88 `88888P'   dP   88888888P `88888P' `8888P88  Y88888P  `88888P' dP  dP  dP dP  dP  dP `88888P8 dP       `8888P88
       .88                                         .88                                                                 .88
    d8888P                                      d8888P                                                              d8888P
*/
String TrLOG::getLogSummary() {
  if (!isCapturing() && logging_started > 0) {
    String ret = "";
    ret += "Timestamp: ";
    ret += _log_ts;

    ret += "\n";
    ret += "Launch detected: ";
    ret += _launch_detected ? "Yes" : "No";

    ret += "\n";
    ret += "Parachute deployed: ";
    ret += _chute_deployed ? "Yes" : "No";

    if (_chute_deployed) {
      ret += "\n";
      ret += "Deployment: ";
      ret += _reason;
    }

    //    if (EMU.isEnabled()) {
    //      ret += "\n";
    //      ret += "Peak Altitude: ";
    //      ret += _peak_emu_altitude;
    //      ret += " m";
    //    }
    if (IMU.isEnabled()) {
      ret += "\n";
      ret += "Peak Acceleration: ";
      ret += _peak_acc / ONE_G;
      ret += " g";
    }
    if (_max_height > DUFF_VALUE) {
      ret += "\n";
      ret += "Apogee: ";
      ret += _max_height;
      ret += " m";
    }
    if (gps_distance != "") {
      ret += "\n";
      ret += "Flight distance: ";
      ret += gps_distance;
      ret += " m";
    }
    if (GPS.isEnabled()) {
      //      if (_peak_gps_altitude > DUFF_VALUE) {
      //        ret += "\n";
      //        ret += "Peak Altitude: ";
      //        ret += _peak_gps_altitude;
      //        ret += " m";
      //      }
      //      if (_final_ground_distance > DUFF_VALUE) {
      //        ret += "\n";
      //        ret += "Final distance: ";
      //        ret += _final_ground_distance;
      //        ret += " m";
      //      }
      if (_peak_speed > DUFF_VALUE) {
        ret += "\n";
        ret += "Peak Speed: ";
        ret += double(floor(_peak_speed * 10)) / 10.0;
        ret += " m/s (";
        ret += double(floor(_peak_speed * 2.237 * 10)) / 10.0;
        ret += " mph)";
      }
    }
    return ret;
  }

  return "";
}

/************************************************************************************************************************************************************
                                        dP                                dP
                                        88                                88
  .d8888b. .d8888b. 88d888b. .d8888b. d8888P 88d888b. dP    dP .d8888b. d8888P
  88'  `"" 88'  `88 88'  `88 Y8ooooo.   88   88'  `88 88    88 88'  `""   88
  88.  ... 88.  .88 88    88       88   88   88       88.  .88 88.  ...   88
  `88888P' `88888P' dP    dP `88888P'   dP   dP       `88888P' `88888P'   dP
*/
TrLOG::TrLOG() {};




//void deleteEmpty(String dir) {
//  String fileName = myLog.getNextDirectoryItem(); //getNextDirectoryItem() will return "" when we've hit the end of the directory
//  while (fileName != "") {
//    //Get size of file
//    long sizeOfFile = myLog.size(fileName);
//
//    if (sizeOfFile == -1) {
//      Serial.print("LOG::tidy() - file '");
//      Serial.print(dir);
//      Serial.print("/");
//      Serial.print(fileName);
//      Serial.print("' cannot be found???");
//      Serial.println();
//    } else if (sizeOfFile == 0) {
//      if (myLog.removeFile(fileName) == 1) {
//        Serial.print("LOG::tidy() - deleted empty file '");
//        Serial.print(dir);
//        Serial.print("/");
//        Serial.print(fileName);
//        Serial.print("'");
//        Serial.println();
//      } else {
//        Serial.print("LOG::tidy() - failed to delete empty file '");
//        Serial.print(dir);
//        Serial.print("/");
//        Serial.print(fileName);
//        Serial.print("'");
//        Serial.println();
//      }
//    } else {
//      Serial.print("LOG::tidy() - file '");
//      Serial.print(dir);
//      Serial.print("/");
//      Serial.print(fileName);
//      Serial.print("' is ");
//      Serial.print(double(sizeOfFile) / (1024.), 3);
//      Serial.print (" KB");
//      Serial.println();
//    }
//
//    fileName = myLog.getNextDirectoryItem();
//  }
//
//void deleteEmptyFiles(String dir) {
//  String fileName = myLog.getNextDirectoryItem(); //getNextDirectoryItem() will return "" when we've hit the end of the directory
//  while (fileName != "") {
//    //Get size of file
//    long sizeOfFile = myLog.size(fileName);
//
//    if (sizeOfFile == -1) {
//      Serial.print("LOG::tidy() - file '");
//      Serial.print(dir);
//      Serial.print("/");
//      Serial.print(fileName);
//      Serial.print("' cannot be found???");
//      Serial.println();
//    } else {
//      Serial.print("LOG::tidy() - file '");
//      Serial.print(dir);
//      Serial.print("/");
//      Serial.print(fileName);
//      Serial.print("' is ");
//      Serial.print(double(sizeOfFile) / (1024.), 3);
//      Serial.print (" KB");
//      if (sizeOfFile == 0) {
//        Serial.print (" - deleting");
//
//      }
//      Serial.println();
//    }
//
//    fileName = myLog.getNextDirectoryItem();
//    Serial.print("LOG::tidy() - next file is '");
//    Serial.print(fileName);
//    Serial.print ("'");
//    Serial.println();
//  }
//}
//void TrLOG::tidy() {
//  //myLog.searchDirectory("*"); //Give me everything
//  //myLog.searchDirectory("*.txt"); //Give me all the txt files in the directory
//  //myLog.searchDirectory("*/"); //Get just directories
//  //myLog.searchDirectory("*.*"); //Get just files
//  //myLog.searchDirectory("LOG*.TXT"); //Give me a list of just the logs
//  //myLog.searchDirectory("LOG000*.TXT"); //Get just the logs LOG00000 to LOG00099 if they exist.
//
//  Serial.print("LOG::tidy() - Scanning root folder for empty files");
//  Serial.println();
//  myLog.changeDirectory("..");
//
//  String dir = "";
//  myLog.searchDirectory("*.*");
//  String fileName = myLog.getNextDirectoryItem();
//  while (fileName != "") //getNextDirectoryItem() will return "" when we've hit the end of the directory
//  {
//    //Get size of file
////    long sizeOfFile = myLog.size(fileName);
//
//
//      Serial.print("LOG::tidy() - file '");
//      Serial.print(dir);
//      Serial.print("/");
//      Serial.print(fileName);
//      Serial.print("'");
//      Serial.println();
//
//
////    if (sizeOfFile == -1) {
////      Serial.print("LOG::tidy() - file '");
////      Serial.print(dir);
////      Serial.print("/");
////      Serial.print(fileName);
////      Serial.print("' cannot be found???");
////      Serial.println();
////    } else {
////      Serial.print("LOG::tidy() - file '");
////      Serial.print(dir);
////      Serial.print("/");
////      Serial.print(fileName);
////      Serial.print("' is ");
////      Serial.print(double(sizeOfFile) / (1024.), 3);
////      Serial.print (" KB");
////      if (sizeOfFile == 0) {
////        Serial.print (" - deleting");
////
////      }
////      Serial.println();
////    }
//
////    fileName = myLog.getNextDirectoryItem();
////    Serial.print("LOG::tidy() - next file is '");
////    Serial.print(fileName);
////    Serial.print ("'");
////    Serial.println();
//
//
//
//
//    Serial.println(fileName);
//    fileName = myLog.getNextDirectoryItem();
//  }
//
////  deleteEmptyFiles("");
//
//  //  myLog.searchDirectory("*.TXT");
//  //  deleteEmpty("");
//
////  if (_log_dir.length() > 0) {
////    Serial.print("LOG::tidy() - Scanning log folder for empty files");
////    Serial.println();
////    myLog.changeDirectory("..");
////    myLog.changeDirectory(_log_dir);
////
////    myLog.searchDirectory("*.*");
////    deleteEmptyFiles(String("/") + _log_dir);
////
////    //    myLog.searchDirectory("*.csv");
////    //    deleteEmpty(String("/") + _log_dir);
////  }
//  Serial.print("LOG::tidy() - process complete");
//}
