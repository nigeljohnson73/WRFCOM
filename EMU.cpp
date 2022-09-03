#include "EMU.h"
TrEMU EMU;

/************************************************************************************************************************************************************
  888888ba   .88888.  888888ba   88888888b
  88    `8b d8'   `8b 88    `8b  88
  88     88 88     88 88     88 a88aaaa
  88     88 88     88 88     88  88
  88     88 Y8.   .8P 88     88  88
  dP     dP  `8888P'  dP     dP  88888888P
*/
#if EMU_TYPE == EMU_NONE

TrEMU::TrEMU() {}

void TrEMU::begin() {
#if _DEBUG_ && _DISABLED_DEBUG_
  Serial.println("EMU init: disabled");
#endif
}

void TrEMU::loop() {}
double TrEMU::getTemperature() {
  return 0;
}
double TrEMU::getPressure() {
  return 0;
}

#endif // EMU_TYPE == EMU_NONE

/************************************************************************************************************************************************************
   888888ba  8888ba.88ba   888888ba  d8888b. .d888b.  a8888a
   88    `8b 88  `8b  `8b  88    `8b     `88 Y8' `88 d8' ..8b
  a88aaaa8P' 88   88   88 a88aaaa8P'  aaad8' `8bad88 88 .P 88
   88   `8b. 88   88   88  88            `88     `88 88 d' 88
   88    .88 88   88   88  88            .88 d.  .88 Y8'' .8P
   88888888P dP   dP   dP  dP        d88888P `8888P   Y8888P
*/

#if EMU_TYPE == EMU_BMP390
// https://github.com/adafruit/Adafruit_BMP3XX/blob/master/examples/bmp3xx_simpletest/bmp3xx_simpletest.ino
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

Adafruit_BMP3XX bmp390;

TrEMU::TrEMU() {};

void TrEMU::begin() {
  if (!bmp390.begin_I2C()) {   // hardware I2C mode, can pass in address & alt Wire
#if _DEBUG_
    Serial.println("EMU init: BMP390: disconnected");
#endif
    return;
  }

  // Set up oversampling and filter initialization
  bmp390.setTemperatureOversampling(BMP3_OVERSAMPLING_4X);
  bmp390.setPressureOversampling(BMP3_OVERSAMPLING_8X);
  bmp390.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_7);
  bmp390.setOutputDataRate(BMP3_ODR_25_HZ);

  for (int x = 0; x < 5; x++) {
    bmp390.performReading();
    delay(50);
  }
  if (! bmp390.performReading()) {
    //Serial.println("BMP390: Failed to perform reading :(");
#if _DEBUG_
    Serial.println("EMU init: BMP390: failed");
#endif
    return;
  }

  _enabled = true;
  _has_temperature = true;
  _has_pressure = true;

#if _DEBUG_
  Serial.print("EMU init: BMP390, ");
  Serial.print("Tx4, Px8, IIR7, 25Hz");
  //  Serial.print(", ");
  //  Serial.print(getPressure());
  //  Serial.print(" hPa");
  Serial.println();
#endif
}

void TrEMU::loop() {
  if (!isEnabled()) return;

  if (! bmp390.performReading()) {
    return;
  }
}

double TrEMU::getPressure() {
  if (!isEnabled()) return 0.;
  return bmp390.pressure / 100.0;
}

double TrEMU::getTemperature() {
  if (!isEnabled()) return 0.;
  return bmp390.temperature;
}

#endif // EMU_TYPE == EMU_BMP390

/************************************************************************************************************************************************************
  888888ba   888888ba  .d88888b  d8888b. d88   a8888a
  88    `8b  88    `8b 88.    "'     `88  88  d8' ..8b
  88     88 a88aaaa8P' `Y88888b.  aaad8'  88  88 .P 88
  88     88  88              `8b     `88  88  88 d' 88
  88    .8P  88        d8'   .8P     .88  88  Y8'' .8P
  8888888P   dP         Y88888P  d88888P d88P  Y8888P
*/
#if EMU_TYPE == EMU_DSP310
// https://github.com/adafruit/Adafruit_DPS310/blob/master/examples/dps310_sensortest/dps310_sensortest.ino
#include <Adafruit_DPS310.h>

Adafruit_DPS310 dps;
Adafruit_Sensor *dps_temp = dps.getTemperatureSensor();
Adafruit_Sensor *dps_pressure = dps.getPressureSensor();
sensors_event_t temp_event, pressure_event;

TrEMU::TrEMU() {};

void TrEMU::begin() {
  if (! dps.begin_I2C()) {
#if _DEBUG_
    Serial.println("EMU init: DPS310: disconnected");
#endif
    return;
  }

  dps.setMode(DPS310_CONT_PRESTEMP);
  dps.configurePressure(DPS310_32HZ, DPS310_32SAMPLES);
  dps.configureTemperature(DPS310_32HZ, DPS310_32SAMPLES);

  //  for (int x = 0; x < 5; x++) {
  //    bmp390.performReading();
  //    delay(50);
  //  }
  //  if (! bmp390.performReading()) {
  //    //Serial.println("BMP390: Failed to perform reading :(");
  //#if _DEBUG_
  //    Serial.println("EMU init: DPS310: failed");
  //#endif
  //    return;
  //  }

  _enabled = true;
  _has_temperature = true;
  _has_pressure = true;

#if _DEBUG_
  Serial.print("EMU init: DPS310, Continuous, ");
  Serial.print("Tx32 @ 32Hz, Px32 @32Hz");
  //  Serial.print(", ");
  //  Serial.print(getPressure());
  //  Serial.print(" hPa");
  Serial.println();
#endif
}


void TrEMU::loop() {
  if (!isEnabled()) return;

  if (dps.temperatureAvailable()) {
    dps_temp->getEvent(&temp_event);
  }

  // Reading pressure also reads temp so don't check pressure
  // before temp!
  if (dps.pressureAvailable()) {
    dps_pressure->getEvent(&pressure_event);
  }
}

double TrEMU::getPressure() {
  if (!isEnabled()) return 0.;
  return pressure_event.pressure;
}

double TrEMU::getTemperature() {
  if (!isEnabled()) return 0.;
  return temp_event.temperature;
}

#endif // EMU_TYPE == EMU_BMP390

/************************************************************************************************************************************************************
                      dP    .d888888  dP   dP   oo   dP                  dP
                      88   d8'    88  88   88        88                  88
  .d8888b. .d8888b. d8888P 88aaaaa88a 88 d8888P dP d8888P dP    dP .d888b88 .d8888b.
  Y8ooooo. 88ooood8   88   88     88  88   88   88   88   88    88 88'  `88 88ooood8
        88 88.  ...   88   88     88  88   88   88   88   88.  .88 88.  .88 88.  ...
  `88888P' `88888P'   dP   88     88  dP   dP   dP   dP   `88888P' `88888P8 `88888P'
*/
void TrEMU::setAltitude(double h) {
  if (!isEnabled()) {
    return;
  }
  double p = getPressure();
  double T = _local_temperature;//getTemperature();

  // https://keisan.casio.com/keisan/image/Convertpressure.pdf
  double c = 0.0065 * h;
  double p0 = p * pow(1 - (c / (T + 273.15 + c)), -5.257);
  _sea_level_pressure = p0;

#if _DEBUG_
  Serial.print("EMU::setAltitude(");
  Serial.print(h);
  Serial.print(" m): Pressure at sea-level = " );
  Serial.print(p0);
  Serial.print(" hPa");
  Serial.println();
#endif
}

/************************************************************************************************************************************************************
                      dP    .d888888  dP   dP   oo   dP                  dP
                      88   d8'    88  88   88        88                  88
  .d8888b. .d8888b. d8888P 88aaaaa88a 88 d8888P dP d8888P dP    dP .d888b88 .d8888b.
  88'  `88 88ooood8   88   88     88  88   88   88   88   88    88 88'  `88 88ooood8
  88.  .88 88.  ...   88   88     88  88   88   88   88   88.  .88 88.  .88 88.  ...
  `8888P88 `88888P'   dP   88     88  dP   dP   dP   dP   `88888P' `88888P8 `88888P'
       .88
   d8888P
*/
double TrEMU::getAltitude() {
  if (!isEnabled()) return 0.;

  // hypsometric formula from here: https://keisan.casio.com/has10/SpecExec.cgi?id=system/2006/1224585971
  double p0 = _sea_level_pressure;
  double p = getPressure();
  double T = _local_temperature;
  double h = ((pow(p0 / p, 1.0 / 5.257) - 1) * (T + 273.15)) / 0.0065;

  return h;
}
