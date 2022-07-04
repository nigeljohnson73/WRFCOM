#include "SRV.h"

//#include <SRVlib.h>

//SRV_PCF8523 rtc;
TrSRV SRV;

TrSRV::TrSRV() {};

void TrSRV::begin() {
#if _DEBUG_
    Serial.println("SRV disconnected");
#endif
}

void TrSRV::loop() {
}
