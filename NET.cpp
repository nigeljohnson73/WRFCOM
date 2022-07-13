#include "NET.h"
TrNET NET;
TrNET::TrNET(): _hostname(AP_NAME), _ap_pass(AP_PASSWORD) {}

#if !_USE_WIFI_

void TrNET::begin(String ssid, String pass, long wait_secs) {}
void TrNET::loop() {}
void TrNET::setHostname(String name) {}
String TrNET::getTimestamp() {
  return "";
}

#else

#ifdef ESP32
#include <WiFi.h>
#include <ESPmDNS.h>
// TODO: Make some more shit work here
#else
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#endif

#if _USE_OTA_
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#endif

#if _USE_NTP_
#include <NTPClient.h>
#include <RTClib.h>  // For type inclusions
#endif

#ifdef ESP32
int espChipId() {
  int chipId = 0;
  for (int i = 0; i < 17; i = i + 8) {

    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  return chipId;
}
#endif

#if  _USE_NTP_
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", NTP_OFFSET_SECONDS);
#endif

void TrNET::setHostname(String str) {
  _hostname = str;
}

void TrNET::begin(String ssid, String pass, long wait) {
#if !_USE_WIFI_
  return;
#endif

  if (wait < 1 || wait > 300) {
    wait = 30;
  }

  if (_hostname == String("")) {
#ifdef ESP32
    _hostname = String ("TR-") + espChipId();
#else
    _hostname = String ("TR-") + ESP.getChipId();
#endif
  }

  if (ssid.length() > 0) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());

    WiFi.hostname(_hostname);

    Serial.print("Connecting to WiFi network '");
    Serial.print(WIFI_SSID);
    Serial.print("'.");

    unsigned long connect_millis = millis() + (wait * 1000);
    while ((WiFi.status() != WL_CONNECTED) && (millis() < connect_millis)) {
      Serial.print(".");
      delay(500);
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" connected!");
    _ip_address = WiFi.localIP().toString();
#ifdef ESP32
    _hostname = WiFi.getHostname();
#else
    _hostname = WiFi.hostname().c_str();
#endif
    //Display.showIpAddress(_ip_address);

#if _DEBUG_
    Serial.println(String("      WiFi SSID: ") + ssid);
    Serial.println(String("     IP address: ") + _ip_address);
#endif
    /******************************************
         Initialise mDNS
    */
    if (MDNS.begin(_hostname.c_str())) {
#if _DEBUG_
      Serial.println(String("   DNS hostname: ") + _hostname + ".local");
#endif
    }

#if _USE_OTA_
    /******************************************
       Initialise OTA software update
    */
    // Port defaults to 8266
    // ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    // ArduinoOTA.setHostname("myesp8266");
    ArduinoOTA.setHostname(_hostname.c_str());
    
#if _DEBUG_
    Serial.println("   OTA hostname: " + String(ArduinoOTA.getHostname()));
#endif

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else { // U_FS
        type = "filesystem";
      }

      // NOTE: if updating FS this would be the place to unmount FS using FS.end()
      Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      }
    });
    ArduinoOTA.begin();
#endif

#if _USE_NTP_
    /******************************************
      Initialise time server
    */
    timeClient.begin();
    timeClient.setTimeOffset(NTP_OFFSET_SECONDS);
    while (!timeClient.update()) {
      timeClient.forceUpdate();
    }
#if _DEBUG_
    Serial.println(String("NTP initialised: ") + getTimestamp());
#endif
    //#else
    //    Serial.println(String("   NTP disabled");
#endif

  } else {
    /******************************************
      Start up in local mode
    */
    if (ssid.length() > 0) Serial.println(" not connected.");
    Serial.println("Starting WiFi in AP mode.");
    _ap_ssid = _hostname;
    _hostname = "";

    //    IPAddress local_ip(192, 168, 1, 1);
    //    IPAddress gateway(192, 168, 1, 1);
    //    IPAddress subnet(255, 255, 255, 0);

    //WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP(_ap_ssid.c_str(), _ap_pass.c_str());
    delay(100);

    //    _ip_address = local_ip.toString();
    IPAddress _ip =  WiFi.softAPIP();
    _ip_address = _ip.toString();
    Serial.println(String("        AP SSID: ") + _ap_ssid);
    Serial.println(String("     IP address: ") + _ip_address);
  }
}

void TrNET::loop() {
  if (!isApMode()) {
#if _USE_OTA_
    ArduinoOTA.handle();
#endif
#if _USE_NTP_
    timeClient.update();
#endif
  }
}

String TrNET::getTimestamp() {
  if (!isEnabled()) return "";
  if (isApMode()) return "";

#if _USE_NTP_
  DateTime now(timeClient.getEpochTime());
  return now.timestamp() + "Z";
#else
  return "";
#endif
}

#endif //_USE_WIFI_
