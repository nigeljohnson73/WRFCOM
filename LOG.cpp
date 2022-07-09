#include "LOG.h"

#if ESP32

#ifndef LED_PIN
#define LED_PIN 13
#endif

#else

#ifndef LED_PIN
#define LED_PIN LED_BUILTIN
#endif

#endif

#include "SparkFun_Qwiic_OpenLog_Arduino_Library.h"
OpenLog myLog;

TrLOG LOG;

#ifndef PI
#define PI 3.14159265358979323846
#endif

#define EARTH_RADIUS 6372797.56085
#define ONE_G 9.80665

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

TrLOG::TrLOG() {};

void TrLOG::begin() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  //  Wire.begin();
  if (!myLog.begin()) {
#if _DEBUG_
    //Serial.println("LOG disconnected (OpenLog begin() failure)");
    Serial.println("LOG disconnected");
#endif
    return;
  }
  String ver = myLog.getVersion();
  if (ver == "255.255") {
#if _DEBUG_
    //Serial.println("LOG disconnected (OpenLog getVersion() failure)");
    Serial.println("LOG disconnected");
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

void TrLOG::loop() {
  if (!isEnabled() || !isCapturing()) return;

  if ((millis() - logging_started) > (1000 * MAX_LOG_DURATION_SECONDS)) {
#if _XDEBUG_
    Serial.print("TrLOG::loop(): maximum log period exceeded");
    Serial.println();
#endif
    capture(false);
    return;
  }

  if ((millis() - _last_sync) >= _sync_interval) {
    syncLog();
  }

  String line = "";
  String comma = ",";
  line += String(millis() - logging_started);

  if (BAT.isEnabled()) {
    line += comma + String(BAT.getCapacityPercent());
    line += comma + String(BAT.getCapacityVoltage());
  } else {
    line += comma;
    line += comma;
  }


  if (BMP.isEnabled()) {
    double alt = BMP.getAltitude();
    line += comma + String(BMP.getTemperature());
    line += comma + String(BMP.getSeaLevelPressure());
    line += comma + String(BMP.getPressure());
    line += comma + String(alt);

    _peak_bmp_altitude = max(alt, _peak_bmp_altitude);
    if (alt < (_peak_bmp_altitude + PARACHUTE_DEPLOY_APOGEE_OFFSET)) {
      _chute_deployed = true;
      // TODO: trigger deployment
    }
  } else {
    line += ",,,,";
  }

  if (IMU.isEnabled()) {
    line += comma + String(IMU.getTemperature());
    line += comma + String(IMU.getAccelerationX());
    line += comma + String(IMU.getAccelerationY());
    line += comma + String(IMU.getAccelerationZ());
    line += comma + String(IMU.getGyroX());
    line += comma + String(IMU.getGyroY());
    line += comma + String(IMU.getGyroZ());

    double g_force = sqrt(pow(IMU.getAccelerationX(), 2) + pow(IMU.getAccelerationY(), 2) + pow(IMU.getAccelerationZ(), 2));
    _peak_g = max(g_force, _peak_g);
  } else {
    line += ",,,,,,,";
  }

  if (GPS.isEnabled()) {
    line += comma + String(GPS.getSatsInView());
    if (GPS.isConnected()) {
      line += comma + String(GPS.getLatitude(), 7);
      line += comma + String(GPS.getLongitude(), 7);
      line += comma + String(GPS.getAltitude(), 4);

      _furthest_ground_distance = max(gpsDistance(GPS.getLatitude(), GPS.getLongitude(), _start_latitude, _start_longitude), _furthest_ground_distance);
      _peak_gps_altitude = max(GPS.getAltitude(), _peak_gps_altitude);
    } else {
      line += ",,,";
    }
  } else {
    line += ",,,,";
  }
  line += comma + (_chute_deployed ? "Yes" : "No");

  //  _log += String("\n") + line;
  myLog.println(line);

}

void TrLOG::syncLog() {
  _last_sync = millis();
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
  fs = fs/1024.;
  if(fs >=1000) {
	  fs = fs/1024.;
	  sm = "MB";
  }
  Serial.print(fs);
  Serial.print(" ");
  Serial.print(sm);
  Serial.println();
}

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
//}
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
void TrLOG::tidy() {
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
}

void TrLOG::resetCapture() {
  _chute_deployed = false;
  _peak_g = 0;
  _peak_bmp_altitude = -99999;
  _peak_gps_altitude - -99999;
  _start_latitude = 0;
  _start_longitude = 0;
  _furthest_ground_distance = 0;
  _final_latitude = 0;
  _final_longitude = 0;
  _final_ground_distance = 0;

  if (GPS.isEnabled() && GPS.isConnected()) {
    if (RTC.isEnabled()) {
      // Set time from GPS;
    }
    if (BMP.isEnabled()) {
      BMP.setAltitude(GPS.getAltitude());
    }
  }
}

void TrLOG::startCapture() {
  resetCapture();

  // get GPS, RTC time or network time in that order
  String fn = "";
  String header = "";
  if (GPS.isEnabled() && GPS.isConnected()) {
    _start_latitude = GPS.getLatitude();
    _start_longitude = GPS.getLongitude();

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

  Serial.print("LOG::startCapture() - sorted file name: '");
  Serial.print((_log_dir.length() > 0) ? "/" : "");
  Serial.print(_log_dir);
  Serial.print("/");
  Serial.print(_log_fn);
  Serial.print("'");
  Serial.println();

  header += F("millis, BAT Pcnt, BAT Volts, BMP Temp, BMP MSL hPa, BMP hPa, BMP Altitude, IMU Temp, IMU AccX, IMU AccY, IMU AccZ, IMU GyroX, IMU GyroY, IMU GyroZ, GPS Sats, GPS Lat, GPS Lng, GPS Alt, Chute");
  //_log = header;

  Serial.print("LOG::startCapture() - Moving to the root directory");
  Serial.println();
  myLog.changeDirectory(".."); // Make sure we are in the root (Don't care if this fails)
  if (_log_dir.length() > 0) {
    // try to Change into it
    if (!myLog.changeDirectory(_log_dir)) {
      Serial.print("LOG::startCapture() - failed to change into log directory '/");
      Serial.print(_log_dir);
      Serial.print("' - attempting to create it");
      Serial.println();

      // Create the new directory
      if (!myLog.makeDirectory(_log_dir)) {
        Serial.print("LOG::startCapture() - failed to create log directory '/");
        Serial.print(_log_dir);
        Serial.print("', stay in the root then");
        Serial.println();
        _log_dir = "";
      } else {
        // Change into it
        if (!myLog.changeDirectory(_log_dir)) {
          Serial.print("LOG::startCapture() - failed to change into log directory '/");
          Serial.print(_log_dir);
          Serial.print("' again... stay in the root then");
          Serial.println();
          _log_dir = "";
        }
      }
    }
  }
  Serial.print("LOG: working log file is now '");
  Serial.print((_log_dir.length() > 0) ? "/" : "");
  Serial.print(_log_dir);
  Serial.print("/");
  Serial.print(_log_fn);
  Serial.print("'");
  Serial.println();

  // Now create our log file where we are.
  if (!myLog.append(_log_fn)) {
    Serial.print("LOG: Failed to create file '");
    Serial.print((_log_dir.length() > 0) ? "/" : "");
    Serial.print(_log_dir);
    Serial.print("/");
    Serial.print(_log_fn);
    Serial.print("' to append to");
    Serial.println();
  } else {
    Serial.print("LOG: created new log file '");
    Serial.print((_log_dir.length() > 0) ? "/" : "");
    Serial.print(_log_dir);
    Serial.print("/");
    Serial.print(_log_fn);
    Serial.print("'");
    Serial.println();
  }

  _last_sync = millis();
  if (!myLog.println(header)) {
#if _DEBUG_
    Serial.print("LOG::startCapture() failed to write header line");
    Serial.println();
#if _XDEBUG_
  } else {
    Serial.print("LOG::startCapture() - header line written");
    Serial.println();
#endif
#endif
  }

  logging_started = millis();
#if _DEBUG && _XDEBUG_
  Serial.print(RTC.getTimestamp());
  Serial.print(": ");
  Serial.print("Logging started");
  Serial.println();
#endif

  _logging = true;
  digitalWrite(LED_PIN, HIGH);

}

void TrLOG::stopCapture() {
  digitalWrite(LED_PIN, LOW);
  _logging = false;
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
  fs = fs/1024.;
  if(fs >=1000) {
	  fs = fs/1024.;
	  sm = "MB";
  }
  Serial.print(fs);
  Serial.print(" ");
  Serial.print(sm);
  Serial.println();
#endif

//  tidy();

  if (GPS.isEnabled() && GPS.isConnected()) {
    _final_latitude = GPS.getLatitude();
    _final_longitude = GPS.getLongitude();
    _final_ground_distance = max(gpsDistance(_final_latitude, _final_longitude, _start_latitude, _start_longitude), _furthest_ground_distance);
  }

#if _DEBUG_
  if (IMU.isEnabled()) {
    Serial.print("       Peak Aceeleration: ");
    Serial.print(_peak_g / ONE_G);
    Serial.print (" g");
    Serial.println();
  } else {
    Serial.print("                  Peak G: ");
    Serial.print("[not calculated]");
    Serial.println();
  }
  if (BMP.isEnabled()) {
    Serial.print("       Peak BMP Altitude: ");
    Serial.print(_peak_bmp_altitude);
    Serial.print (" m");
    Serial.println();
  } else {
    Serial.print("       Peak BMP Altitude: ");
    Serial.print("[not calculated]");
    Serial.println();
  }
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

String TrLOG::getLogSummary() {
  if (!isCapturing() && logging_started > 0) {
    String ret = "";
    ret += "Timestamp: ";
    ret += _log_ts;

    ret += "\n";
    ret += "Parachute deployed: ";
    ret += _chute_deployed ? "Yes" : "No";

    if (BMP.isEnabled()) {
      ret += "\n";
      ret += "Peak Altitude: ";
      ret += _peak_bmp_altitude;
      ret += " m";
    }
    if (IMU.isEnabled()) {
      ret += "\n";
      ret += "Peak Acceleration: ";
      ret += _peak_g / ONE_G;
      ret += " g";
    }
    if (GPS.isEnabled()) {
      ret += "\n";
      ret += "Final distance: ";
      ret += _final_ground_distance;
      ret += " m";
    }
    return ret;
  }

  return "";
}
