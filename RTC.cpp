#include "RTC.h"

#include <RTClib.h>

RTC_PCF8523 rtc;
TrRTC RTC;

TrRTC::TrRTC() {};

void TrRTC::begin() {
#if !_USE_RTC_
  return;
#endif

  if (!rtc.begin()) {
#if _DEBUG_
    Serial.println("RTC disconnected");
#endif
    return;
  }

  String mode = "Standalone";
  //  String network_time = NET.getTimestamp();
  // Lets see if network time in avaiable
  if (setTimestamp(NET.getTimestamp())) {
    //    char network_time_ca[21];
    //    network_time.toCharArray(network_time_ca, 21);

    //    rtc.adjust(DateTime(network_time_ca));
    //    rtc.adjust(DateTime(network_time.c_str()));
    //    setTimestamp(network_time);
    mode = "NTP";
    if (_XDEBUG_) {
      Serial.println("RTC: synchronised from network time");
    }

  } else {
    if (_XDEBUG_) {
      Serial.println("RTC: network time not available");
    }
    if (! rtc.initialized() || rtc.lostPower()) {
      Serial.println("RTC NOT initialized - defautling to compile o'clock");
      mode = "CT";
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    } else {
      if (_XDEBUG_) {
        Serial.println("RTC: standalone mode");
      }
    }
  }
  _enabled = true;

  rtc.start();
#if _DEBUG_
  Serial.println("RTC initialised: " + getTimestamp() + " (" + mode + ")");
#endif
}

void TrRTC::loop() {
}

bool TrRTC::setTimestamp(String iso) {
  if (!isEnabled()) return false;
  if (iso.length() == 0) return false;

  rtc.adjust(DateTime(iso.c_str()));
  return true;
}

String TrRTC::getTimestamp() {
  if (!isEnabled()) return "";

  DateTime now = rtc.now();
  return now.timestamp() + "Z";
}
