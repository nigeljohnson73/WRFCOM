#include "BAT.h"

//#include <BATlib.h>

//BAT_PCF8523 rtc;
TrBAT BAT;

TrBAT::TrBAT() {};

void TrBAT::begin() {
#if _DEBUG_
    Serial.println("BAT disconnected");
#endif
}

void TrBAT::loop() {
}

double TrBAT::getFill() {
	if(!isEnabled()) return 0.;

	return 0.;
}
