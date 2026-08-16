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

const char* mqtt_server = "192.168.1.105";

const char* TOPIC_TEMP      = "home/esp32/sensor/temperature";
const char* TOPIC_HUM       = "home/esp32/sensor/humidity";
const char* TOPIC_PRESS     = "home/esp32/sensor/pressure";

const char* TOPIC_POWER     = "home/esp32/motor/power";
const char* TOPIC_DIRECTION = "home/esp32/motor/direction";
const char* TOPIC_SPEED     = "home/esp32/motor/speed";
const char* TOPIC_EMERGENCY = "home/esp32/motor/emergency";

WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_BME280 bme;

void reconnect()
{
    while (!client.connected())
    {
        Serial.print("Connecting MQTT...");

        if (client.connect(
                "ESP32_Motor",
                nullptr,
                nullptr,
                "home/esp32/status",
                1,
                true,
                "OFFLINE"))
        {
            Serial.println(" Connected");

            // Publish online status
            client.publish("home/esp32/status", "ONLINE", true);

            // Subscribe to motor control topics
            client.subscribe(TOPIC_POWER);
            client.subscribe(TOPIC_DIRECTION);
            client.subscribe(TOPIC_SPEED);
            client.subscribe(TOPIC_EMERGENCY);

            Serial.println("Subscribed to all motor topics");
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

    if (String(topic) == TOPIC_POWER)
    {
        if (message == "ON")
            motorState = true;
        else if (message == "OFF")
            motorState = false;
    }

    else if (String(topic) == TOPIC_DIRECTION)
    {
        if (message == "FORWARD")
            motorDirection = "FORWARD";
        else if (message == "REVERSE")
            motorDirection = "REVERSE";

        Serial.print("Direction = ");
        Serial.println(motorDirection);
    }

    else if (String(topic) == TOPIC_SPEED)
    {
        motorSpeed = constrain(message.toInt(), 0, 100);
        motorSpeed = map(motorSpeed, 0, 100, 0, 255);

        Serial.print("Speed = ");
        Serial.println(map(motorSpeed, 0, 255, 0, 100));
    }
    else if (String(topic) == TOPIC_EMERGENCY)
{
    if (message == "STOP")
    {
        motorState = false;

        Serial.println("***** EMERGENCY STOP *****");
    }
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

    ledcSetup(0, 5000, 8);
    ledcAttachPin(ENA, 0);

    ledcWrite(0, motorSpeed);

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    Serial.print("Connecting WiFi");

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi Connected");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void loop()
{
    if (!client.connected())
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

        // Publish sensor data
        bool t = client.publish(TOPIC_TEMP, String(temperature, 1).c_str(), true);
bool h = client.publish(TOPIC_HUM, String(humidity, 1).c_str(), true);
bool p = client.publish(TOPIC_PRESS, String(pressure, 1).c_str(), true);

Serial.print("Temp publish = ");
Serial.println(t);

Serial.print("Hum publish = ");
Serial.println(h);

Serial.print("Press publish = ");
Serial.println(p);
        // Publish actual motor status
        client.publish(
            "home/esp32/status/power",
            motorState ? "ON" : "OFF",
            true);

        client.publish(
            "home/esp32/status/direction",
            motorDirection.c_str(),
            true);

        client.publish(
            "home/esp32/status/speed",
            String(map(motorSpeed, 0, 255, 0, 100)).c_str(),
            true);
            client.publish("home/esp32/status/speed","0",  true
);

        Serial.println("Sensor Data Published");

        Serial.print("Temperature: ");
        Serial.println(temperature);

        Serial.print("Humidity: ");
        Serial.println(humidity);

        Serial.print("Pressure: ");
        Serial.println(pressure);

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