#ifndef BMP388s_H
#define BMP388s_H

#include <BMP388_DEV.h>

class BMP
{
private:
  BMP388_DEV bmp;
  struct kalmanVar
  {
    float Estimate = 0;
    float ErrorEstimate = 0.3;
    float Gain;
    float Measurement;
  };
  float ePressure;

public:
  BMP();
  bool begin();
  float setBasePressure(float basePressure);
  float getTemperature(float temperature);
  float getPressure(float pressure);
  float getAltitude(float basePressure);
  bool getSensorStatus(float &temperature, float &pressure, float &altitude);
};

#endif