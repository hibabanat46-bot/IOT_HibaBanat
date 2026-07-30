#ifndef CONFIG_H
#define CONFIG_H

// Change only this line
//#define DEVICE_ID 'A'
#define DEVICE_ID 'B'

// WiFi
#define WIFI_SSID "CYBER_EXT"
#define WIFI_PASSWORD "cyberap2025"

// MQTT
#define MQTT_SERVER "broker.emqx.io"
#define MQTT_PORT 1883

// Topics
#define TOPIC_A "UJ/ESP32/A"
#define TOPIC_B "UJ/ESP32/B"

// Pins
#define IR_PIN 4

#define ENA 25
#define IN1 26
#define IN2 27

// PWM
#define PWM_CHANNEL 0
#define PWM_FREQ 5000
#define PWM_RESOLUTION 8
#define MOTOR_SPEED 200

// Timeout
#define TIMEOUT 2000

#endif