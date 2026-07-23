#include <Arduino.h>
#include "CurrentSensor.h"

// Private variables
static int sensorPin;
static float sensorSensitivity;

void CurrentSensor_Init(int pin, float sensitivity)
{
    sensorPin = pin;
    sensorSensitivity = sensitivity;

    pinMode(sensorPin, INPUT);
}



float CurrentSensor_ReadAmps()
{
    int adcValue = analogRead(sensorPin);

    float voltage = (adcValue * 3.3) / 4095.0;

    const float OFFSET = 2.42;

float current = (voltage - OFFSET) / sensorSensitivity;

    return current;
}



bool CurrentSensor_IsOvercurrent(float threshold)
{
    return (CurrentSensor_ReadAmps() > threshold);
}