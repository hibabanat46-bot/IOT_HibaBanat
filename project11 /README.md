# Pure ESP-IDF Sensor and PWM Control Project

A practical ESP32 project developed using **Pure ESP-IDF** through **PlatformIO**.

The project demonstrates digital GPIO control, digital sensor input, ADC OneShot reading, and LED PWM control. A potentiometer controls the brightness of one LED, while an IR sensor controls a second LED.

---

## Project Overview

This project uses an ESP32 development board to demonstrate several fundamental ESP-IDF peripherals and APIs without using Arduino functions or Arduino libraries.

The system performs two main tasks:

1. Read a potentiometer using the ESP-IDF ADC OneShot API and use its value to control the brightness of an LED using PWM.
2. Read an IR sensor as a digital input and use its state to control a second LED.

### Main System

```text
                    ┌─────────────────┐
                    │      ESP32      │
                    │                 │
                    │   ADC GPIO 34   │◄──── Potentiometer
                    │                 │
                    │   GPIO 18       │◄──── IR Sensor
                    │                 │
                    │   GPIO 19       │────► Green LED
                    │                 │
                    │   GPIO 21       │────► Red LED
                    └─────────────────┘
Technologies and Framework
Microcontroller: ESP32
Framework: ESP-IDF
Development Environment: PlatformIO
IDE: Visual Studio Code
Language: C
Arduino Framework: Not used
RTOS APIs: Not used directly

The project is configured in platformio.ini using:

[env:esp32dev]
platform = espressif32
board = esp32dev
framework = espidf
monitor_speed = 115200
Hardware Components

The project uses:

ESP32 Development Board
Potentiometer
IR Sensor Module
Green LED
Red LED
Current-limiting resistors for LEDs
Breadboard
Jumper wires
USB cable
Pin Configuration
Component	ESP32 Pin	Function
Green LED	GPIO 19	PWM Output
IR Sensor OUT	GPIO 18	Digital Input
Red LED	GPIO 21	Digital Output
Potentiometer Wiper	GPIO 34	ADC Input
Power Connections

The potentiometer is connected between 3.3 V and GND, with its middle/wiper pin connected to GPIO 34.

The IR sensor is connected to 3.3 V, GND, and GPIO 18.

Task 1 — Potentiometer Controlled LED Brightness

The first task is to read the potentiometer value and use it to control the brightness of the green LED.

The signal flow is:

Potentiometer
      ↓
   GPIO 34
      ↓
   ADC OneShot
      ↓
 ADC Value 0–4095
      ↓
 Convert to 0–255
      ↓
   PWM Duty
      ↓
   LEDC PWM
      ↓
Green LED GPIO 19
ADC OneShot

The potentiometer is read using the ESP-IDF ADC OneShot driver.

The project uses the following APIs:

adc_oneshot_new_unit()
adc_oneshot_config_channel()
adc_oneshot_read()
ADC Unit Initialization

The ADC unit is initialized using:

adc_oneshot_unit_init_cfg_t adc_init_config = {
    .unit_id = ADC_UNIT_1,
};

Then the ADC unit is created using:

adc_oneshot_new_unit(&adc_init_config, &adc_handle);
ADC Channel Configuration

GPIO 34 corresponds to ADC1 Channel 6 on the ESP32.

#define POT_CHANNEL ADC_CHANNEL_6

The channel is configured using:

adc_oneshot_config_channel(
    adc_handle,
    POT_CHANNEL,
    &adc_channel_config
);
Reading the Potentiometer

The potentiometer value is continuously read using:

adc_oneshot_read(
    adc_handle,
    POT_CHANNEL,
    &pot_value
);

The ADC reading is then used to calculate the PWM duty cycle.

PWM LED Brightness Control

The green LED is connected to GPIO 19 and controlled using the ESP-IDF LEDC peripheral.

The project uses:

ledc_timer_config()
ledc_channel_config()
ledc_set_duty()
ledc_update_duty()
PWM Configuration

The PWM configuration uses:

Frequency: 5 kHz
Resolution: 8-bit
Duty range: 0–255
PWM GPIO: GPIO 19
#define LEDC_FREQUENCY 5000
#define LEDC_DUTY_RES  LEDC_TIMER_8_BIT
ADC-to-PWM Conversion

The ADC produces a value between approximately:

0 → 4095

The 8-bit PWM duty cycle uses:

0 → 255

Therefore, the ADC value is converted using:

int pwm_duty = (pot_value * 255) / 4095;

This allows the potentiometer position to directly control LED brightness.

Example
Potentiometer Value	PWM Duty	Approximate Brightness
0	0	0%
~1024	~64	25%
~2048	~128	50%
~3072	~192	75%
4095	255	100%
Task 2 — IR Sensor Controlled LED

The second task uses the IR sensor as a digital input.

The IR sensor output is connected to:

GPIO 18

The ESP32 reads the sensor using:

gpio_get_level(IR_PIN);

The GPIO is configured as a digital input:

gpio_set_direction(IR_PIN, GPIO_MODE_INPUT);
Active-Low IR Sensor

During testing, the IR sensor was observed to operate with active-low logic:

IR = 0 → Object detected
IR = 1 → No object detected

Because the required LED behavior is opposite to the raw sensor logic, the value is inverted when controlling the red LED:

gpio_set_level(LED2_PIN, !ir_state);

Therefore:

IR State	Sensor Condition	Red LED
0	Object detected	ON
1	No object detected	OFF
Digital GPIO Control

The project also demonstrates basic ESP-IDF digital GPIO control.

The LEDs are configured as outputs using:

gpio_set_direction(
    LED_PIN,
    GPIO_MODE_OUTPUT
);

The output level is controlled using:

gpio_set_level(
    LED_PIN,
    level
);

The IR sensor is configured as an input:

gpio_set_direction(
    IR_PIN,
    GPIO_MODE_INPUT
);

Its state is read using:

gpio_get_level(IR_PIN);
Complete System Logic

The main loop continuously performs the following operations:

              Start
                │
                ▼
       Initialize GPIOs
                │
                ▼
        Initialize ADC
                │
                ▼
        Initialize PWM
                │
                ▼
             Main Loop
                │
       ┌────────┴────────┐
       ▼                 ▼
 Read Potentiometer   Read IR Sensor
       │                 │
       ▼                 ▼
   ADC 0–4095       IR = 0 or 1
       │                 │
       ▼                 ▼
 Convert ADC to      Control Red LED
 PWM Duty
       │
       ▼
 Update PWM
       │
       ▼
 Green LED Brightness
       │
       └───────► Repeat
Main ESP-IDF APIs Used
Function	Purpose
gpio_set_direction()	Configure GPIO as input or output
gpio_set_level()	Set digital output level
gpio_get_level()	Read digital input level
adc_oneshot_new_unit()	Initialize ADC OneShot unit
adc_oneshot_config_channel()	Configure ADC channel
adc_oneshot_read()	Read ADC value
ledc_timer_config()	Configure PWM timer
ledc_channel_config()	Configure PWM output channel
ledc_set_duty()	Set PWM duty cycle
ledc_update_duty()	Apply the PWM duty cycle
Testing and Results

The project was tested using the physical ESP32 hardware.

Test 1 — Digital LED Output

The LED connected to GPIO 19 was successfully configured as a digital output and tested.

Result: PASS

Test 2 — IR Sensor

The IR sensor connected to GPIO 18 was successfully read as a digital input.

The sensor was confirmed to produce:

0 → Object detected
1 → No object detected

Result: PASS

Test 3 — IR Controlled LED

The red LED connected to GPIO 21 was successfully controlled according to the IR sensor state.

Result: PASS

Test 4 — Potentiometer ADC

The potentiometer connected to GPIO 34 was successfully read using the ADC OneShot API.

The ADC value changed as the potentiometer was rotated.

Result: PASS

Test 5 — PWM

The green LED on GPIO 19 was successfully controlled using LEDC PWM.

The brightness changed according to the PWM duty cycle.

Result: PASS

Test 6 — Complete Integration

The final integrated system successfully performed both required tasks:

Potentiometer → ADC → PWM → LED brightness
IR sensor → digital input → second LED ON/OFF

Result: PASS

Serial Monitor

The project uses a serial monitor speed of:

115200 baud

Example output:

IR = 1 | POT = 100 | PWM = 6
IR = 1 | POT = 1500 | PWM = 93
IR = 0 | POT = 3000 | PWM = 186

The values demonstrate that:

The IR state is being read continuously.
The potentiometer ADC value changes.
The PWM duty cycle follows the potentiometer value.
Project Structure
project11/
│
├── src/
│   ├── main.c
│   └── CMakeLists.txt
│
├── include/
│   └── README
│
├── lib/
│   └── README
│
├── test/
│   └── README
│
├── CMakeLists.txt
├── platformio.ini
├── sdkconfig.esp32dev
└── .gitignore
Development Workflow

The project was developed incrementally:

Configured a Pure ESP-IDF project using PlatformIO.
Tested digital output using an LED.
Added the IR sensor as a digital input.
Connected the IR sensor to a second LED.
Configured the ADC OneShot driver.
Tested potentiometer readings.
Configured LEDC PWM.
Tested PWM independently.
Integrated the potentiometer with PWM.
Tested the complete system on physical ESP32 hardware.
Key Learning Outcomes

This project demonstrates practical understanding of:

ESP32 GPIO configuration
Digital input and output
Active-low sensor logic
ADC OneShot operation
ADC channel configuration
PWM generation using LEDC
PWM duty-cycle control
ADC-to-PWM mapping
Hardware/software integration
Pure ESP-IDF development
PlatformIO project configuration
Conclusion

The project successfully demonstrates two integrated ESP32 control tasks using Pure ESP-IDF.

The first task uses a potentiometer as an analog input. Its ADC value is converted into an appropriate PWM duty cycle, allowing continuous control of an LED's brightness.

The second task uses an IR sensor as a digital input. The sensor state is read through GPIO and used to control a second LED.

The project was successfully built, uploaded, and tested on physical ESP32 hardware using PlatformIO and the ESP-IDF framework without using Arduino functions.
