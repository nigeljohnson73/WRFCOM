#include "LED.h"

#include <Adafruit_NeoPixel.h>

#if ESP32

#ifndef LED_PIN
#define LED_PIN 13
#endif

#ifndef NEO_PIN
#define NEO_PIN PIN_NEOPIXEL
#endif

#else

#ifndef LED_PIN
#define LED_PIN LED_BUILTIN
#define NEO_PIN 6
#endif

#endif


Adafruit_NeoPixel NEO(1, NEO_PIN, NEO_GRB + NEO_KHZ800);
TrLED LED;

void logit(String str) {
  //Serial.println(str);
}
void logit(int str) {
  logit(String(str));
}
//void logit(unsigned long str) {
//	logit(String(str));
//}

TrLED::TrLED(): _blip_duration(_blip_on_duration + _blip_off_duration)
{};

void TrLED::begin() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  NEO.begin();
  NEO.clear();

#if _DEBUG_
  Serial.print("LED initialised: OK");
  Serial.println();
#endif
  _enabled = true;
}

void TrLED::loop() {
  if (!isEnabled()) {
    return;
  }

  if (GPS.isEnabled()) {
    if (GPS.isConnected()) {
      NEO.setPixelColor(0, NEO.Color(0x00, 0x00, 0xff));
    } else {
      NEO.setPixelColor(0, NEO.Color(0x99, 0x33, 0x00));
    }
  } else {
    NEO.setPixelColor(0, NEO.Color(0x66, 0x00, 0x00));
  }
  NEO.show();

  if (LOG.isCapturing()) {
    _blip = true;
  } else {
    unsigned long now = millis();

    if ((now - _sequence_started) < _sequence_duration) {
      // So we are in a sequence
      if (_n_blips > 0) {
        // we should be blipping
        if ((now - _blip_started) <= _blip_on_duration) {
          // In the honemoon period, light 'er up
          _blip = true;
        } else {
          // regardless of whether we should preparing another pone or just in the off period, still lights out
          _blip = false;
          if ((now - _blip_started) > (_blip_on_duration + _blip_off_duration)) {
            // we are past when the blip should finish, lets do another one... if needed
            if (_n_blips > 0) {
              _n_blips -= 1;
              _blip_started = now;
            }
          }
        }
      }
    } else {
      // Start a new sequence with the right blinky parameters
      if (NET.isApMode()) {
        blink(2);
      } else {
        if (NET.isEnabled()) {
          blink(1);
        } else if (BLE.isEnabled()) {
          blink(3);
        }
      }
      _sequence_started = now;
    }
  }
  digitalWrite(LED_PIN, _blip ? HIGH : LOW);
}
