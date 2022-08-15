/**************
  SPIFFS doesnt work on the Feather S3 :(
  Use the "Huge App" for this in Partition Scheme, unless you really want OTA in which case use "Minimal SPIFFS"
*/

#include <Wire.h>
#include "App.h"

String espChipId() {
#ifdef ESP32
  //  int chipId = 0;
  //  for (int i = 0; i < 17; i = i + 8) {
  //
  //    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  //  }
  uint32_t chipId = 0;
  for (int i = 0; i < 17; i = i + 8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  char buffer[20];
  sprintf(buffer, "%04X", chipId);

  return String(buffer);

#else // ESP32
  return String(ESP.getChipId());
#endif // ESP32
}

void setup() {
  Wire.begin(); // Initialise the IIC bus
  Wire.setClock(400000); //Go super fast

  Serial.begin(115200);
  delay(5000);
  Serial.println();
#if _DEBUG_
  Serial.print("WRFCOM ");
  Serial.print(VERSION);
  Serial.print(": ");
  Serial.print(__DATE__);
  Serial.print(" @ ");
  Serial.print(__TIME__);
  Serial.println();
#endif

  BUT.begin(); // Switch it off early
  LED.begin();

  RTC.begin();
  EMU.begin();
  IMU.begin();
  GPS.begin();

  LOG.begin();
  SRV.begin();

  BMS.begin();
  BLE.begin();
  NOW.begin();
  NET.setHostname(DEVICE_NAME);
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
    EMU.loop();
    IMU.loop();
    GPS.loop();
    SRV.loop();

    LOG.loop();
  }
  BLE.loop();
  NOW.loop();
  WEB.loop();
}
