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

    double getParachuteDeployApogeeOffset() {return _deploy_apogee_offset;};
    double getParachuteDeployDistanceOffset() {return _deploy_distance_offset;};
    double getLaunchDetectSpeed() {return _launch_detect_speed;};

    void setParachuteDeployApogeeOffset(double m) {_deploy_apogee_offset = m;};
    void setParachuteDeployDistanceOffset(double m) {_deploy_distance_offset = m;};
    void setLaunchDetectSpeed(double mps) {_launch_detect_speed = mps;};

  protected:
    void resetData();
    void getData();
    void writeHeader();
    void writeData();

    //    void resetCapture();
    void startCapture();
    void stopCapture();
    void syncLog();
    void tidy();

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

    //    String _reason;
    //    bool _chute_deployed;
    //    bool _in_flight;
    //    double _peak_speed;
    //    double _peak_g;
    //    double _peak_emu_altitude;
    //    double _peak_gps_altitude;
    //    double _start_latitude;
    //    double _start_longitude;
    //    double _ground_distance;
    //    double _furthest_ground_distance;
    //    double _final_latitude;
    //    double _final_longitude;
    //    double _final_ground_distance;
};

extern TrLOG LOG;

#endif
