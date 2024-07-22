#include "mpu9250sensor.h"

MPU9250Sensor::MPU9250Sensor() : imu(MPU9250_WE(0x68)) {}

bool MPU9250Sensor::begin()
{
    // Implementation of begin() function
    Wire.begin();
    if (!imu.init())
    {
        Serial.println("MPU9250 does not respond");
    }
    else
    {
        Serial.println("MPU9250 is connected");
    }
    if (!imu.initMagnetometer())
    {
        Serial.println("Magnetometer does not respond");
    }
    Serial.println("Position you MPU9250 flat and don't move it - calibrating...");
    delay(1000);
    imu.autoOffsets();
    Serial.println("Done!");

    imu.enableGyrDLPF();
    imu.setGyrDLPF(MPU9250_DLPF_6);
    imu.setSampleRateDivider(5);
    imu.setGyrRange(MPU9250_GYRO_RANGE_250);
    imu.setAccRange(MPU9250_ACC_RANGE_2G);
    imu.enableAccDLPF(true);
    imu.setAccDLPF(MPU9250_DLPF_6);
    imu.setMagOpMode(AK8963_CONT_MODE_100HZ);
    delay(1000);
    return true;
}

void MPU9250Sensor::getData(float &roll, float &pitch, float &yaw)
{
    // Implementation of getData() function
    xyzFloat magvalue = imu.getMagValues();
    roll = imu.getPitch() * -1;
    pitch = imu.getRoll();

    // Assuming imu has methods to get magnetometer data
    float magX = magvalue.x;
    float magY = magvalue.y;

    // Calculate yaw
    yaw = atan2(magY, magX) * 180 / PI; // Convert radians to degrees
}