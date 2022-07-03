#include "Logger.h"

TrLogger Logger;

#define PI 3.14159265358979323846
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

TrLogger::TrLogger() {};

void TrLogger::resetCapture() {
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

void TrLogger::begin() {
}

void TrLogger::loop() {
  if (!isEnabled()) return;

  if ((millis() - logging_started) > (1000 * MAX_LOG_DURATION_SECONDS)) {
#if _XDEBUG_
    Serial.print("TrLogger::loop(): maximum log period exceeded");
    Serial.println();
#endif
    enable(false);
    return;
  }

  String line = "";
  String comma = ",";
  line += String(millis() - logging_started);
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

  if (GPS.isEnabled() && GPS.isConnected()) {
    line += comma + String(GPS.getSatsInView());
    line += comma + String(GPS.getLatitude());
    line += comma + String(GPS.getLongitude());
    line += comma + String(GPS.getAltitude());

    _furthest_ground_distance = max(gpsDistance(GPS.getLatitude(), GPS.getLongitude(), _start_latitude, _start_longitude), _furthest_ground_distance);
    _peak_gps_altitude = max(GPS.getAltitude(), _peak_gps_altitude);
  } else {
    if (GPS.isEnabled()) { // It's plugged in, so maybe not locked yet
      line += comma + String(GPS.getSatsInView());
    } else {
      line += comma;

    }
    line += ",,,";
  }
  line += comma + _chute_deployed ? "Yes" : "No";

  _log += String("\n") + line;

}

void TrLogger::enable(boolean tf) {
#if _XDEBUG_
  Serial.print("TrLogger::enable(");
  Serial.print(tf ? "true" : "false");
  Serial.print("): called");
  Serial.println();
#endif
  if (_enabled == tf) {
#if _XDEBUG_
    Serial.print("TrLogger::enable(");
    Serial.print(tf ? "true" : "false");
    Serial.print("): already in that state, ignoring call");
    Serial.println();
#endif
    return;
  }
  if (tf) {
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
      fn = Network.getTimestamp();
    }

    if (fn.length() == 0) {
      fn = millis();
    } else {
      header = fn;
      _log_ts = fn;

    }

    fn.replace("-", "");
    fn.replace(":", "");
    fn.replace("Z", "");
    fn = fn.substring(9);
    fn += ".csv";
    _log_fn = fn;

    // Create a new file on the logger
    // output header line
    header += "millis, BME Temp, BMP MSL BMP, BMP BMP, BME Altitude, IMU Temp, IMU AccX, IMU AccY, IMU AccZ, IMU GyroX, IMU GyroY, IMU GyroZ, GPS Sats, GPS Lat, GPS Lon, GPS Alt, Chute";

    _log = header;

    logging_started = millis();
#if _XDEBUG_
    Serial.print(RTC.getTimestamp());
    Serial.print(": ");
    Serial.print("Logging started");
    Serial.println();
#endif
    _enabled = tf;
  } else {
    _enabled = tf;
#if _DEBUG_
    Serial.print(RTC.getTimestamp());
    Serial.print(": ");
    Serial.print("Logging completed ");
    Serial.print(_log.length());
    Serial.print(" bytes captured to ");
    Serial.print(_log_fn);
    Serial.println();
#endif
    // close file on logger

    if (GPS.isEnabled() && GPS.isConnected()) {
      _final_latitude = GPS.getLatitude();
      _final_longitude = GPS.getLongitude();
      _final_ground_distance = max(gpsDistance(_final_latitude, _final_longitude, _start_latitude, _start_longitude), _furthest_ground_distance);
    }

    if (IMU.isEnabled()) {
      //      Serial.print("                  Peak G: ");
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
  }
}

String TrLogger::getLogSummary() {
  //  Serial.println("TrLogger::getLogSummary(): called");
  //    Serial.print("                         : enabled: ");
  //    Serial.print(isEnabled()?"true":"false");
  //    Serial.print(", Size: ");
  //    Serial.print(_log.length());
  //    Serial.print(" bytes");
  //    Serial.println();

  if (!isEnabled() && _log.length() > 0) {
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
