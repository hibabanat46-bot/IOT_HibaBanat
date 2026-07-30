#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <WiFi.h>
#include <PubSubClient.h>


class MQTTManager
{
public:
    MQTTManager();

    void begin();
    void reconnect();
    void loop();

    void publishMotorState(bool runMotor);

    bool motorCommandReceived();

    unsigned long getLastPacketTime();

private:
    WiFiClient wifiClient;
    PubSubClient client;

    static unsigned long lastPacketTime;

    static bool receivedCommand;

    static void callback(char* topic, byte* payload, unsigned int length);

    void connectWiFi();
};

#endif