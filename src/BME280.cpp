#include "BME280.h"

BME::BME() : bme(BME280())
{
  bme.setI2CAddress(0x76);
}

void BME::begin()
{
  if (bme.beginI2C(Wire1) == false) // Begin communication over I2C
  {
    Serial.println("The sensor did not respond. Please check wiring.");
  }
  bme.setFilter(3);
  bme.setHumidityOverSample(0);
  bme.setPressureOverSample(3);
}

float BME::getTemperature()
{
  return bme.readTempC();
}

float BME::getPressure()
{
  return bme.readFloatPressure();
}

void BME::setCurrentPressure()
{
  int i = 0;
  while (i < 10)
  {
    bme.setReferencePressure(bme.readFloatPressure());
    i++;
  }
}

float BME::getAltitude()
{
  return bme.readFloatAltitudeMeters();
}