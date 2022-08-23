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
    //    double getLinearSpeed(); // metres per second
    //    double getGroundSpeed(); // metres per second

    int getSatsInView();

    int getRefreshRate() {
      return _refresh_hz;
    };

    bool isEnabled() {
      return _enabled;
    };

    bool isConnected() {
      return _connected;
    };

    void callbackDataRecieved();

  private:
    bool _enabled = false;
    bool _connected = false;

    double _lat = 0;
    double _lng = 0;
    double _alt = 0;
    //    double _speed = 0;
    //    double _g_speed = 0;
    int _siv; // Sats in view
    String _timestamp = "";
    int _refresh_hz;
};

extern TrGPS GPS;

#endif
