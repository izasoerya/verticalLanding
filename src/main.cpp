#include <Arduino.h>
#include <Scheduler.h>
#include "BNO.h"
#include "BME280.h"
#include "bmp3XX.h"
#include "Models.h"
#include "mpu9250sensor.h"

void dataBMEFunc();
void dataBNOFunc();
void dataBMPFunc();
void dataMPU9250Func();
void printDataFunc();
void thrusterControlFunc();
// void loraTransmitFunc(){};
// void loraReceiveFunc(){};
float calculateVelocity(float currentAltitude);

TaskScheduler dataBME(1, "dataBME", 80, dataBMEFunc);
TaskScheduler dataBMP(2, "dataBMP", 80, dataBMPFunc);
TaskScheduler dataBNO(3, "dataBNO", 10, dataBNOFunc);
TaskScheduler dataMPU(3, "dataMPU", 80, dataMPU9250Func);
TaskScheduler logData(3, "PrintData", 1000, printDataFunc);
TaskScheduler thrusterControl(3, "thrusterControl", 10, thrusterControlFunc);
// TaskScheduler loraTransmit(4, "loraTask", 1000, loraTransmitFunc);
// TaskScheduler loraReceive(5, "loraReceive", 1000, loraReceiveFunc);

BNO bno;
BME bme;
BMP bmp;
MPU9250Sensor mpu;
FlightData data;
FinalData finalData;
RocketState currentState;
float bmpBasePressure;

#define PWM1 22 // MAIN THRUSTER
#define PWM2 4  // roll thruster -
#define PWM3 7  // pitch thruster -
#define PWM4 6  // roll thruster +
#define PWM5 5  // pitch thruster +

float lastAltitude = 0;
unsigned long lastTime = 0;

void setup()
{
    Serial.begin(9600);
    bno.begin();
    bno.calibrate();

    mpu.begin();

    bmp.begin() ? Serial.println("BMP388 initialized") : Serial.println("BMP388 failed to initialize");
    delay(5000);
    float temperature, pressure, altitude;
    bmp.getSensorStatus(temperature, pressure, altitude);
    Serial.println(pressure); // debug
    bmp.setBasePressure(pressure);

    bme.begin();
    bme.getTemperature();
    Serial.println(bme.getPressure()); // debug
    bme.setCurrentPressure();
    bmpBasePressure = bme.getPressure() / 100.0F;
    currentState = initialization;
    pinMode(PWM1, OUTPUT);
    pinMode(PWM2, OUTPUT);
    pinMode(PWM3, OUTPUT);
    pinMode(PWM4, OUTPUT);
    pinMode(PWM5, OUTPUT);

    digitalWrite(PWM1, LOW);
    digitalWrite(PWM2, LOW);
    digitalWrite(PWM3, LOW);
    digitalWrite(PWM4, LOW);
    digitalWrite(PWM5, LOW);
    lastTime = millis();
}

void loop()
{
    // put your main code here, to run repeatedly:
    dataBME.runTask();
    dataBNO.runTask();
    dataBMP.runTask();
    dataMPU.runTask();
    logData.runTask();
    thrusterControl.runTask();

    if (currentState == initialization && finalData.altitude > 3)
        currentState = flying;

    finalData.roll = (data.angleXBNO);
    finalData.pitch = (data.angleYBNO);
    finalData.yaw = (data.angleZBNO);
    finalData.altitude = (44300 * (1 - pow(data.pressureBME / bmpBasePressure, (1 / 5.255)))) + 0.2;
    finalData.temperature = (data.temperatureBME + data.temperatureBMP) / 2;
    finalData.pressure = (data.pressureBME + data.pressureBMP) / 2;

    // Calculate velocity
    finalData.velocity = calculateVelocity(finalData.altitude);
}

void dataBMEFunc()
{
    data.temperatureBME = bme.getTemperature();
    data.pressureBME = bme.getPressure() / 100.0F;
    data.altitudeBME = bme.getAltitude();
}

void dataMPU9250Func()
{
    float roll, pitch, yaw;
    mpu.getData(roll, pitch, yaw);
    {
        data.angleXMPU = roll;
        data.angleYMPU = pitch;
        data.angleZMPU = yaw;
    }
}

void dataBNOFunc()
{
    bno.readSensor();
    data.angleXBNO = bno.getRoll();
    data.angleYBNO = bno.getPitch();
    data.angleZBNO = bno.getYaw();
}

void dataBMPFunc()
{
    // data.counter++;
    float temperature, pressure, altitude;
    bmp.getSensorStatus(temperature, pressure, altitude);
    if (temperature > 0)
    {
        data.temperatureBMP = temperature;
        data.pressureBMP = pressure;
        data.altitudeBMP = altitude;
    }
}

void printDataFunc()
{
    char buffer[256];
    snprintf(buffer, sizeof(buffer),
             "roll: %0.1f, pitch: %0.1f, yaw: %0.1f, altitude: %0.1f, velocity: %0.1f, temperature: %0.1f, pressure: %0.1f",
             finalData.roll, finalData.pitch, finalData.yaw, finalData.altitude, finalData.velocity, finalData.temperature, finalData.pressure);
    Serial.println(String(buffer));
}

void thrusterControlFunc()
{
    static bool rollPositiveThrusterOn = false;
    static bool rollNegativeThrusterOn = false;
    static bool pitchPositiveThrusterOn = false;
    static bool pitchNegativeThrusterOn = false;

    // Roll+ thruster control
    if (finalData.roll > 30)
    {
        if (!rollPositiveThrusterOn)
        {
            Serial.println("Roll+ thruster activated");
            digitalWrite(PWM4, HIGH);

            rollPositiveThrusterOn = true;
        }
    }
    else if (finalData.roll < 5)
    {
        if (rollPositiveThrusterOn)
        {
            Serial.println("Roll+ thruster deactivated");
            digitalWrite(PWM4, LOW);

            rollPositiveThrusterOn = false;
        }
    }

    // Roll- thruster control
    if (finalData.roll < -30)
    {
        if (!rollNegativeThrusterOn)
        {
            Serial.println("Roll- thruster activated");
            digitalWrite(PWM2, HIGH);
            rollNegativeThrusterOn = true;
        }
    }
    else if (finalData.roll > -5)
    {
        if (rollNegativeThrusterOn)
        {
            Serial.println("Roll- thruster deactivated");
            digitalWrite(PWM2, LOW);
            rollNegativeThrusterOn = false;
        }
    }

    // Pitch+ thruster control
    if (finalData.pitch > 30)
    {
        if (!pitchPositiveThrusterOn)
        {
            Serial.println("Pitch+ thruster activated");
            digitalWrite(PWM5, HIGH);
            pitchPositiveThrusterOn = true;
        }
    }
    else if (finalData.pitch < 5)
    {
        if (pitchPositiveThrusterOn)
        {
            Serial.println("Pitch+ thruster deactivated");
            digitalWrite(PWM5, LOW);
            pitchPositiveThrusterOn = false;
        }
    }

    // Pitch- thruster control
    if (finalData.pitch < -30)
    {
        if (!pitchNegativeThrusterOn)
        {
            Serial.println("Pitch- thruster activated");
            digitalWrite(PWM3, HIGH);
            pitchNegativeThrusterOn = true;
        }
    }
    else if (finalData.pitch > -5)
    {
        if (pitchNegativeThrusterOn)
        {
            Serial.println("Pitch- thruster deactivated");
            digitalWrite(PWM3, LOW);
            pitchNegativeThrusterOn = false;
        }
    }
}

float calculateVelocity(float currentAltitude)
{
    unsigned long currentTime = millis();
    float timeDiff = (currentTime - lastTime) / 80.0; // Convert to seconds
    float altitudeDiff = currentAltitude - lastAltitude;

    lastAltitude = currentAltitude;
    lastTime = currentTime;

    return altitudeDiff / timeDiff;
}
