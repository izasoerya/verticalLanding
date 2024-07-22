#include "bmp3XX.h"

BMP::BMP() : bmp(BMP388_DEV(Wire1)) {}

bool BMP::begin()
{
    bmp.begin(NORMAL_MODE, OVERSAMPLING_X16, OVERSAMPLING_X2, IIR_FILTER_4); // Default initialisation, place the BMP388 into SLEEP_MODE
    bmp.setTimeStandby(TIME_STANDBY_80MS);                                   // Set the standby time to 1.3 seconds
    bmp.startNormalConversion();
    return true;
}

float BMP::setBasePressure(float basePressure)
{
    bmp.setSeaLevelPressure(basePressure);
    return basePressure;
}

float BMP::getTemperature(float temperature)
{
    return bmp.getTemperature(temperature);
}

float BMP::getPressure(float pressure)
{
    return bmp.getPressure(pressure);
}

float BMP::getAltitude(float basePressure)
{
    return bmp.getAltitude(basePressure);
}

bool BMP::getSensorStatus(float &temperature, float &pressure, float &altitude)
{
    bmp.getMeasurements(temperature, pressure, altitude);
    return true;
}