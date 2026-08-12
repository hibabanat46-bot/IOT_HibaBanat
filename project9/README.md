# ESP32 MQTT Motor Control Dashboard

## Overview

This project demonstrates a complete IoT system using an ESP32, MQTT, Mosquitto, and Node-RED Dashboard. The ESP32 collects environmental data from a BME280 sensor and publishes it to an MQTT broker. Node-RED subscribes to these topics to display live telemetry and provides a dashboard to remotely control a DC motor.

The project also implements MQTT Last Will and Testament (LWT) to detect when the ESP32 disconnects unexpectedly.

---

## Hardware

- ESP32 DevKit
- BME280 Temperature/Humidity/Pressure Sensor
- DC Motor
- Motor Driver (L298N or equivalent)
- Breadboard & Jumper Wires
- Wi-Fi Network

---

## Software

- PlatformIO (VS Code)
- Arduino Framework
- Node-RED
- Node-RED Dashboard
- Mosquitto MQTT Broker
- MQTT Explorer

---

# Features

## Live Telemetry

- Live Temperature Gauge
- Live Humidity Gauge
- Live Pressure Gauge
- Temperature History Graph
- Humidity History Graph
- Pressure History Graph

---

## Motor Control

- Motor ON/OFF Switch
- Forward / Reverse Direction Control
- Speed Slider (0–100%)
- Emergency Stop Button

---

## Motor Status

- Live Motor State (Running / Stopped)
- Live Motor Direction
- Live Motor Speed
- ESP32 Connection Status (ONLINE / OFFLINE)

---

# MQTT Topics

## Sensor Topics

| Topic | Description |
|--------|-------------|
| home/esp32/sensor/temperature | Temperature (°C) |
| home/esp32/sensor/humidity | Humidity (%) |
| home/esp32/sensor/pressure | Pressure (hPa) |

---

## Motor Control Topics

| Topic | Description |
|--------|-------------|
| home/esp32/motor/power | Motor ON/OFF |
| home/esp32/motor/direction | Motor Direction |
| home/esp32/motor/speed | Motor Speed (0–100%) |
| home/esp32/motor/emergency | Emergency Stop |

---

## Status Topics

| Topic | Description |
|--------|-------------|
| home/esp32/status | ESP32 ONLINE / OFFLINE |
| home/esp32/status/power | Current Motor State |
| home/esp32/status/direction | Current Motor Direction |
| home/esp32/status/speed | Current Motor Speed |

---

# Dashboard Components

## Live Telemetry

- Temperature Gauge
- Humidity Gauge
- Pressure Gauge
- Temperature Line Chart
- Humidity Line Chart
- Pressure Line Chart

---

## Control Panel

- ON/OFF Switch
- Forward / Reverse Selector
- Speed Slider
- Emergency Stop Button

---

## Status Panel

- Power Status
- Direction Status
- Speed Status
- Connection Status

---

# Last Will and Testament (LWT)

The ESP32 uses MQTT Last Will and Testament to notify Node-RED when the device disconnects unexpectedly.

When connected:

```
ONLINE
```

When disconnected unexpectedly:

```
OFFLINE
```

This allows the dashboard to immediately indicate the connection status without waiting for new sensor data.

---

# Project Structure

```
project/
│
├── src/
│   └── main.cpp
│
├── include/
├── lib/
├── platformio.ini
├── flows.json
├── README.md
└── images/
```

---

# How to Run

1. Start the Mosquitto Broker.
2. Start Node-RED.
3. Open the Node-RED Dashboard.
4. Power on the ESP32.
5. Verify that the dashboard shows live sensor readings.
6. Test motor controls.
7. Test the Emergency Stop button.
8. Disconnect the ESP32 and verify that the dashboard displays **OFFLINE**.

---


## Dashboard

_Add dashboard screenshot here._

## Live Telemetry

_Add telemetry screenshot here._

## Motor Control

_Add motor control screenshot here._

## Connection Status

_Add offline status screenshot here._

---

# Author

**Hiba Hussein Banat**

University of Jordan

Mechatronics Engineering
