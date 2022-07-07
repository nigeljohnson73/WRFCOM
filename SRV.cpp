#include "SRV.h"

#ifdef ESP32
#include <ESP32Servo.h>
#else
#include <Servo.h>
#endif

Servo myservo;
TrSRV SRV;

TrSRV::TrSRV() {};

void TrSRV::begin() {
#if !USE_SERVO
  return;
#endif
  myservo.attach(SERVO_PIN);
  _enabled =  true;
  arm(false);

  //  myservo.write(30);
#if _DEBUG_
  //    Serial.println("SRV disconnected");
  Serial.print("SRV initialised: DISARMED");
  Serial.println();
#endif
}

//long last_srv = 0;
void TrSRV::loop() {
  //  long now = millis();
  //
  //  if ((last_srv == 0) || ((now - last_srv) > 5000)) {
  //    last_srv = now;
  //    arm(!isArmed());
  //  }
}

void TrSRV::arm(bool tf) {
  if (!isEnabled()) return;

  int ang = 30 + (tf ? 120 : 0);
#if _DEBUG_ && _XDEBUG_
  Serial.print("SRV::arm(");
  Serial.print(tf ? "true" : "false");
  Serial.print("): called - ang: ");
  Serial.print(ang);
  Serial.println();
#endif

  myservo.write(ang);
  _armed = tf;
}
