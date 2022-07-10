#ifndef _LOG_h
#define _LOG_h

#include "App.h"

class TrLOG {
  public:
    TrLOG();

    void begin();
    void loop();

    String getLogSummary();

    void capture(bool tf); // start or stop the logging
    bool isCapturing() {
      return _logging;
    };
    bool isEnabled() {
      return _enabled;
    };

  protected:
    void resetCapture();
    void startCapture();
    void stopCapture();
    void syncLog();
    void tidy();

  private:
    long logging_started = 0;
    bool _enabled = false;
    bool _logging = false;

    bool _chute_deployed;
    //    String _log;
    String _log_fn;
    String _log_dir;
    String _log_ts;
    unsigned long _last_sync;
    const unsigned long _sync_interval = 5000;

    double _peak_g;
    double _peak_bmp_altitude;
    double _peak_gps_altitude;
    double _start_latitude;
    double _start_longitude;
    double _ground_distance;
    double _furthest_ground_distance;
    double _final_latitude;
    double _final_longitude;
    double _final_ground_distance;
};

extern TrLOG LOG;

#endif
