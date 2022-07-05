#ifndef _APP_h
#define _APP_h

// If you only ever want this device to appear on your wifi, hard code those details here. (Since it's also written to config, it'll bypass the AP mode setup on a new device)
//#define WIFI_SSID "BT-WTA5HW"
//#define WIFI_PASS "LR43beLGftNaAH"
//#define WIFI_SSID "NJJIHotSpot"
//#define WIFI_PASS "V0dafone"
//#include "myWIFI.h"
#define WIFI_WAIT 30

// In AP mode (When the Wifi will not connect) this is the password - it needs to be at least 8 chars long for most devices to let you enter the details
#define _AP_NAME_ "WRFCOM"
#define _AP_PASSWORD_ "12345678"

#define VERSION "v0.1a"
#define _USE_NTP_ true
#define _NTP_OFFSET_SECONDS_ 0
#define _DEBUG_ true
#define _XDEBUG_ false

#define PARACHUTE_DEPLOY_APOGEE_OFFSET -2
#define SENSOR_HZ 14
//#define MAX_LOG_DURATION_SECONDS (5*60)
#define MAX_LOG_DURATION_SECONDS (15)

#ifndef ESP32
#include <Arduino.h> // For type inclusion - String for example
#endif
#include "BAT.h"
#include "BMP.h"
#include "GPS.h"
#include "IMU.h"
#include "LOG.h"
#include "NET.h"
#include "RTC.h"
#include "SRV.h"
#include "WEB.h"

#endif
