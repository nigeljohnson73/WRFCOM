#include "NOW.h"
TrNOW NOW;
TrNOW::TrNOW() {};

#if !_USE_NOW_

void TrNOW::begin() {
#if _DEBUG_ && _DISABLED_DEBUG_
  Serial.println("NOW init: disabled");
#endif // _DEBUG_
}

void TrNOW::loop() {}
bool TrNOW::isConnected() {
  return false;
};



#else //_USE_NOW_
#include "NOW_SVC.h"

now_status_packet status_packet;
now_command_packet command_packet;
bool now_connected = false;

bool TrNOW::isConnected() {
  return now_connected;
};

void TrNOW::begin() {
  Serial.print("NOW init: ");
  Serial.print(sizeof(status_packet));
  Serial.print(" byte status packets, ");
  Serial.print(sizeof(command_packet));
  Serial.print(" byte command packets ");
  Serial.println();

  _enabled = true;
}

void TrNOW::loop() {
}


#endif // !_USE_NOW_
