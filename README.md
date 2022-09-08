# Water Rocket Flight Computer

For more background and discussion, check out [the wiki](https://github.com/nigeljohnson73/WRFCOM/wiki).

This is a small project that uses off the shelf modules as much as possible to build a data logger for attaching to a 
water bottle rocket... or any kind of vehicle. The idea is that is uses various sensors to define your position in 
3 dimensions with some additional parameters from Gyros, Accelerometers and magnetometers. All of this data being 
logged to an SD card as fast as possible and running from a small battery. The interface should be a web page from the 
on-board WiFi Access Point.

See the '[BLE Remote Controller](https://github.com/nigeljohnson73/WRFCOM-Remote)' for more on a BLE interface.

Things to install
-----------------
I will assume you are comfortable with the Arduino IDE.

You should install the latest ESP boards into your board manager if you haven't already (depending on which you 
want to use). You can only install the ESP32 bitis if you are using the out-of-the-box stuff in this project.
Add these to your "Additional Boards Manager URLs" section in preferences.

```
https://arduino.esp8266.com/stable/package_esp8266com_index.json
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json
```

With that installed the board you want is the `Adafruit Feather ESP32-S3 No PSRAM` for the out-of-the-box project.

<!--
You will need to install the file system plugins for 
[ESP32](https://microcontrollerslab.com/install-esp32-filesystem-uploader-in-arduino-ide-spiffs/) and/or 
[ESP8266](https://randomnerdtutorials.com/install-esp8266-filesystem-uploader-arduino-ide/) depending 
on which platform youre using. This will allow you to upload the static (mostly image) files for the web pages.

I have also found the [ESP Exception Decoder](https://github.com/me-no-dev/EspExceptionDecoder) to be very useful.
-->

Code to update
--------------
To maintain the ability for you to modify settings without causing a code thing if you're working on the code, 
you should create a file called `myConfig.h` and define any of the overridable settings in `App.h` for example:

```
#define WIFI_SSID "YOUR-WIFI-SSID"
#define WIFI_PASS "YOUR-WIFI-PASS"
```

This will be discussed more in the Wiki.

Hardware required
-----------------
For the sake of sanity I used off the shelf QWIIC connecting sensor breakout boards. If you're good with soldering, you can certainly 
make things a lot smaller (and probably cheaper). The servo bits did need a little cutting and soldering, but this appeared to be 
well within my capability. A review of the sensors and their options will happen over in the Wiki.

The following microcontrollers have been tested and in my preferred order:

* [ESP32-S3 Feather][ESP32-S3-NOPSRAM] Dual core, BLE, WiFi 8MB flash, No PSRAM. My primary option.
* [ESP32-S3 Feather][ESP32-S3] Dual core, BLE, WiFi 4MB flash, 2MB PSRAM. The PSRAM may come in handy later.
* [ESP32-S2 Feather][ESP32-S2] Single core, No BLE, WiFi. A good board that works for all the core stuff.

### Important components
* [LIPO battery pack][LIPO-1200MAH]
* [SparkFun OpenLog][OPENLOG]
* [Simple class 10 MicroSD card][SDCARD]
* [SAM-M8Q GPS Breakout][SAM-M8Q]
* Enough [50mm QWIIC connector cables][QWIIC-CONNECTOR-50] for your sensors
* A lot of M2.3x5, 2 M2.3x10, 2 M2x5 and 4 M2.6x5 self tapping screws
* You might need some command strips to cut up and attach cables

### Optional components
* [LSM6DSOX/LIS3MDL Combo][LSM6DSOX] 9DOF IMU
* [DPS310 Barometric Pressure Sensor][DPS310]
* [PCF8523 Real Time Clock Breakout Board][PCF8523]
* [Power switch][POWER-SWITCH] and [extension cable][SWITCH-EXTENSION]

### Alternate components
* [BMP390 Barometric Pressure Sensor][BMP390]
* [LSM6DSO32 32g 6DOF Inertial Measurement Unit][LSM6DSO32]
* A smaller GPS option is the [ZOE-M8Q GPS Breakout][ZOE-M8Q] with a [u.Fl antenna][UFL-ANTENNA-LARGE]
* * There is a [small u.Fl antenna][UFL-ANTENNA-SMALL], but it can't pick up as many satelites

[SWITCH-EXTENSION]: https://thepihut.com/products/jst-ph-2-pin-cable-female-connector-150mm
[POWER-SWITCH]: https://thepihut.com/products/adafruit-switched-jst-ph-2-pin-smt-right-angle-breakout-board
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
[UFL-ANTENNA-SMALL]: https://thepihut.com/products/passive-gps-antenna-ufl-9mm-x-9mm-2dbi-gain
[UFL-ANTENNA-LARGE]: https://thepihut.com/products/passive-gps-antenna-ufl-15mm-x-15mm-1-dbi-gain
[SAM-M8Q]: https://thepihut.com/products/sparkfun-gps-breakout-chip-antenna-sam-m8q-qwiic
[OPENLOG]: https://thepihut.com/products/sparkfun-openlog
[QWIIC-CONNECTOR-100]: https://thepihut.com/products/stemma-qt-qwiic-jst-sh-4-pin-cable-100mm-long
[QWIIC-CONNECTOR-50]: https://thepihut.com/products/stemma-qt-qwiic-jst-sh-4-pin-cable
[SDCARD]: https://smile.amazon.co.uk/dp/B07R59FHVG

### Paracute option
If you want to control a parachute as well, then you will need a couple more bits:

* [9g servo][TOWER-PRO] or a [micro servo][MICRO-SERVO].
* [Buck converter][BUCKY-5V]
* [Servo extension cable][SERVO-EXTENSION]

You will need to cut the extension cable and solder the female side to your microcontroller. GND, BATT for power and GPIO12 for signal. 
You will also need to splice in the buck converter into the power line (not the signal). Preferably on the controller end so you can 
use any servo.

[MICRO-SERVO]: https://www.amazon.co.uk/dp/B09NY5C4N8
[TOWER-PRO]: https://thepihut.com/products/servo-motor-sg92r-micro
[BUCKY-5V]: https://thepihut.com/products/dc-dc-automatic-step-up-down-power-module-3-15v-to-5v-600ma?variant=39824836886723
[SERVO-EXTENSION]: https://thepihut.com/products/servo-motor-sg92r-micro

### Gotta have a button?

If you want touch control for arming and starting the logging, a button is useful for testing but highly impractical for real use:

* SparkFun Qwiic Button [Green][BUTTON-GREEN] or [Red][BUTTON-RED]

[BUTTON-GREEN]: https://thepihut.com/products/sparkfun-qwiic-button-green-led
[BUTTON-RED]: https://thepihut.com/products/sparkfun-qwiic-button-red-led

Libraries required
------------------
All of these are available in the Library Manager in the Arduino IDE.

* Adafruit LC709203F (for the Feather S2/3's onboard LiPO monitor)
* SparkFun u-blox GNSS Arduino Library (for the ZOE-M8Q and SAM-M8Q GPS modules)
* SparkFun Qwiic OpenLog (for the OpenLog module)
* RTCLib (for the PCF8523 RTC module)
* Adafruit BMP3XX Library (for the BMP390 Barometric Pressure Sensor)
* Adafruit DPS310 Library (for the DSP310 Barometric Pressure Sensor)
* Adafruit LSM6DS (for the LSM6DSO32 and LSMM6DOX IMU)
* Adafruit LIS3MDL (for the LIS3MDL magnetometer)
* Adafruit Unified Sensor and Adafruit BusIO (for the LSM6DSO32/OX IMU and DPS310 Barometric Pressure Sensor)
* ESP32Servo (for the parachute trigger)
* NTPClient (for getting the network time, if enabled)
* SparkFun Qwiic Button and Qwiic Switch Library (for the Qwiic buttons)

 __NOTE:__ The ESP32Servo servo control compiles in and works on my Mac, but not on my Dell. On my Dell everything compiles and uploads fine, but the servo just does not respond. Could also be useing "PWMServo" but still unsure on Windows.

You will also need the follwing directly from GitHUB. Download them from there, unzip into your arduino library folder and remove the `-master` in the folder name.

* [ESPAsyncWebServer][ESPAsyncWebServer]
* [AsyncTCP][AsyncTCP] 

[ESPAsyncWebServer]: https://github.com/me-no-dev/ESPAsyncWebServer
[AsyncTCP]: https://github.com/me-no-dev/AsyncTCP
