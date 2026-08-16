# ESP32 IoT Dashboard with Firebase Cloud Logging

## Overview

This project extends the ESP32 MQTT dashboard by integrating Firebase Realtime Database with Node-RED.

The ESP32 publishes sensor readings and motor status to a local Mosquitto MQTT broker. Node-RED subscribes to these MQTT topics, displays live telemetry on the dashboard, and stores the data in Firebase.

---

## System Architecture

ESP32
↓
MQTT (Mosquitto)
↓
Node-RED
├── Dashboard
└── Firebase Realtime Database

---

## Features

### Live Dashboard

- Live Temperature Gauge
- Live Humidity Gauge
- Live Pressure Gauge
- Historical Charts
- Motor ON/OFF Status
- Motor Direction
- Motor Speed
- Emergency Stop
- ESP32 Connection Status

---

## Firebase Cloud Logging

The Firebase integration was added as an additional branch to the existing MQTT flow.

No duplicate MQTT subscriptions were created.

### Current Data

The following values are continuously updated:

- Temperature
- Humidity
- Pressure
- Motor Power
- Motor Direction
- Motor Speed
- ESP32 Status

Example:

```
current/
    temperature
    humidity
    pressure
    motor/
        power
        direction
        speed
    status
```

---

### History Log

Every **10 seconds**, Node-RED stores a snapshot of the current system state in Firebase.

Each record includes:

- Timestamp
- Temperature
- Humidity
- Pressure
- Motor Power
- Motor Direction
- Motor Speed
- ESP32 Status

Example:

```
history/
    1786880237092/
        temperature
        humidity
        pressure
        power
        direction
        speed
        status
        timestamp
```

---

## Offline Detection

The ESP32 uses MQTT Last Will and Testament (LWT).

If the ESP32 disconnects from Wi-Fi or loses power:

- Node-RED detects the disconnection.
- Dashboard status changes to OFFLINE.
- Firebase status is updated to OFFLINE.
- History also records the OFFLINE event.

---

## Technologies Used

- ESP32
- Arduino IDE
- MQTT
- Mosquitto Broker
- Node-RED
- Node-RED Dashboard
- Firebase Realtime Database

---

