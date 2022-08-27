#ifndef _GPS_h
#define _GPS_h

#include "App.h"

class TrGPS {
  public:
    TrGPS();

    void begin();
    void loop();
    String getTimestamp();
    double getLatitude(); // Degrees Noth
    double getLongitude(); // Degrees East
    double getAltitude(); // meters above MSL
    double getGroundSpeed(); // metres per second
    double getTravelSpeed(); // meters per second
    double getTravelBearing(); // degrees from latitudeal north 0-359.9999999
    double getTravelElevation(); // degrees from upright in the plane of travel (bearing) - the x-axis. positive it top away, negative is top toward you


    int getSatsInView();

    int getRefreshRate() {
      return _refresh_hz;
    };

    bool isEnabled() {
      return _enabled;
    };
    
    bool hasMoved() {
      return _moved;
    };

    bool isConnected() {
      return _connected;
    };

    void callbackDataRecieved();

  protected:
    void processData();

  private:
    bool _enabled = false;
    bool _connected = false;

    double _lat = 0;
    double _lng = 0;
    double _alt = 0;
    bool _moved = false;
    double _ground_speed = 0;
    double _travel_speed = 0;
    double _travel_bearing = 0;
    double _travel_elevation = 0;
    int _siv; // Sats in view
    String _timestamp = "";
    int _refresh_hz;
    unsigned long _lock_millis = 0;
    unsigned long _confirmed_lock_millis = 1000;
};

extern TrGPS GPS;

#endif
