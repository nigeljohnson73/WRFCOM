#ifndef _APP_h
#define _APP_h

#define VERSION "v0.1a"

// This is how long the wifi will look for an AP to connect to before launcing into it's own mess
#define WIFI_WAIT 30

// In AP mode (When the Wifi will not connect) this is the password.
// it needs to be at least 8 chars long for most devices to let you enter the details
#define _AP_NAME_ "WRFCOM"
#define _AP_PASSWORD_ "12345678"

// If you going to be connected to a network, then this is good starter. Use ZULU/GMT/UTC
#define _USE_NTP_ true
#define _NTP_OFFSET_SECONDS_ 0

// How long should we do a log before auto shutting it off
#define MAX_LOG_DURATION_SECONDS (15*60)
//#define MAX_LOG_DURATION_SECONDS (15)

// Should the servo code be used for parachute deployment
#define USE_SERVO true
#if ESP32
#define SERVO_PIN 12
#define LED_PIN 13
#else
#define SERVO_PIN D4
#define LED_PIN LED_BUILTIN
#endif

// Once the peak height has been reached, then at this point, throw out hte parachute
#define PARACHUTE_DEPLOY_APOGEE_OFFSET -1

// Configure LiPO and monitor use
#define USE_LIPO true
#define LIPO_SIZE LC709203F_APA_500MAH
// Options: LC709203F_APA_100MAH, LC709203F_APA_200MAH, LC709203F_APA_500MAH, LC709203F_APA_1000MAH, LC709203F_APA_2000MAH, LC709203F_APA_3000MAH

// How fast should we coll the sensors
// The slowest is going to be the GPS at 18 Hz, but on the D1 mini, 15 will choke the IIC bus
#define SENSOR_HZ 14

// Level of debugging to the serial port
#define _DEBUG_ true
#define _XDEBUG_ false


#if USE_LIPO
#include "BAT.h"
#endif

#include <Arduino.h> // For type inclusion - String for example
#include "BMP.h"
#include "BUT.h"
#include "GPS.h"
#include "IMU.h"
#include "LOG.h"
#include "NET.h"
#include "RTC.h"
#include "SRV.h"
#include "WEB.h"

#endif
