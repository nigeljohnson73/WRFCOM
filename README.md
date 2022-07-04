# WRFCOM
Water Rocket Flight Computer.

Things to install
-----------------

I will assume you are comfortable with the Arduino IDE.

You should install the ESP8266 and ESP32 boards into your board manager (depending on which you want to use). 
Add these to your additional sources in preferences.

    https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json

You will also need to install the file system plugins for 
[ESP32](https://microcontrollerslab.com/install-esp32-filesystem-uploader-in-arduino-ide-spiffs/) and/or 
[ESP8266](https://randomnerdtutorials.com/install-esp8266-filesystem-uploader-arduino-ide/) depending 
on which platform youre using.

I have also found the [ESP Excpetion Decoder](https://github.com/me-no-dev/EspExceptionDecoder) to be very useful.

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


