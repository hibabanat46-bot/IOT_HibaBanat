#include <Arduino.h>
#include "IRSensor.h"
#include "Config.h"

void IRSensor::begin()
{
    pinMode(IR_PIN,INPUT);
}

bool IRSensor::obstacleDetected()
{
    return digitalRead(IR_PIN)==LOW;
}