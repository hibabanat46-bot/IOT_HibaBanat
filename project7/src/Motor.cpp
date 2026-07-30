#include <Arduino.h>
#include "Motor.h"
#include "Config.h"

void Motor::begin()
{
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(ENA, PWM_CHANNEL);

    stop();
}

void Motor::start()
{
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);

    ledcWrite(PWM_CHANNEL, MOTOR_SPEED);

}

void Motor::stop()
{
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,LOW);

    ledcWrite(PWM_CHANNEL, 0);
}
