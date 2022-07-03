/**************
   SPIFFS is installed and this is designed for an ESP8266 (not the ESP32)

   Add files into the 'data' directory in the sketch
   Close the serial monitor
   from the tools menu select 'ESP8266 Sketch Data Upload'

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

  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
  }

  Network.setHostname(_AP_NAME_);
  Network.begin(WIFI_SSID, WIFI_PASS, WIFI_WAIT);

  RTC.begin();
  BMP.begin();
  IMU.begin();
  GPS.begin();
  Logger.begin();
  Server.begin();
}


void loop() {
	long sweep_millis = 0;
  long m_start = millis();

  Network.loop();
  RTC.loop();
  BMP.loop();
  IMU.loop();
  GPS.loop();

  Logger.loop();
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
