#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#define IR_PIN 4

#define ENA 25
#define IN1 26
#define IN2 27

#define PWM_FREQ 5000
#define PWM_RESOLUTION 8

typedef struct struct_message
{
    uint8_t senderID;
    bool runMotor;
    uint32_t timestamp;

} struct_message;

struct_message outgoingMessage;
struct_message incomingMessage;

uint8_t peerAddress[] =
{
    0x00,
    0x70,
    0x07,
    0xA2,
    0x5E,
    0x8C
};
unsigned long lastPacketTime = 0;

esp_now_peer_info_t peerInfo;

void startMotor()
{
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    ledcWrite(0, 200);
}
void stopMotor()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    ledcWrite(0, 0);
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    Serial.print("Send Status: ");

    if(status == ESP_NOW_SEND_SUCCESS)
        Serial.println("Success");

    else
        Serial.println("Failed");
}

void OnDataRecv(const uint8_t * mac,
                const uint8_t *incomingData,
                int len)
{

    memcpy(&incomingMessage,
           incomingData,
           sizeof(incomingMessage));

    lastPacketTime = millis();

    if(incomingMessage.runMotor)
    {
        startMotor();
    }
    else
    {
        stopMotor();
    }

}
void setup()
{
    Serial.begin(115200);

    pinMode(IR_PIN, INPUT);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

   ledcSetup(0, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(ENA, 0);


    stopMotor();

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW Init Failed");
        return;
    }

    esp_now_register_send_cb(OnDataSent);

    esp_now_register_recv_cb(OnDataRecv);

    memcpy(peerInfo.peer_addr, peerAddress, 6);

    peerInfo.channel = 0;

    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("Failed to Add Peer");
        return;
    }

    Serial.println("ESP-NOW Ready");
}

void loop()
{
    int sensor = digitalRead(IR_PIN);

    outgoingMessage.senderID = 2;

    outgoingMessage.timestamp = millis();

    if(sensor == HIGH)
    {
        outgoingMessage.runMotor = true;
    }
    else
    {
        outgoingMessage.runMotor = false;
    }

    esp_now_send(peerAddress,
                 (uint8_t *) &outgoingMessage,
                 sizeof(outgoingMessage));

    if(millis() - lastPacketTime > 2000)
    {
        stopMotor();
    }

    delay(100);
}