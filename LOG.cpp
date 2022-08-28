#include "LOG.h"

#include "SparkFun_Qwiic_OpenLog_Arduino_Library.h"
OpenLog myLog;

TrLOG LOG;

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
static String tvl_distance;
static String tvl_speed;
static String gps_bearing;
static String gps_elevation;
static bool _launch_detected;
static bool _in_flight;
static bool _chute_deployed;
static String _reason;
static bool _landing_detected;

static bool _unprepared_launch;
static unsigned long _launch_millis;
static double _launch_altitude;
static double _launch_latitude;
static double _launch_longitude;
static double _max_speed;
static double _max_acceleration;
static double _max_height;
static double _flight_time;
static double _flight_distance;

static double lat_raw;
static double lng_raw;
static double alt_raw;
static double spd_raw;
static double ele_raw;
static double brg_raw;
static double _final_latitude;
static double _final_longitude;
static double _peak_gps_altitude; // Used for Apogee detect

/************************************************************************************************************************************************************
                      oo   dP            dP     dP                          dP
                           88            88     88                          88
  dP  dP  dP 88d888b. dP d8888P .d8888b. 88aaaaa88a .d8888b. .d8888b. .d888b88 .d8888b. 88d888b.
  88  88  88 88'  `88 88   88   88ooood8 88     88  88ooood8 88'  `88 88'  `88 88ooood8 88'  `88
  88.88b.88' 88       88   88   88.  ... 88     88  88.  ... 88.  .88 88.  .88 88.  ... 88
  8888P Y8P  dP       dP   dP   `88888P' dP     dP  `88888P' `88888P8 `88888P8 `88888P' dP
*/
void TrLOG::writeHeader() {
  String line = GPS.getTimestamp() + "\n";
  if (_unprepared_launch) {
    line += "## UNPREPARED LAUNCH";
    line += "\n";
  }
  line += "millis";
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
  line += comma + "gps_bearing";
  line += comma + "gps_elevation";
  line += comma + "tvl_speed";
  line += comma + "tvl_distance";

  line += comma + "_launch_detected";
  line += comma + "_in_flight";
  line += comma + "_chute_deployed";
  line += comma + "_reason";
  line += comma + "_landing_detected";
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
  line += comma + gps_bearing;
  line += comma + gps_elevation;
  line += comma + tvl_speed;
  line += comma + tvl_distance;

  line += comma + (_launch_detected ? "Yes" : "No");
  line += comma + (_in_flight ? "Yes" : "No");
  line += comma + (_chute_deployed ? "Yes" : "No");
  line += comma + _reason;
  line += comma + (_landing_detected ? "Yes" : "No");

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
  tvl_distance = "";
  tvl_speed = "";
  gps_bearing = "";
  gps_elevation = "";

  _launch_detected = false;
  _in_flight = false;
  _chute_deployed = false;
  _reason = "";
  _landing_detected = false;

  _unprepared_launch = false;
  _launch_millis = 0;
  _launch_latitude = DUFF_VALUE;
  _launch_longitude = DUFF_VALUE;
  _launch_altitude = DUFF_VALUE;
  _flight_time = DUFF_VALUE;
  _flight_distance = DUFF_VALUE;
  _max_speed = DUFF_VALUE;
  _max_height = DUFF_VALUE;
  _max_acceleration = DUFF_VALUE;
  _final_latitude = DUFF_VALUE;
  _final_longitude = DUFF_VALUE;
  _peak_gps_altitude = DUFF_VALUE;
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
      emu_pressure_msl = EMU.getLocalSeaLevelPressure();
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
      _max_acceleration = max(acc, _max_acceleration);
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
    if (GPS.isConnected()) {
      lat_raw = GPS.getLatitude();
      lng_raw = GPS.getLongitude();
      alt_raw = GPS.getAltitude();
      brg_raw = GPS.getTravelBearing();
      ele_raw = GPS.getTravelElevation();
      spd_raw = GPS.getTravelSpeed();
    }
  }
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
void TrLOG::processData() {
  static unsigned long last_track;
  static double last_lat = DUFF_VALUE;
  static double last_lng = DUFF_VALUE;
  static double last_alt = DUFF_VALUE;

  // Blank the values so they are lot logged - we specifically have to calulate them this frame
  gps_latitude = "";
  gps_longitude = "";
  gps_altitude = "";
  tvl_speed = "";
  gps_bearing = "";
  gps_elevation = "";
  tvl_distance = "";
  _in_flight = false;

  if (GPS.isEnabled() && GPS.isConnected()) {

    unsigned long track = millis();
    // First ever GPS reading after it connected... Skip any further processing
    if (last_track == 0) {
      last_track = track;
      return;
    }

    // Only the second ever GPS reading, capture it to set up the GPS deltas in the next frame, but do no comparison now
    if (spd_raw < 0 || spd_raw > 500) {
      last_track = track;
      last_lat = lat_raw;
      last_lng = lng_raw;
      last_alt = alt_raw;
      return;
    }

    gps_latitude = String(lat_raw, 7);
    gps_longitude = String(lng_raw, 7);
    gps_altitude = alt_raw;

    // Calculate all the detlas from the last cycle
    if (GPS.hasMoved()) {
      // We have moved since the last recorded value
      gps_elevation = ele_raw;
      gps_bearing = brg_raw;
      tvl_speed = spd_raw;
      _max_speed = max(spd_raw, _max_speed);

      // If we are ready and expecting a launch, and we have enough speped to be launching... start that process
      if (SRV.isArmed() && LOG.isCapturing() && !_launch_detected && spd_raw >= _launch_detect_speed) {
        const char* str = "## LAUNCH DETECTED";
        myLog.println(str);
#if _DEBUG_
        Serial.println(str);
#endif
        _launch_detected = true;
      }

      // If we have a lot of speed, have the parachute primed, but aren't in launch ready... assume the idiot forgot to press the button and cover their butt.
      if (SRV.isArmed() && !LOG.isCapturing() && spd_raw >= _emergency_launch_detect_speed) {
        startCapture();
        const char* str = "## EMERGENCY CAPTURE FORCE START";
        myLog.println(str);
#if _DEBUG_
        Serial.println(str);
#endif
        _launch_detected = true;
        _unprepared_launch = true;
        // Cuz we are emergencying, the launch parameters will never have been updated, so anything depending on them will be crap
        _launch_latitude = lat_raw;
        _launch_longitude = lng_raw;
        _launch_altitude = alt_raw;
        _peak_gps_altitude = alt_raw;
      }

      // Set the last track values
      last_track = track;
      last_lat = lat_raw;
      last_lng = lng_raw;
      last_alt = alt_raw;
    }

    if (_launch_detected) {
      _peak_gps_altitude = max(alt_raw, _peak_gps_altitude);

      // First check to see if we have landed
      if (!_landing_detected && spd_raw < _landing_detect_speed) {
        const char* str = "## LANDIND DETECTED";
        myLog.println(str);
#if _DEBUG_
        Serial.println(str);
#endif
        _landing_detected = true;
      }

      // Calulate the current distance from the launch position, and if we are still flying update the log strings
      double dst = gpsDistance(_launch_latitude, _launch_longitude, lat_raw, lng_raw);
      tvl_distance = dst;
      if (!_landing_detected) {
        _flight_distance = max(dst, _flight_distance);
        _flight_time = track - _launch_millis;
        _max_height = max(alt_raw - _launch_altitude, _max_height);
      }

      if (_deploy_apogee_offset != 0 && SRV.isArmed() && LOG.isCapturing() && !_chute_deployed && alt_raw <= (_peak_gps_altitude - _deploy_apogee_offset)) {
        const char* str = "## PARACHUTE DEPLOYED (APOGEE)";
        myLog.println(str);
#if _DEBUG_
        Serial.println(str);
#endif
        _chute_deployed = true;
        _reason = "GPS Apogee";
        SRV.arm(false);
      }

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
    } else {
      //Not yet launched - keep up updating until we do
      _launch_latitude = lat_raw;
      _launch_longitude = lng_raw;
      _launch_altitude = alt_raw;
      _peak_gps_altitude = alt_raw;
    }

    // Being inflight is the bit between launching and landing
    _in_flight = ! _landing_detected;

  } else {
    // GPS is not connected
    last_track = 0;
    //    last_lat = DUFF_VALUE;
    //    last_lng = DUFF_VALUE;
    //    last_alt = DUFF_VALUE;
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
  //  if (speed > _emergency_launch_detect_speed) {
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
    Serial.println("LOG init: disconnected");
#endif
    return;
  }
  String ver = myLog.getVersion();
  if (ver == "255.255") {
#if _DEBUG_
    //Serial.println("LOG disconnected (OpenLog getVersion() failure)");
    Serial.println("LOG init: disconnected");
#endif
    return;
  }

#if _DEBUG_
  Serial.print("LOG init: version ");
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
  processData();

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
  // Synchronise the hardware from the GPS device
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

  // Reset all the internal values
  resetData();

  String fn = getTimestamp();
  // Fall back to millis, but should not be here operationally
  if (fn.length() == 0) {
    fn = millis();
    _log_dir = "";
  } else {
    // Store the  for processing later
    _log_ts = fn;

    // Remove all the crap so that the date and time is only 8 and 6 chars long respectively
    fn.replace("-", "");
    fn.replace(":", "");
    fn.replace("Z", "");

    _log_dir = fn.substring(0, 8);
    fn = fn.substring(9);
  }

  fn += ".csv";
  _log_fn = fn;

  myLog.changeDirectory(".."); // Make sure we are in the root (Don't care if this fails, cuz we are in the root in that case)
  myLog.makeDirectory(_log_dir);
  myLog.changeDirectory(_log_dir);
  myLog.append(_log_fn);
  logging_started = millis();
  _last_sync = logging_started;

  writeHeader();

#if _DEBUG && _XDEBUG_
  Serial.print(GPS.getTimestamp());
  Serial.print(": ");
  Serial.print("Logging started");
  Serial.println();
#endif

  _logging = true;
}

/************************************************************************************************************************************************************
                      dP   d888888P oo                                dP
                      88      88                                      88
  .d8888b. .d8888b. d8888P    88    dP 88d8b.d8b. .d8888b. .d8888b. d8888P .d8888b. 88d8b.d8b. 88d888b.
  88'  `88 88ooood8   88      88    88 88'`88'`88 88ooood8 Y8ooooo.   88   88'  `88 88'`88'`88 88'  `88
  88.  .88 88.  ...   88      88    88 88  88  88 88.  ...       88   88   88.  .88 88  88  88 88.  .88
  `8888P88 `88888P'   dP      dP    dP dP  dP  dP `88888P' `88888P'   dP   `88888P8 dP  dP  dP 88Y888P'
       .88                                                                                     88
   d8888P                                                                                      dP
*/
String TrLOG::getTimestamp() {
  String ts = "";

  // get GPS, RTC time or network time in that order to generate a file name
  if (GPS.isEnabled() && GPS.isConnected()) {
    ts = GPS.getTimestamp();
  } else if (RTC.isEnabled()) {
    ts = RTC.getTimestamp();
  } else {
    ts = NET.getTimestamp();
  }

  return ts;
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
  myLog.flush();
  myLog.println("## END OF LOG");
  myLog.syncFile();

  long log_size = myLog.size(_log_fn);
#if _DEBUG_
  Serial.print(LOG.getTimestamp());
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
    _final_latitude = lat_raw;
    _final_longitude = lng_raw;
    // TODO: start broadcasting this
  }

  //#if _DEBUG_
  //  Serial.print("    Max Acceleration: ");
  //  if (IMU.isEnabled()) {
  //    Serial.print(_max_acceleration / ONE_G);
  //    Serial.print (" g");
  //  } else {
  //    Serial.print("[not calculated]");
  //  }
  //  Serial.println();
  //
  //  Serial.print("           Max Speed: ");
  //  if (IMU.isEnabled()) {
  //    Serial.print(_max_speed);
  //    Serial.print (" m/s (");
  //    Serial.print(_max_speed * 2.237);
  //    Serial.print (" mph)");
  //  } else {
  //    Serial.print("[not calculated]");
  //  }
  //  Serial.println();
  //
  //  Serial.print("          Max Height: ");
  //  if (_max_height > DUFF_VALUE) {
  //    Serial.print(_max_height);
  //    Serial.print (" m");
  //  } else {
  //    Serial.print("[not calculated]");
  //  }
  //  Serial.println();
  //
  //  Serial.print("         Flight Time: ");
  //  if (_flight_time > DUFF_VALUE) {
  //    Serial.print(_flight_time);
  //    Serial.print (" s");
  //  } else {
  //    Serial.print("[not calculated]");
  //  }
  //  Serial.println();
  //
  //  Serial.print("     Flight Distance: ");
  //  if (_flight_distance > DUFF_VALUE) {
  //    Serial.print(_flight_distance);
  //    Serial.print (" s");
  //  } else {
  //    Serial.print("[not calculated]");
  //  }
  //  Serial.println();
  //#endif

#if _DEBUG_
  Serial.print(getLogSummary());
  Serial.println();
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

    if (_final_latitude > DUFF_VALUE) {
      ret += "Location: ";
      ret += String(_final_latitude, 7);
      ret += ", ";
      ret += String(_final_longitude, 7);
      ret += "\n";
    }

    ret += "Launch detected: ";
    ret += _launch_detected ? "Yes" : "No";
    if (_unprepared_launch) {
      ret += " (unexpected)";
    }
    ret += "\n";

    if (_launch_detected) {
      ret += "Parachute deployed: ";
      ret += _chute_deployed ? "Yes" : "No";
      ret += "\n";

      if (_chute_deployed) {
        ret += "Deployment: ";
        ret += _reason;
        ret += "\n";
      }

      ret += "Landing detected: ";
      ret += _landing_detected ? "Yes" : "No";
      ret += "\n";
    }

    if (_landing_detected) {
      if (_flight_time > DUFF_VALUE) {
        ret += "Flight time: ";
        ret += double(floor(_flight_time / 100.0)) / 10.0; // tenths of a second
        ret += " s";
        ret += "\n";
      }
      if (_flight_distance > DUFF_VALUE) {
        ret += "Flight distance: ";
        ret += _flight_distance;
        ret += " m";
        ret += "\n";
      }
    }
    if (_max_height > DUFF_VALUE) {
      ret += "Apogee: ";
      ret += _max_height;
      ret += " m";
      ret += "\n";
    }
    if (IMU.isEnabled()) {
      ret += "Peak Acceleration: ";
      ret += _max_acceleration / ONE_G;
      ret += " g";
      ret += "\n";
    }
    if (GPS.isEnabled()) {
      if (_max_speed > DUFF_VALUE) {
        ret += "Peak Speed: ";
        ret += double(floor(_max_speed * 10)) / 10.0;  // tenths of a meter/second
        ret += " m/s (";
        ret += double(floor(_max_speed * 2.237 * 10)) / 10.0; // tenths of a mile per hour
        ret += " mph)";
        ret += "\n";
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
