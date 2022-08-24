#ifndef _LOG_h
#define _LOG_h

#include "App.h"

class TrLOG {
  public:
    TrLOG();

    void begin();
    void loop();

    String getLogSummary();
    //    void detectLaunch();

    void capture(bool tf); // start or stop the logging
    bool isCapturing() {
      return _logging;
    };
    bool isEnabled() {
      return _enabled;
    };

    double getParachuteDeployApogeeOffset() {
      return _deploy_apogee_offset;
    };

    void setParachuteDeployApogeeOffset(double m) {
      _deploy_apogee_offset = m;
    };

    double getParachuteDeployDistanceOffset() {
      return _deploy_distance_offset;
    };

    void setParachuteDeployDistanceOffset(double m) {
      _deploy_distance_offset = m;
    };

    double getLaunchDetectSpeed() {
      return _launch_detect_speed;
    };

    void setLaunchDetectSpeed(double mps) {
      _launch_detect_speed = mps;
    };

  protected:
    void resetData();
    void getData();
    void writeHeader();
    void writeData();

    void startCapture();
    void stopCapture();
    void syncLog();
    //    void tidy();

  private:
    long logging_started = 0;
    bool _enabled = false;
    bool _logging = false;

    //    String _log;
    String _log_fn;
    String _log_dir;
    String _log_ts;
    unsigned long _last_sync;
    const unsigned long _sync_interval = 0;// 5000;

    double _deploy_apogee_offset = -2.0;
    double _deploy_distance_offset = 65.0;
    double _launch_detect_speed = 5;

};

extern TrLOG LOG;

#endif
