#include "NET.h"

#if _USE_NTP_
#include <NTPClient.h>
#include <RTClib.h>  // For type inclusions
#endif

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
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#ifdef ESP32
int espChipId() {
  int chipId = 0;
  for (int i = 0; i < 17; i = i + 8) {

    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  return chipId;
}
#endif

TrNET NET;

#if _USE_NTP_
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", _NTP_OFFSET_SECONDS_);
#endif

TrNET::TrNET(): _hostname(_AP_NAME_), _ap_pass(_AP_PASSWORD_) {}

bool TrNET::isApMode() {
  return _ap_ssid.length() > 0;
}

String TrNET::getHostname() {
  return _hostname;
}

void TrNET::setHostname(String str) {
  _hostname = str;
}

void TrNET::begin(String ssid, String pass, long wait) {
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

    Serial.print("\n\nConnecting to network.");

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

#if _USE_NTP_
    /******************************************
      Initialise time server
    */
    timeClient.begin();
    timeClient.setTimeOffset(_NTP_OFFSET_SECONDS_);
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
    Serial.println(" not connected.\nStarting in AP mode.");
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
    ArduinoOTA.handle();
#if _USE_NTP_
    timeClient.update();
#endif
  }
}

String TrNET::getTimestamp() {
  if (isApMode()) return "";
#if _USE_NTP_
  DateTime now(timeClient.getEpochTime());
  return now.timestamp() + "Z";
#else
  return "";
#endif
}
