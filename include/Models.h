#ifndef MODELS_H
#define MODELS_H

enum RocketState
{
    initialization = 1,
    flying = 2,
    landing = 3,
};

struct FlightData
{
    float angleXBNO;
    float angleYBNO;
    float angleZBNO;
    float angleXMPU;
    float angleYMPU;
    float angleZMPU;
    float altitudeBMP;
    float temperatureBMP;
    float pressureBMP;
    float altitudeBME;
    float temperatureBME;
    float basePressure;
    float pressureBME;
    int counter = 0;
};

#endif