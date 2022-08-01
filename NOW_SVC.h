#ifndef __NOW_SERVICE_DEFINITIONS__
#define __NOW_SERVICE_DEFINITIONS__

typedef struct now_status_packet {
  char device_name[24];

  // Flags
  bool has_bms = false;
  bool has_gps = false;
  bool has_rtc = false;
  bool has_emu = false;
  bool has_imu = false;
  bool has_srv = false;
  bool has_temp = false;
  bool has_pressure = false;
  bool has_acc = false;
  bool has_gyro = false;
  bool has_mag = false;

  bool is_gps_locked = false;
  bool is_arm = false;
  bool is_log = false;
  double battery = 0;

  // GPS
  int siv = 0;
  double latitude = 0;
  double longitude = 0;
  double altitude = 0;

  // EMU
  double temp = 0;
  double pressure = 0;

  // IMU
  double acc_x = 0;
  double acc_y = 0;
  double acc_z = 0;

  double gyro_x = 0;
  double gyro_y = 0;
  double gyro_z = 0;

  double mag_x = 0;
  double mag_y = 0;
  double mag_z = 0;

} now_status_packet;

typedef struct now_command_packet {
  char device_name[24];

  unsigned long cmd = 0x00; // 0x01 is arm, 0x02 is log
  unsigned long val = 0x00;

} now_command_packet;

#endif
