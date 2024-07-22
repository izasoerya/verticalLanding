#include "transmitter.h"

LoraDevice::LoraDevice() {}
LoraDevice::~LoraDevice() {}

String LoraDevice::constructMessage(FlightData data)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer),
             "%.2f,%.2f,%.2f,"
             "%.2f",
             data.angleXBNO, data.angleYBNO, data.angleZBNO,
             data.altitudeBME);
    return buffer;
}

void LoraDevice::sendMessage(HardwareSerial &Lora, String message)
{
    Lora.println(message);
}