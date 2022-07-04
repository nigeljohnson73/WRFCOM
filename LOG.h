#ifndef _LOG_h
#define _LOG_h

#include "App.h"

class TrLOG {
  public:
    TrLOG();

    void begin();
    void loop();

    String getLogSummary();

    void capture(boolean tf); // start or stop the logging
    boolean isCapturing() {
      return _logging;
    };
    boolean isEnabled() {
      return _enabled;
    };

  protected:
    void resetCapture();
    void startCapture();
    void stopCapture();

  private:
    long logging_started = 0;
    boolean _enabled = false;
    boolean _logging = false;

    boolean _chute_deployed;
//    String _log;
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

extern TrLOG LOG;

#endif
