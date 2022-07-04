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
    int getSatsInView();

    int getRefreshRate() {
      return _refresh_hz;
    };

    boolean isEnabled() {
      return _enabled;
    };

    boolean isConnected() {
      return _connected;
    };

    void callbackDataRecieved();

  private:
    boolean _enabled = false;
    boolean _connected = false;

    double _lat = 0;
    double _lng = 0;
    double _alt = 0;
    int _siv; // Sats in view
    String _timestamp = "";
    int _refresh_hz;
};

extern TrGPS GPS;

#endif
