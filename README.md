# Water Rocket Flight Computer

This is a small project that uses off the shelf modules as much as possible to build a data logger for attaching to a 
water bottle rocket... or any kind of vehicle. The idea is that is used GPS and barimetric pressure to define your 
location with some additional parameters from Gyros and Accelerometers (maybe magnetometers). All of this data being 
logged to an SD card as fast as possible and running from a small battery. Optionally I want to add a parachute 
deployment mechanism as well. Control should also be via a web page delivered through it's own Wireless Access Point 
(maybe BTLE if I can work it out).

It would be nice if I could build an app that would suck off the data and display it without having to pull the SD
card, but that is a stretch goal.

Things to install
-----------------
I will assume you are comfortable with the Arduino IDE.

You should install the ESP8266 and ESP32 boards into your board manager (depending on which you want to use). 
Add these to your "Additional Boards Manager URLs" section in preferences.

```
https://dl.espressif.com/dl/package_esp32_index.json, 
http://arduino.esp8266.com/stable/package_esp8266com_index.json
```

You will need to install the file system plugins for 
[ESP32](https://microcontrollerslab.com/install-esp32-filesystem-uploader-in-arduino-ide-spiffs/) and/or 
[ESP8266](https://randomnerdtutorials.com/install-esp8266-filesystem-uploader-arduino-ide/) depending 
on which platform youre using. This will allow you to upload the static (mostly image) files for the web pages.

I have also found the [ESP Exception Decoder](https://github.com/me-no-dev/EspExceptionDecoder) to be very useful.

Code to update
--------------
To maintain the ability for you to modify your WiFi settings without causing a code thing if you're working on the code, 
you should create a file called `myWIFI.h` and in it define your wifi access token.

```
#define WIFI_SSID "YOUR-WIFI-SSID"
#define WIFI_PASS "YOUR-WIFI-PASS"
```

Hardware required
-----------------
Apart from the controller you will also need sensors and battery bits. For the sake of sanity I used off the shelf QWIIC
connecting sensor breakout boards. If you're good with soldering, you can certainly make things a lot smaller (and 
probably cheaper).

The following microcontrollers have been tested:

* LOLIN D1 mini (ESP8266 board) Wiring is fun, you'll need a [power socket][JST-POWER] and half a [QWIIC cable][QWIIC-CONNECTOR]
* [ESP32-S3 Feather][ESP32-S3] Dual core, BT, WiFi 4MB flash, 2MB PSRAM
* [ESP32-S3 Feather][ESP32-S3-NOPSRAM] Dual core, BT, WiFi 8MB flash, No PSRAM
* [ESP32-S2 Feather][ESP32-S2] No Bluetooth

[ESP32-S3]: https://shop.pimoroni.com/products/adafruit-esp32-s3-feather-with-4mb-flash-2mb-psram-stemma-qt-qwiic?variant=40017517215827
[ESP32-S3-NOPSRAM]: https://shop.pimoroni.com/products/adafruit-esp32-s3-feather-with-stemma-qt-qwiic-8mb-flash-no-psram?variant=39878737395795
[ESP32-S2]: https://shop.pimoroni.com/products/adafruit-esp32-s2-feather-2-mb-psram-and-stemma-qt-qwiic?variant=39537754210387

### Sensors
* [PCF8523 Real Time Clock Breakout Board][RTC]
* [BMP390 Barometric Pressure Sensor][BMP390]
* [LSM6DSO32 32g 6DOF Inertial Measurement Unit][IMU]
* [SparkFun OpenLog][OPENLOG]
* [Simple class 10 MicroSD card][SDCARD]
* [ZOE-M8Q GPS Breakout][GPS]
* [QWIIC connector cables][QWIIC-CONNECTOR]

[RTC]: https://shop.pimoroni.com/products/adafruit-pcf8523-real-time-clock-breakout-board-stemma-qt-qwiic?variant=39421340680275
[BMP390]: https://shop.pimoroni.com/products/adafruit-bmp390-precision-barometric-pressure-and-altimeter-stemma-qt-qwiic?variant=32302189346899
[IMU]: https://shop.pimoroni.com/products/adafruit-lsm6dso32-6-dof-accelerometer-and-gyroscope-stemma-qt-qwiic?variant=32237051641939
[GPS]: https://shop.pimoroni.com/products/sparkfun-gps-breakout-zoe-m8q-qwiic?variant=31615967789139
[OPENLOG]: https://shop.pimoroni.com/products/sparkfun-qwiic-openlog?variant=40046785953875
[QWIIC-CONNECTOR]: https://shop.pimoroni.com/products/jst-sh-cable-qwiic-stemma-qt-compatible?variant=31910609813587
[SDCARD]: https://smile.amazon.co.uk/dp/B07R59FHVG

If you want to control a parachute as well, then you will need the following, and can only really use the PP3 battery
option.

<!-- * [8-Channel PWM or Servo FeatherWing][SERVO-FEATHER] -->
* [Micro servo][TOWER-PRO]
* Optionally [SparkFun Qwiic Button][BUTTON]

[SERVO-FEATHER]: https://shop.pimoroni.com/products/8-channel-pwm-or-servo-featherwing-add-on-for-all-feather-boards?variant=13710765383
<!-- [MICRO-SERVO]: https://shop.pimoroni.com/products/servo-generic-sub-micro-size?variant=19330899143 -->
[TOWER-PRO]: https://thepihut.com/products/servo-motor-sg92r-micro
[BUTTON]: https://shop.pimoroni.com/products/sparkfun-qwiic-button-green-led?variant=32000924647507

### LiPO battery option
This setup is lighter, smaller and more monitorable.

* [1,200mAh LiPO battery][LIPO-BATTERY]
* [LC709203F LiPoly fuel guage][LIPO-MONITOR] (built into the Feather boards so you won't need the external QWIIC board)
<!-- * [AP3429A 3.3V Buck Converter Breakout][REGULATOR-3v] -->
* Optionally the [LiPo Amigo charger and switch][AMIGO]

<!-- [REGULATOR-3v]: https://shop.pimoroni.com/products/ap3429a-3-3v-buck-converter-breakout-3-3v-output-1-2a-max?variant=32173899546707 -->
[LIPO-BATTERY]: https://shop.pimoroni.com/products/lipo-battery-pack?variant=20429082183
[AMIGO]: https://shop.pimoroni.com/products/lipo-amigo?variant=39779302539347
[LIPO-MONITOR]: https://shop.pimoroni.com/products/adafruit-lc709203f-lipoly-liion-fuel-gauge-and-battery-monitor-stemma-jst-ph-qt-qwiic?variant=32236623396947

<!-- 
### PP3 battery option
This option will run the controller but you have no visibility of the power level.

* [PP3 battery][PP3-BATTERY]
* [PP3 battery snap][PP3-SNAP]
* [5v power regulator][REGULATOR-5V]
* 2x [JST power leads][JST-POWER] (one for the 5v rail of the microcontroller and one for the servo board)

[JST-POWER]: https://shop.pimoroni.com/products/jst-2-wire-assembly?variant=711377809
[PP3-SNAP]: https://shop.pimoroni.com/products/pp3-battery-snap?variant=371728927
[REGULATOR-5V]: https://smile.amazon.co.uk/dp/B08HK6Z91G
[PP3-BATTERY]: https://smile.amazon.co.uk/dp/B093CBYWSL
-->

Libraries required
------------------
* NTPClient (for getting the network time)
* Adafruit BMP3XX Library (for the BMP390 Barometric Pressure Sensor)
* Adafruit LSM6DS, Adafruit Unified Sensor and Adafruit BusIO (for the LSM6DSO32 32g 6DOF IMU)
* Adafruit LC709203F (for the LiPO monitor)
* RTCLib (for the PCF8523 RTC module)
* SparkFun Qwiic OpenLog (for the OpenLog module)
* SparkFun u-blox GNSS Arduino Library (for the ZOE-M8Q GPS module)
<!-- * Adafruit PWM Servo Driver Library (for the 8-channel servo board) -->
* SparkFun Qwiic Button and Qwiic Switch Library (for the Qwiic buttons)
* [ESPAsyncWebServer][ESPAsyncWebServer] and [AsyncTCP][AsyncTCP] (unzip into your library folder and remove the `-master` in the folder name)

[ESPAsyncWebServer]: https://github.com/me-no-dev/ESPAsyncWebServer
[AsyncTCP]: https://github.com/me-no-dev/AsyncTCP
Known issues
------------

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
