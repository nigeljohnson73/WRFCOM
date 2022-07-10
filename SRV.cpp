#include "SRV.h"

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


Servo myservo;
TrSRV SRV;

TrSRV::TrSRV() {};

void TrSRV::begin() {
#if !USE_SERVO
  return;
#endif

#if ESP32
  // Allow allocation of all timers
  Serial.print("mysero.setPeriodHertz(50)");
  Serial.println();
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);
#endif

  myservo.attach(SERVO_PIN, MIN_VAL, MAX_VAL);
  //  myservo.write(90); // center for starters

  _enabled =  true;
  arm(false);

#if _DEBUG_
  //    Serial.println("SRV disconnected");
  Serial.print("SRV initialised: TBD");
  Serial.println();
#endif
}

//long last_srv = 0;
//bool deployed = false;

void TrSRV::loop() {
  //    long now = millis();
  //  //
  //  if (last_srv == 0) last_srv = now;
  //
  //  if ((now - last_srv) > 1000) {
  //    last_srv = now;
  //    //      if(deployed) myservo.write(0);
  //    //      else myservo.write(180);
  //    //      deployed = !deployed;
  //    arm(!isArmed());
  //  }
}

void TrSRV::arm(bool tf) {
  if (!isEnabled()) return;

#define stow_angle 30
  int ang = stow_angle + (tf ? (180 - stow_angle) : 0); // Armed should be 180. Stow angle so it doesn't bind
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
