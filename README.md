# Water Rocket Flight Computer

This is a small project that uses off the shelf modules as much as possible to build a data logger for attaching to a 
water bottle rocket... or any kind of vehicle. The idea is that is used GPS and barametric pressure to define your 
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
https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
```

You will need to install the file system plugins for 
[ESP32](https://microcontrollerslab.com/install-esp32-filesystem-uploader-in-arduino-ide-spiffs/) and/or 
[ESP8266](https://randomnerdtutorials.com/install-esp8266-filesystem-uploader-arduino-ide/) depending 
on which platform youre using. This will allow you to upload the static (mostly image) files for the web pages.

I have also found the [ESP Exception Decoder](https://github.com/me-no-dev/EspExceptionDecoder) to be very useful.

Libraries required
------------------

* NTPClient (for getting the network time)
* Adafruit BMP3XX Library (for the BMP390 Barometric Pressure Sensor)
* Adafruit LSM6DS, Adafruit Unified Sensor and Adafruit BusIO (for the LSM6DSO32 32g 6DOF IMU)
* Adafruit LC709203F (for the LiPO monitor)
* RTCLib (for the PCF8523 RTC module)
* SparkFun Qwiic OpenLog (for the OpenLog module)
* SparkFun u-blox GNSS Arduino Library (for the ZOE-M8Q GPS module)
* Adafruit PWM Servo Driver Library (for the 8-channel servo board)
* SparkFun Qwiic Button and Qwiic Swith Library (for the Qwiic buttons)


