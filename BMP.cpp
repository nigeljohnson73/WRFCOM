#include "BMP.h"

#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

Adafruit_BMP3XX bmp390;
TrBMP BMP;

TrBMP::TrBMP() {};

void TrBMP::begin() {
  if (!bmp390.begin_I2C()) {   // hardware I2C mode, can pass in address & alt Wire
#if _DEBUG_
    Serial.println("BMP disconnected");
#endif
    return;
  }

  _enabled = true;
  // Set up oversampling and filter initialization
  bmp390.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp390.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp390.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp390.setOutputDataRate(BMP3_ODR_50_HZ);

  for (int x = 0; x < 5; x++) {
    bmp390.performReading();
    delay(50);
  }
  if (! bmp390.performReading()) {
    //Serial.println("BMP390: Failed to perform reading :(");
    return;
  }

#if _DEBUG_
  Serial.print("BMP initialised: BMP390, ");
  Serial.print(getPressure());
  Serial.print(" hPa, ");
  Serial.print(getTemperature());
  Serial.print(" C");
  Serial.println();
#endif

  //  setAltitude(22);
}

void TrBMP::loop() {
  if (!isEnabled()) return;

  if (! bmp390.performReading()) {
    //Serial.println("Failed to perform reading :(");
    return;
  }
}

void TrBMP::setAltitude(double h) {
  if (!isEnabled()) {
    return;
  }
  double p = getPressure();
  double t = getTemperature();

  // https://keisan.casio.com/keisan/image/Convertpressure.pdf
  double c = 0.0065 * h;
  double p0 = p * pow(1 - (c / (t + 273.15 + c)), -5.257);
  sea_level_pressure = p0;

#if _DEBUG_
  Serial.print("BMP390::setAltitude(");
  Serial.print(h);
  Serial.print(" m): BMP at sea-level = " );
  Serial.print(p0);
  Serial.print(" hPa");
  Serial.println();
#endif
}

double TrBMP::getPressure() {
  if (!isEnabled()) return 0.;
  return bmp390.pressure / 100.0;
}

double TrBMP::getTemperature() {
  if (!isEnabled()) return 0.;
  return bmp390.temperature;
}

double TrBMP::getAltitude() {
  if (!isEnabled()) return 0.;
  return bmp390.readAltitude(sea_level_pressure);
}
