#include "BUT.h"
#include <SparkFun_Qwiic_Button.h>

QwiicButton button;
TrBUT BUT;

TrBUT::TrBUT() {};

void TrBUT::begin() {
  if (button.begin() == false) {
//    Serial.println("Device did not acknowledge! Freezing.");
    Serial.println("BUT disconnected");
    return;
  }
  Serial.println("Button acknowledged.");
  button.LEDoff();  //start with the LED off

  #if _DEBUG_
  Serial.print("BUT initialised: 0x");
  Serial.print(button.getI2Caddress(), HEX);
  Serial.println();
#endif

  _enabled = true;
}

//bool armed = false;
//bool logging = false;

void TrBUT::loop() {
  unsigned long now = millis();
  bool state = button.isPressed();

  if (state == true) {
    if (_last_pressed_at > 0) {
      // possibly held?
      if (!_button_held && ((now - _last_pressed_at) > _held_trigger)) {
        _button_held = true;
        //        Serial.println("The button is held!");
        Serial.println("BUT::held()");
#if _DEBUG_
        Serial.println("SRV::arm(false)");
        Serial.println("LOG::capture(false)");
        //        armed = false;
        //        logging = false;
#endif
        if (SRV.isEnabled()) {
          SRV.arm(false);
        }

        if (LOG.isEnabled()) {
          LOG.capture(false);
        }

      }
    } else {
      Serial.println("The button is newly pressed!");
      _last_pressed_at = now;
    }
  } else {
    if (_last_pressed_at > 0) {
      //      Serial.println("The button is released!");
      // released
      if (!_button_held) {
        Serial.println("BUT::click()");
        if (!(SRV.isEnabled() && SRV.isArmed())) {
          //          armed = true;
          SRV.arm(true);
#if _DEBUG_
          Serial.println("SRV::arm(true)");
#endif
        } else {
          //          logging = !logging;
          if (LOG.isEnabled()) {
            LOG.capture(!LOG.isCapturing());
          }
#if _DEBUG_
          Serial.print("LOG::capture(");
          Serial.print((LOG.isEnabled() && LOG.isCapturing()) ? "true" : "false");
          Serial.print(")");
          Serial.println();
#endif
        }
      }
      _last_pressed_at = 0;
      _button_held = false;
    }
  }

  //check if button is pressed, and tell us if it is!
  //    if (button.isPressed() == true) {
  //      Serial.println("The button is pressed!");
  //      button.LEDconfig(brightness, cycleTime, offTime);
  //      while (button.isPressed() == true)
  //        delay(10);  //wait for user to stop pressing
  //      Serial.println("The button is not pressed.");
  //      button.LEDoff();
  //    }

  if (LOG.isCapturing()) {
    button.LEDon(_brightness);
  } else if (SRV.isArmed()) {
    button.LEDconfig(_brightness / 4, _cycleTime, _offTime);
  } else if (GPS.isConnected()) {
    button.LEDconfig(_brightness / 10, _cycleTime, _offTime);
  } else {
    button.LEDoff();
  }
}
