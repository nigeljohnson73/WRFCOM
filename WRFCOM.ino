/**************
  SPIFFS is installed and this sketch is designed for an ESP8266 (not the ESP32) so will need some rework around networking and more specifically web servering.

  To upload data to the static file store:

   - Add files into the 'data' directory in the sketch
   - Close the serial monitor
   - from the tools menu select 'ESP8266 Sketch Data Upload'

  Things to do

   - Configure Servo system
*/

//#ifdef ESP32
//#include <SPIFFS.h>
//#else
//#include <FS.h>
//#endif
#include <Wire.h>

// Create a file called myWIFI.h and add the following (but updated it with your settings)
//#define WIFI_SSID "YOUR-WIFI-SSID"
//#define WIFI_PASS "YOUR-WIFI-PASS"
#include "myWIFI.h"
#include "App.h"

void setup() {
  Wire.begin(); // Initialise the IIC bus (GPS and buttons);

  Serial.begin(115200);
  delay(5000);
  Serial.println();

  NET.setHostname(_AP_NAME_);
  NET.begin(WIFI_SSID, WIFI_PASS, WIFI_WAIT);

  RTC.begin();
#if USE_LIPO
  BAT.begin();
#endif
  BMP.begin();
  BUT.begin();
  IMU.begin();
  GPS.begin();
  SRV.begin();

  LOG.begin();
  WEB.begin();
}

long last_sweep = 0;
void loop() {
  unsigned long now = millis();

  NET.loop();
  if (last_sweep == 0 || (now - last_sweep) >= (1000.0 / double(SENSOR_HZ))) {
    last_sweep = now;
    RTC.loop();
#if USE_LIPO
    BAT.loop();
#endif
    BMP.loop();
    BUT.loop();
    IMU.loop();
    GPS.loop();
    SRV.loop();

    LOG.loop();
  }
  WEB.loop();
}
