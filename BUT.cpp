#include "BUT.h"
TrBUT BUT;
TrBUT::TrBUT() {};

#if !_USE_BUTTON_

void TrBUT::begin() {};
void TrBUT::loop() {};

#else
#include <SparkFun_Qwiic_Button.h>

QwiicButton button;

void TrBUT::begin() {

  if (button.begin() == false) {

#if _DEBUG_
    Serial.println("BUT initialised: disconnected");
#endif // _DEBUG_

    return;
  }
  button.LEDoff();  //start with the LED off

#if _DEBUG_
  Serial.print("BUT initialised: 0x");
  Serial.print(button.getI2Caddress(), HEX);
  Serial.println();
#endif // _DEBUG_

  _enabled = true;
}

void TrBUT::loop() {
  unsigned long now = millis();
  bool state = button.isPressed();

  if (state == true) {
    if (_last_pressed_at > 0) {
      if (!_button_held && ((now - _last_pressed_at) > _held_trigger)) {
        _button_held = true;
#if _DEBUG_ && _XDEBUG_
        Serial.println("BUT::held()");
        Serial.println("SRV::arm(false)");
        Serial.println("LOG::capture(false)");
#endif // _DEBUG_ && _XDEBUG_

        if (SRV.isEnabled()) {
          SRV.arm(false);
        }

        if (LOG.isEnabled()) {
          LOG.capture(false);
        }

      }
    } else {
#if _DEBUG_ && _XDEBUG_
      Serial.println("The button is newly pressed!");
#endif // _DEBUG_ && _XDEBUG_

      _last_pressed_at = now;
    }
  } else {
    if (_last_pressed_at > 0) {
      //      Serial.println("The button is released!");
      // released
      if (!_button_held) {

#if _DEBUG_ && _XDEBUG_
        Serial.println("BUT::click()");
#endif // _DEBUG_ && _XDEBUG_

        if (!(SRV.isEnabled() && SRV.isArmed())) {
          SRV.arm(true);

#if _DEBUG_ && _XDEBUG_
          Serial.println("SRV::arm(true)");
#endif // _DEBUG_ && _XDEBUG_

        } else {
          //          logging = !logging;
          if (LOG.isEnabled()) {
            LOG.capture(!LOG.isCapturing());
          }
#if _DEBUG_ && _XDEBUG_
          Serial.print("LOG::capture(");
          Serial.print((LOG.isEnabled() && LOG.isCapturing()) ? "true" : "false");
          Serial.print(")");
          Serial.println();
#endif // _DEBUG_ && _XDEBUG_

        }
      }
      _last_pressed_at = 0;
      _button_held = false;
    }
  }

  if (LOG.isCapturing()) {
    button.LEDon(_brightness);
  } else if (SRV.isArmed()) {
    button.LEDconfig(_brightness / 10, _cycleTime, _offTime);
  } else if (GPS.isConnected()) {
    button.LEDconfig(_brightness / 40, _cycleTime * 2, _offTime);
  } else {
    button.LEDoff();
  }
}

#endif // _USE_BUTTON_
