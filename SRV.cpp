#include "SRV.h"

TrSRV SRV;
TrSRV::TrSRV() {};

#if !_USE_SERVO_

void TrSRV::begin() {}
void TrSRV::loop() {}
void TrSRV::arm(bool tf) {}

#else

#if ESP32
#ifndef SERVO_PIN
#define SERVO_PIN 12
#endif

#else

#ifndef SERVO_PIN
#define SERVO_PIN D4
#endif

#endif

#ifdef ESP32
#include <ESP32Servo.h>
#else
#include <Servo.h>
#endif

#define MIN_VAL 700
#define MAX_VAL 2300
#define STOW_ANGLE 110


Servo myservo;

void TrSRV::begin() {

#if ESP32
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);
#endif

  if (!myservo.attach(SERVO_PIN, MIN_VAL, MAX_VAL)) {
    Serial.println("SRV disconnected");
  }

  _enabled =  true;
  arm(true);

#if _DEBUG_
  Serial.print("SRV initialised: Stow angle - ");
  Serial.print(STOW_ANGLE);
  Serial.print(" degrees");
  Serial.println();
#endif
}

void TrSRV::loop() {
}

void TrSRV::arm(bool tf) {
  if (!isEnabled()) return;

  int ang = STOW_ANGLE + (tf ? (180 - STOW_ANGLE) : 0); // Armed should be 180. Stow angle so it doesn't bind
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

#endif //_USE_SERVO_
