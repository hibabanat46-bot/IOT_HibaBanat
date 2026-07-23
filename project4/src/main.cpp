#include <Arduino.h>
#include "config.h"
#include "CurrentSensor.h"
#include "L298N.h"

L298N motor(IN1_PIN, IN2_PIN, ENA_PIN);

void setup()
{
    Serial.begin(115200);

    CurrentSensor_Init(CURRENT_SENSOR_PIN, 0.066);

    motor.begin();

    pinMode(ESTOP_PIN, INPUT_PULLUP);

    pinMode(RUN_LED_PIN, OUTPUT);
    pinMode(STOP_LED_PIN, OUTPUT);
    pinMode(HIGH_LED_PIN, OUTPUT);
    pinMode(LOW_LED_PIN, OUTPUT);

    digitalWrite(RUN_LED_PIN, LOW);
    digitalWrite(STOP_LED_PIN, HIGH);
    digitalWrite(HIGH_LED_PIN, LOW);
    digitalWrite(LOW_LED_PIN, LOW);
}


void loop()
{ 
     float current = CurrentSensor_ReadAmps();

    Serial.print("Current = ");
    Serial.print(current);
    Serial.println(" A");

  if (CurrentSensor_IsOvercurrent(CURRENT_LIMIT))
    {
        motor.brake();

        digitalWrite(HIGH_LED_PIN, HIGH);
        digitalWrite(LOW_LED_PIN, LOW);

        digitalWrite(RUN_LED_PIN, LOW);
        digitalWrite(STOP_LED_PIN, HIGH);

        return;
    }

    if (digitalRead(ESTOP_PIN) == LOW)
{
    motor.stop();

    digitalWrite(RUN_LED_PIN, LOW);
    digitalWrite(STOP_LED_PIN, HIGH);

    return;
}
        motor.forward(DEFAULT_SPEED);

          digitalWrite(RUN_LED_PIN, HIGH);
          digitalWrite(STOP_LED_PIN, LOW);


    if(current < CURRENT_LIMIT)
     {
        digitalWrite(LOW_LED_PIN, HIGH);
        digitalWrite(HIGH_LED_PIN, LOW);
    }
    else
    {
        digitalWrite(LOW_LED_PIN, LOW);
        digitalWrite(HIGH_LED_PIN, HIGH);
     }
         delay(100);


}
