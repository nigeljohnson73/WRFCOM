#ifndef _APP_h
#define _APP_h

#define VERSION "v0.1a"

// Preload our config to override any of the following parameters
#include "myConfig.h"

// Defaulting this to blank will just skip into Access Point mode
#ifndef WIFI_SSID
#define WIFI_SSID ""
#endif

#ifndef WIFI_PASS
#define WIFI_PASS ""
#endif

// This is how long the wifi will look for an AP to connect to before launcing into it's own mess
#ifndef WIFI_WAIT
#define WIFI_WAIT 30
#endif

// In AP mode (When the Wifi will not connect) this is the password.
// it needs to be at least 8 chars long for most devices to let you enter the details
#ifndef _AP_NAME_
#define _AP_NAME_ "WRFCOM"
#endif

#ifndef _AP_PASSWORD_
#define _AP_PASSWORD_ "12345678"
#endif

// If you going to be connected to a network, then this is good starter. Use ZULU/GMT/UTC
#ifndef _USE_NTP_
#define _USE_NTP_ true
#endif

// Configure LiPO and monitor use
#ifndef USE_BMS
#define USE_BMS true
#endif

// Should we use a BLE
#ifndef USE_BLE
#define USE_BLE true
#endif

// Should we use a physical button
#ifndef USE_BUTTON
#define USE_BUTTON true
#endif

// Should the servo code be used for parachute deployment
#ifndef USE_SERVO
#define USE_SERVO true
#endif

#ifndef _NTP_OFFSET_SECONDS_
#define _NTP_OFFSET_SECONDS_ 0
#endif

// How long should we do a log before auto shutting it off
#ifndef MAX_LOG_DURATION_SECONDS
#define MAX_LOG_DURATION_SECONDS (5*60)
#endif

// Once the peak height has been reached, then at this point, throw out hte parachute
#ifndef PARACHUTE_DEPLOY_APOGEE_OFFSET
#define PARACHUTE_DEPLOY_APOGEE_OFFSET -2
#endif

#ifndef LIPO_SIZE
#define LIPO_SIZE 0x19
#endif
// Options: 
//  LC709203F_APA_100MAH = 0x08,
//  LC709203F_APA_200MAH = 0x0B,
//  LC709203F_APA_500MAH = 0x10,
//  LC709203F_APA_1000MAH = 0x19,
//  LC709203F_APA_2000MAH = 0x2D,
 // LC709203F_APA_3000MAH = 0x36,


// How fast should we poll the sensors
// The slowest is going to be the GPS at 18 Hz, but on the D1 mini, 15 will choke the IIC bus
#ifndef SENSOR_HZ
#define SENSOR_HZ 14
#endif

// How fast should we push the BLE notification
#ifndef BLE_HZ
#define BLE_HZ 4
#endif

// Level of debugging to the serial port
#ifndef _DEBUG_
#define _DEBUG_ true
#endif

#ifndef _XDEBUG_
#define _XDEBUG_ false
#endif

#include <Arduino.h> // For type inclusion - String for example
#include "BMS.h"
#include "BLE.h"
#include "BMP.h"
#include "BUT.h"
#include "GPS.h"
#include "IMU.h"
#include "LED.h"
#include "LOG.h"
#include "NET.h"
#include "RTC.h"
#include "SRV.h"
#include "WEB.h"

#endif
