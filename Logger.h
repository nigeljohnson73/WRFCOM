#ifndef _Logger_h
#define _Logger_h

#include "App.h"

class TrLogger {
  public:
    TrLogger();

    void begin();
    void loop();

    String getLogSummary();

    void enable(boolean tf); // start or stop the logging
    boolean isEnabled() {
      return _enabled;
    };

  protected:
    void resetCapture();

  private:
    long logging_started = 0;
    boolean _enabled = false;

    boolean _chute_deployed;
    String _log;
    String _log_fn;
    String _log_ts;

    double _peak_g;
    double _peak_bmp_altitude;
    double _peak_gps_altitude;
    double _start_latitude;
    double _start_longitude;
    double _furthest_ground_distance;
    double _final_latitude;
    double _final_longitude;
    double _final_ground_distance;
};

extern TrLogger Logger;

#endif
