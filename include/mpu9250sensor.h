#ifndef MPU9250s_H
#define MPU9250s_H

#include <Wire.h>
#include <MPU9250_WE.h>

class MPU9250Sensor
{
public:
    MPU9250Sensor();
    bool begin();
    void getData(float &roll, float &pitch, float &yaw);

private:
    MPU9250_WE imu;
};

#endif // MPU9250_H