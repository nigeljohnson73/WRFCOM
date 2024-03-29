// Figlet: https://www.askapache.com/online-tools/figlet-ascii/
// font: nancyj
// Maps: https://www.gpsvisualizer.com/map_input?country=gb&form=html&format=google&units=metric

#ifndef _APP_h
#define _APP_h

// Define the types of IMU that are available
#define IMU_NONE 0xff
#define IMU_LSM6DSO32 0x01 // 6DoF up to 32g no mags
#define IMU_LSM6DSOX_LIS3MDL 0x02 // 9DoF up to 16g with mags to 16 gauss

// Define the types of EMU that are available
#define EMU_NONE 0xff
#define EMU_BMP390 0x01
#define EMU_DSP310 0x02

// Version 4 contains mag setup in the IMU
// Version 3 is the first flight config that worked
#define VERSION "v0.4a"

// Preload our config to override any of the following parameters
#include "myConfig.h"

// Should we enable accees points and WiFi (and possibly allow OTA, NTP and AP mode)
#ifndef _USE_WIFI_
#define _USE_WIFI_ true
#endif

// Should we enable accees points and WiFi (and possibly allow OTA, NTP and AP mode)
#ifndef _USE_NOW_
#define _USE_NOW_ false
#endif

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

// This will be the access point name if no WiFi connection happens, but also the BLE server name
// it needs to be a String type
#ifndef DEVICE_NAME
#define DEVICE_NAME (String("WRFCOM-") + espChipId())
#endif

// In AP mode (When the Wifi will not connect) this is the password.
// it needs to be at least 8 chars long for most devices to let you enter the details
#ifndef AP_PASSWORD
#define AP_PASSWORD "12345678"
#endif

// Should we enable OTA updates for debugging and easier access on your home network, it's slower though
#ifndef _USE_OTA_
#define _USE_OTA_ false
#endif

// If you going to be connected to a network, then this is good starter
#ifndef _USE_NTP_
#define _USE_NTP_ false
#endif

// Use ZULU/GMT/UTC
#ifndef NTP_OFFSET_SECONDS
#define NTP_OFFSET_SECONDS 0
#endif

// Configure LiPO and monitor use
#ifndef _USE_BMS_
#define _USE_BMS_ true
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

// Should we use RTC
#ifndef _USE_RTC_
#define _USE_RTC_ true
#endif

// Sholud we use Environment Measurement Unit
#ifndef EMU_TYPE
#define EMU_TYPE EMU_BMP390
#endif

//#ifndef _USE_EMU_
//#define _USE_EMU_ (EMU_TYPE != EMU_NONE)
//#endif

// Shoud we use Inertial Measurement Unit
//#ifndef _USE_IMU_
#ifndef IMU_TYPE
#define IMU_TYPE IMU_LSM6DSOX_LIS3MDL
#endif

// Will the device be upside down in the payload bay (i.e. GPS down)
#ifndef _UPSIDE_DOWN_
#define _UPSIDE_DOWN_ true
#endif

// Should we use Bluetooth LE
#ifndef _USE_BLE_
#define _USE_BLE_ true
#endif

// How fast should we push the BLE notification
#ifndef BLE_HZ
#define BLE_HZ 4
#endif

// Should we use a physical button
#ifndef _USE_BUTTON_
#define _USE_BUTTON_ false
#endif

// Should the servo code be used for parachute deployment
#ifndef _USE_SERVO_
#define _USE_SERVO_ true
#endif

// How long should we do a log before auto shutting it off
#ifndef MAX_LOG_DURATION_SECONDS
#define MAX_LOG_DURATION_SECONDS (5*60)
#endif

// How fast should we poll the sensors
// The slowest is going to be the GPS at 18 Hz
#ifndef SENSOR_HZ
#define SENSOR_HZ 18
#endif

// Define the int/double value that means 'bollocks'
#ifndef DUFF_VALUE
#define DUFF_VALUE -999999
#endif

#ifndef EARTH_RADIUS
#define EARTH_RADIUS 6372797.56085
#endif

#ifndef ONE_G
#define ONE_G 9.80665
#endif

// Level of debugging to the serial port
#ifndef _DEBUG_
#define _DEBUG_ true
#endif

// Should disabled sensors output their initialsiation text (so you know they are there)
#ifndef _DISABLED_DEBUG_
#define _DISABLED_DEBUG_ false
#endif

// Really really flood the crap out of the serial port. Probably should reduce the sensor Hz if you're wanting this.
#ifndef _XDEBUG_
#define _XDEBUG_ false
#endif

#include <Arduino.h> // For type inclusion - String for example
extern String espChipId();
extern double gpsDistance(double lat1, double lng1, double lat2, double lng2);
extern double gpsBearing(double lat1, double lng1, double lat2, double lng2);

#include "BLE.h"
#include "EMU.h"
#include "BMS.h"
#include "BUT.h"
#include "GPS.h"
#include "IMU.h"
#include "LED.h"
#include "LOG.h"
#include "NET.h"
#include "NOW.h"
#include "RTC.h"
#include "SRV.h"
#include "WEB.h"

#endif
