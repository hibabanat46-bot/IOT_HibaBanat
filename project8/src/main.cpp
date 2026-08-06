#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define ENA 25
#define IN1 26
#define IN2 27

int motorSpeed = 255;

const char* ssid = "CYBER_EXT";
const char* password = "cyberap2025";

const char* mqtt_server = "192.168.1.104";

WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_BME280 bme;

void reconnect()
{
    while (!client.connected())
    {
        Serial.print("Connecting MQTT...");

        if (client.connect("ESP32_Motor"))
        {
            Serial.println(" Connected");

            client.subscribe("home/esp32/motor/power");
            Serial.println("Subscribed to home/esp32/motor/power");
            client.subscribe("home/esp32/motor/direction");
            client.subscribe("home/esp32/motor/speed");
        }
        else
        {
            Serial.print(" Failed: ");
            Serial.println(client.state());
            delay(2000);
        }
    }
}



String motorDirection = "FORWARD";
bool motorState = true;



void callback(char* topic, byte* payload, unsigned int length)
{
    String message = "";

    for (int i = 0; i < length; i++)
    {
        message += (char)payload[i];
    }

    Serial.print("Topic: ");
    Serial.println(topic);

    Serial.print("Message: ");
    Serial.println(message);

    if (String(topic) == "home/esp32/motor/power")
    {
        if (message == "ON")
            motorState = true;

        if (message == "OFF")
            motorState = false;
    }

    if (String(topic) == "home/esp32/motor/direction")
{
    if (message == "FORWARD")
    {
        motorDirection = "FORWARD";
    }
    else if (message == "REVERSE")
    {
        motorDirection = "REVERSE";
    }

    Serial.print("Direction = ");
    Serial.println(motorDirection);
}

if (String(topic) == "home/esp32/motor/speed")
{
    motorSpeed = message.toInt();

    motorSpeed = constrain(motorSpeed, 0, 100);

    motorSpeed = map(motorSpeed, 0, 100, 0, 255);

    Serial.print("Speed = ");
    Serial.println(motorSpeed);
}


}

void setup()
{
    Serial.begin(115200);

    Wire.begin();

if (!bme.begin(0x76))
{
    Serial.println("BME280 not found!");
    while (1);
}

Serial.println("BME280 Ready");

    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    ledcWrite(0, motorSpeed);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    ledcSetup(0, 5000, 8);
    ledcAttachPin(ENA, 0);

    Serial.print("Connecting WiFi");

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi Connected");

    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void loop()
{
    if(!client.connected())
    {
        reconnect();
    }

    client.loop();

static unsigned long lastPublish = 0;

if (millis() - lastPublish >= 5000)
{
    lastPublish = millis();

    float temperature = bme.readTemperature();
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure() / 100.0;

    client.publish(
        "home/esp32/sensor/temperature",
        String(temperature,1).c_str()
    );

    client.publish(
        "home/esp32/sensor/humidity",
        String(humidity,1).c_str()
    );

    client.publish(
        "home/esp32/sensor/pressure",
        String(pressure,1).c_str()
    );

    Serial.println("Sensor Data Published");
    Serial.print("Temperature: ");
    Serial.println(bme.readTemperature());

    Serial.print("Humidity: ");
    Serial.println(bme.readHumidity());

    Serial.print("Pressure: ");
    Serial.println(bme.readPressure() / 100.0);

Serial.println("----------------------");
}


    if (motorState)
{
    ledcWrite(0, motorSpeed);

    if (motorDirection == "FORWARD")
    {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
    }
    else
    {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
    }
}
else
{
    ledcWrite(0, 0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
}
}