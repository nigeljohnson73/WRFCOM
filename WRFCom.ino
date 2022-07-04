/**************
  SPIFFS is installed and this sketch is designed for an ESP8266 (not the ESP32) so will need some rework around networking and more specifically web servering.

  To upload data to the static file store:

   - Add files into the 'data' directory in the sketch
   - Close the serial monitor
   - from the tools menu select 'ESP8266 Sketch Data Upload'

  Things to do

   - Attach GPS
   - Attach OpenLog
   - Attach Battery fuel guage
   - Configure Servo system
*/

#ifdef ESP32
#include <SPIFFS.h>
#else
#include <FS.h>
#endif

#include "App.h"

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();

  Network.setHostname(_AP_NAME_);
  Network.begin(WIFI_SSID, WIFI_PASS, WIFI_WAIT);

  RTC.begin();
  BAT.begin();
  BMP.begin();
  IMU.begin();
  GPS.begin();
  SRV.begin();

  LOG.begin();
  Server.begin();
}


void loop() {
  long sweep_millis = 0;
  long m_start = millis();

  Network.loop();
  RTC.loop();
  BAT.loop();
  BMP.loop();
  IMU.loop();
  GPS.loop();
  SRV.loop();

  LOG.loop();
  Server.loop();

  sweep_millis = millis() - m_start;
  long sleep_for = max((long) 1, (1000 / SENSOR_HZ) - sweep_millis);

  delay(sleep_for);

#if _XDEBUG_
  Serial.print("sweep(): ");
  Serial.print(sweep_millis);
  Serial.print("ms, ");
  Serial.print(", Sleep for: ");
  Serial.println(sleep_for);
  Serial.print("md, loop(): ");
  Serial.print(millis() - m_start);
  Serial.print("ms");
  Serial.println();
#endif
}
