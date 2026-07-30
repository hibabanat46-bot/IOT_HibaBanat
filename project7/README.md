# ESP32 MQTT Motor Control System

## Overview

This project demonstrates communication between two ESP32 boards using the MQTT protocol. Each ESP32 reads an IR obstacle sensor and sends its state through a public MQTT broker. The other ESP32 receives the message and controls its DC motor accordingly.

The project replaces ESP-NOW communication with MQTT over Wi-Fi while maintaining the same functionality.

---

## Features

- Two ESP32 boards connected through Wi-Fi
- MQTT communication using a public broker
- IR obstacle detection
- Remote DC motor control
- Automatic MQTT reconnection
- Fail-safe mechanism that stops the motor if communication is lost
- Object-oriented programming using C++ classes

---

## Hardware Components

- 2 × ESP32 Development Boards
- 2 × IR Obstacle Sensors
- 2 × L298N Motor Drivers
- 2 × DC Motors
- Jumper Wires
- Power Supply

---

## Software Requirements

- Visual Studio Code
- PlatformIO
- Arduino Framework
- PubSubClient Library

---

## Project Structure

```
.
├── include/
│   └── Config.h
├── src/
│   ├── main.cpp
│   ├── Motor.cpp
│   ├── IRSensor.cpp
│   └── MQTTManager.cpp
├── lib/
│   ├── Motor/
│   ├── IRSensor/
│   └── MQTTManager/
├── platformio.ini
└── README.md
```

---

## How It Works

1. Each ESP32 connects to the local Wi-Fi network.
2. Both ESP32 boards connect to the MQTT broker.
3. Each ESP32 continuously reads its own IR sensor.
4. The sensor state is published to the MQTT topic assigned to the other ESP32.
5. The receiving ESP32 starts or stops its motor based on the received message.
6. If no MQTT message is received for more than two seconds, the motor stops automatically (Fail-Safe).

---

## MQTT Configuration

Broker:

```
broker.emqx.io
```

Topics:

```
UJ/ESP32/A
UJ/ESP32/B
```

---

## Device Configuration

Select the board identity inside `Config.h`.

ESP32 #1

```cpp
#define DEVICE_ID 'A'
```

ESP32 #2

```cpp
#define DEVICE_ID 'B'
```

Only this value needs to be changed before uploading the code to the second ESP32.

---

## Fail-Safe

If communication is interrupted for more than two seconds, the motor automatically stops to ensure safe operation.

---

## Author

Hiba Hussein Banat

Mechatronics Engineering
