/**************
  SPIFFS doesnt work on the Feather S3 :(
*/

#include <Wire.h>
#include "App.h"

void setup() {
  Wire.begin(); // Initialise the IIC bus (GPS and buttons);
  Wire.setClock(400000); //Go super fast

  Serial.begin(115200);
  delay(5000);
  Serial.println();

  BUT.begin(); // Switch it off early
  LED.begin();

  RTC.begin();
  BMS.begin();
  BMP.begin();
  IMU.begin();
  GPS.begin();
  SRV.begin();

  LOG.begin();

  BLE.begin();
  NET.setHostname(AP_NAME);
  NET.begin(WIFI_SSID, WIFI_PASS, WIFI_WAIT);
  WEB.begin();

#if _DEBUG_
  Serial.println ("Setup complete");
#endif
}

long last_sweep = 0;
void loop() {
  unsigned long now = millis();

  NET.loop();
  LED.loop();
  BUT.loop();
  if (last_sweep == 0 || (now - last_sweep) >= (1000.0 / double(SENSOR_HZ))) {
    last_sweep = now;
    RTC.loop();
    BMS.loop();
    BMP.loop();
    IMU.loop();
    GPS.loop();
    SRV.loop();

    LOG.loop();
  }
  BLE.loop();
  WEB.loop();
}
