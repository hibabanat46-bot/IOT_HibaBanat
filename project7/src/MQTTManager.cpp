#include "MQTTManager.h"
#include "Config.h"

bool MQTTManager::receivedCommand = false;
unsigned long MQTTManager::lastPacketTime = 0;

MQTTManager::MQTTManager()
: client(wifiClient)
{
}
void MQTTManager::connectWiFi()
{
    WiFi.mode(WIFI_STA);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi Connected");
}
void MQTTManager::begin()
{
    connectWiFi();

    client.setServer(MQTT_SERVER, MQTT_PORT);

    client.setCallback(callback);
}
void MQTTManager::reconnect()
{
    while (!client.connected())
    {
        Serial.print("Connecting to MQTT...");

        String clientID = "ESP32-";
        clientID += DEVICE_ID;

        if (client.connect(clientID.c_str()))
        {
            Serial.println("Connected");

#if DEVICE_ID=='A'
            client.subscribe(TOPIC_A);
#else
            client.subscribe(TOPIC_B);
#endif

        }
        else
        {
            Serial.print("Failed, rc=");
            Serial.println(client.state());

            delay(2000);
        }
    }
}
void MQTTManager::loop()
{
    if (!client.connected())
    {
        reconnect();
    }

    client.loop();
}
void MQTTManager::publishMotorState(bool runMotor)
{
#if DEVICE_ID=='A'
    client.publish(TOPIC_B, runMotor ? "1" : "0");
#else
    client.publish(TOPIC_A, runMotor ? "1" : "0");
#endif

    Serial.print("Sending: ");
    Serial.println(runMotor);
}
void MQTTManager::callback(char* topic,
                           byte* payload,
                           unsigned int length)
{
    if(length==0)
        return;

    receivedCommand = payload[0]=='1';

    lastPacketTime = millis();

    Serial.print("Received: ");

    Serial.print("Motor Command: ");
    Serial.println(receivedCommand ? "ON" : "OFF");
}
bool MQTTManager::motorCommandReceived()
{
    return receivedCommand;
}
unsigned long MQTTManager::getLastPacketTime()
{
    return lastPacketTime;
}
