#include <Arduino.h>
#define LED 2


void setup() {
 Serial.begin (115200);
 delay(1000);
 pinMode(LED,OUTPUT);

 Serial.println("START");
}

void loop() {

   digitalWrite(LED, HIGH);
   Serial.println("LED ON");
    delay(1000);

    digitalWrite(LED, LOW);
    Serial.println("LED OFF");
     delay(1000);

}

