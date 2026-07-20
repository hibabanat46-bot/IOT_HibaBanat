#include <Arduino.h>

const int IR_PIN = 4;
const int POT_PIN = 34;
const int LED_PIN = 25;
const int channel = 0;

void setup()
{
    Serial.begin(115200);

    pinMode(IR_PIN, INPUT);

    ledcSetup(channel, 5000, 8);
    ledcAttachPin(LED_PIN, channel);
}

void loop()
{
    int irState = digitalRead(IR_PIN);

    int potValue = analogRead(POT_PIN);

    float voltage = potValue * (3.3 / 4095.0);

    int pwmValue = map(potValue, 0, 4095, 0, 255);

    // IR بالعكس (Active LOW)
    if (irState == LOW)
    {
        ledcWrite(channel, pwmValue);
    }
    else
    {
        ledcWrite(channel, 0);
    }

    Serial.print("IR: ");
    Serial.print(irState);

    Serial.print(" | Pot: ");
    Serial.print(potValue);

    Serial.print(" | Voltage: ");
    Serial.print(voltage, 2);

    Serial.print(" V | PWM: ");
    Serial.println(pwmValue);

    delay(300);
}