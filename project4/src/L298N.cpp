#include <Arduino.h>
#include "L298N.h"

L298N::L298N(int in1, int in2, int ena)
{
    in1Pin = in1;
    in2Pin = in2;
    enaPin = ena;
}

void L298N::begin()
{
    pinMode(in1Pin, OUTPUT);
    pinMode(in2Pin, OUTPUT);

    ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);

    ledcAttachPin(enaPin, PWM_CHANNEL);

    stop();
}

void L298N::forward(int speed)
{
    digitalWrite(in1Pin, HIGH);
    digitalWrite(in2Pin, LOW);

    ledcWrite(PWM_CHANNEL, speed);
}

void L298N::reverse(int speed)
{
    digitalWrite(in1Pin, LOW);
    digitalWrite(in2Pin, HIGH);

    ledcWrite(PWM_CHANNEL, speed);
}

void L298N::stop()
{
    digitalWrite(in1Pin, LOW);
    digitalWrite(in2Pin, LOW);

    ledcWrite(PWM_CHANNEL, 0);
}

void L298N::brake()
{
    digitalWrite(in1Pin, HIGH);
    digitalWrite(in2Pin, HIGH);

    ledcWrite(PWM_CHANNEL, 255);
}