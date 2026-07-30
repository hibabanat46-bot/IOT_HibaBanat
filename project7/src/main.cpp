#include <Arduino.h>

#include "Config.h"
#include "Motor.h"
#include "IRSensor.h"
#include "MQTTManager.h"

Motor motor;
IRSensor sensor;
MQTTManager mqtt;


void setup()
{
    Serial.begin(115200);

    motor.begin();

    sensor.begin();

    mqtt.begin();

    Serial.println("System Ready");
}

void loop()
{
    mqtt.loop();

    bool obstacle = sensor.obstacleDetected();

    mqtt.publishMotorState(!obstacle);

    if (millis() - mqtt.getLastPacketTime() > TIMEOUT)
    {
        motor.stop();
    }
    else if (mqtt.motorCommandReceived())
    {
        motor.start();
    }
    else
    {
        motor.stop();
    }

    delay(100);
}