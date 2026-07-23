#ifndef CONFIG_H
#define CONFIG_H

// Motor Driver Pins
#define IN1_PIN 25
#define IN2_PIN 26
#define ENA_PIN 27

// Current Sensor
#define CURRENT_SENSOR_PIN 34

// Emergency Button
#define ESTOP_PIN 33

// LEDs
#define RUN_LED_PIN       16
#define STOP_LED_PIN      19
#define HIGH_LED_PIN      17
#define LOW_LED_PIN       18

#define PWM_CHANNEL       0
#define PWM_FREQUENCY     5000
#define PWM_RESOLUTION    8

#define CURRENT_LIMIT 1.5
#define DEFAULT_SPEED 255

#endif