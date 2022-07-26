# Water Rocket Flight Computer

This is a small project that uses off the shelf modules as much as possible to build a data logger for attaching to a 
water bottle rocket... or any kind of vehicle. The idea is that is used GPS and barimetric pressure to define your 
location with some additional parameters from Gyros and Accelerometers (maybe magnetometers). All of this data being 
logged to an SD card as fast as possible and running from a small battery. The interface should be a web page from the 
on-board WiFi Access Point, as well as a load of Bluetooth LE (BLE) data and control.

See the '[BLE Remote Controller](https://github.com/nigeljohnson73/WRFCOM-Remote)' for more on that.

It would be nice if I could build an app that would suck off the data and display it without having to pull the SD
card, but that is a stretch goal.

Things to install
-----------------
I will assume you are comfortable with the Arduino IDE.

You should install the latest ESP32 boards into your board manager if you haven't already (depending on which you 
want to use). Add these to your "Additional Boards Manager URLs" section in preferences.

```
https://dl.espressif.com/dl/package_esp32_index.json, 
http://arduino.esp8266.com/stable/package_esp8266com_index.json
```

<!--
You will need to install the file system plugins for 
[ESP32](https://microcontrollerslab.com/install-esp32-filesystem-uploader-in-arduino-ide-spiffs/) and/or 
[ESP8266](https://randomnerdtutorials.com/install-esp8266-filesystem-uploader-arduino-ide/) depending 
on which platform youre using. This will allow you to upload the static (mostly image) files for the web pages.

I have also found the [ESP Exception Decoder](https://github.com/me-no-dev/EspExceptionDecoder) to be very useful.
-->

Code to update
--------------
To maintain the ability for you to modify the settings without causing a code thing if you're working on the code, 
you should create a file called `myConfig.h` and in it define any of the overridable settings in `App.h` for example:

```
#define WIFI_SSID "YOUR-WIFI-SSID"
#define WIFI_PASS "YOUR-WIFI-PASS"
```

Hardware required
-----------------
Apart from the controller you will also need sensors and battery bits. For the sake of sanity I used off the shelf QWIIC
connecting sensor breakout boards. If you're good with soldering, you can certainly make things a lot smaller (and 
probably cheaper). The servo bits did need a little cutting and soldering, but this appeared to be well within my capability.

The following microcontrollers have been tested and in my preferred order:

* [ESP32-S3 Feather][ESP32-S3] Dual core, BLE, WiFi 4MB flash, 2MB PSRAM
* [ESP32-S3 Feather][ESP32-S3-NOPSRAM] Dual core, BLE, WiFi 8MB flash, No PSRAM
* [ESP32-S2 Feather][ESP32-S2] Single core, No BLE, WiFi

### Sensors and things
* [LIPO battery pack][LIPO-1200MAH]
* [PCF8523 Real Time Clock Breakout Board][PCF8523] (really optional)
* [DPS310 Barometric Pressure Sensor][DPS310] (really optional)
* [LSM6DSO32 32g 6DOF Inertial Measurement Unit][LSM6DSO32] (kind of optional)
* [SparkFun OpenLog][OPENLOG]
* [Simple class 10 MicroSD card][SDCARD]
* [SAM-M8Q GPS Breakout][SAM-M8Q]
* 5-7 [50mm QWIIC connector cables][QWIIC-CONNECTOR-50]

### Alternate components
* [BMP390 Barometric Pressure Sensor][BMP390]
* A smaller GPS option is the [ZOE-M8Q GPS Breakout][ZOE-M8Q] with a [Small u-FL antenna][UFL-ANTENNA]
* * [LSM6DSOX/LIS3MDL Combo][LSM6DSOX] 9DOF IMU

[ESP32-S3]: https://thepihut.com/products/adafruit-esp32-s3-feather-with-4mb-flash-2mb-psram-stemma-qt-qwiic
[ESP32-S3-NOPSRAM]: https://thepihut.com/products/adafruit-esp32-s3-feather-with-stemma-qt-qwiic-8mb-flash-no-psram
[ESP32-S2]: https://thepihut.com/products/adafruit-esp32-s2-feather-2-mb-psram-and-stemma-qt-qwiic
[LIPO-1100MAH]: https://smile.amazon.co.uk/dp/B087LTZW61
[LIPO-1200MAH]: https://shop.pimoroni.com/products/lipo-battery-pack?variant=20429082183
[PCF8523]: https://thepihut.com/products/adafruit-pcf8523-real-time-clock-breakout-board-stemma-qt-qwiic
[BMP390]: https://shop.pimoroni.com/products/adafruit-bmp390-precision-barometric-pressure-and-altimeter-stemma-qt-qwiic?variant=32302189346899
[DPS310]: https://thepihut.com/products/adafruit-dps310-precision-barometric-pressure-altitude-sensor
[LSM6DSO32]: https://thepihut.com/products/adafruit-lsm6dso32-6-dof-accelerometer-and-gyroscope
[LSM6DSOX]: https://thepihut.com/products/adafruit-lsm6dsox-lis3mdl-precision-9-dof-imu?variant=31618642182206
[ZOE-M8Q]: https://shop.pimoroni.com/products/sparkfun-gps-breakout-zoe-m8q-qwiic?variant=31615967789139
[UFL-ANTENNA]: https://shop.pimoroni.com/products/passive-gps-antenna-ufl-9mm-x-9mm-2dbi-gain?variant=4487793281
[SAM-M8Q]: https://thepihut.com/products/sparkfun-gps-breakout-chip-antenna-sam-m8q-qwiic
[OPENLOG]: https://thepihut.com/products/sparkfun-openlog
[QWIIC-CONNECTOR-100]: https://thepihut.com/products/stemma-qt-qwiic-jst-sh-4-pin-cable-100mm-long
[QWIIC-CONNECTOR-50]: https://thepihut.com/products/stemma-qt-qwiic-jst-sh-4-pin-cable
[SDCARD]: https://smile.amazon.co.uk/dp/B07R59FHVG

If you want to control a parachute as well, then you will need a servo, and if you want touch control, a button is useful:

<!-- * [8-Channel PWM or Servo FeatherWing][SERVO-FEATHER] -->
* [Micro servo][TOWER-PRO]
* [Buck converter][BUCKY-5V]
* SparkFun Qwiic Button[Green][BUTTON-GREEN] or [Red][BUTTON-RED]

[TOWER-PRO]: https://thepihut.com/products/servo-motor-sg92r-micro
[BUCKY-5V]: https://thepihut.com/products/dc-dc-automatic-step-up-down-power-module-3-15v-to-5v-600ma?variant=39824836886723
[SERVO-EXTENSION]: https://thepihut.com/products/servo-motor-sg92r-micro
[BUTTON-GREEN]: https://thepihut.com/products/sparkfun-qwiic-button-green-led
[BUTTON-RED]: https://thepihut.com/products/sparkfun-qwiic-button-red-led

You will need to cut the extension cable and solder the female side to your microcontroller. GND, BATT for power and GPIO12 for signal. You will also need to 
splice in the buck converter into the power line (not the signal). Preferably on the controller end so you can use any servo.

Libraries required
------------------
* NTPClient (for getting the network time)
* Adafruit BMP3XX Library (for the BMP390 Barometric Pressure Sensor)
* Adafruit LSM6DS (for the LSM6DSO32 and LSMM6DOX IMU)
* Adafruit LIS3MDL (for the LIS3MDL magnetometer)
* Adafruit Unified Sensor and Adafruit BusIO (for the LSM6DSO32/oX IMU and DPS310 Barometric Pressure Sensor)
* Adafruit LC709203F (for the onboard LiPO monitor)
* RTCLib (for the PCF8523 RTC module)
* SparkFun Qwiic OpenLog (for the OpenLog module)
* SparkFun u-blox GNSS Arduino Library (for the ZOE-M8Q and SAM-M8Q GPS module)
* SparkFun Qwiic Button and Qwiic Switch Library (for the Qwiic buttons)
* [ESPAsyncWebServer][ESPAsyncWebServer] and [AsyncTCP][AsyncTCP] (unzip into your library folder and remove the `-master` in the folder name)

[ESPAsyncWebServer]: https://github.com/me-no-dev/ESPAsyncWebServer
[AsyncTCP]: https://github.com/me-no-dev/AsyncTCP

<!--
### Access points
At this point, the code is designed to create an access point if it cannot connect to a wireless network. However, this
appears to be really slow and flakey for some reason. For use out in the field, I'd recommend setting up a hotspot on 
your phone for it to connect to. Finding the IP address to connect to though can be tricky. On my Pixel device I cannot
see a list of connected devices (thanks google for the proivacy concern). If you use a terminal emulator though, you can
type the following in and it will list the IP addresses of any attached device (which should be the only one)

```
ip neigh
```

Future plans for the ESP32 build will possibly include a BTLE broadcast of it's IP address... or possibly using the bluetooth
connection in an app (for android only) or, better yet, working out why this is so flakey.
-->
