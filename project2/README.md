"C:\Users\USER\Downloads\Report-2.docx"

1. Project Overview
The purpose of this project is to demonstrate the use of both digital and analog inputs with the ESP32 microcontroller. The system combines an infrared (IR) obstacle sensor and a potentiometer to control the brightness of an LED using Pulse Width Modulation (PWM). All sensor readings, the calculated voltage, and the resulting PWM duty cycle are displayed in real time through the Serial Monitor for monitoring and debugging.
The firmware was developed in C++ using Visual Studio Code with the PlatformIO extension, which provides a professional, reproducible embedded-systems development environment with integrated build, upload, and serial-monitoring tools.
2. Objectives
•	Use an IR sensor to detect the presence of an object (digital input).
•	Use a potentiometer to generate a variable analog signal (analog input).
•	Convert the analog reading into a PWM signal to control LED brightness (0–100%).
•	Display sensor readings, the calculated voltage, and the PWM value on the Serial Monitor.
•	Understand and demonstrate the integration of digital and analog logic on a single microcontroller.
3. Hardware Selection
The following components were selected for the project:
No.	Component	Qty	Description
1	ESP32 Development Board	1	Main microcontroller
2	IR Obstacle Sensor	1	Detects object presence (Active LOW)
3	Potentiometer (10 kΩ)	1	Provides a variable analog voltage
4	LED	1	Brightness controlled by PWM
5	Resistor (220 Ω)	1	Current-limiting resistor for the LED
6	Breadboard	1	Prototyping connections
7	Jumper Wires	Various	Point-to-point interconnections
8	USB Cable	1	Power supply and programming

The ESP32 was selected as the main controller because it provides:
•	Multiple digital GPIO pins for sensor interfacing.
•	A built-in Analog-to-Digital Converter (ADC) with 12-bit resolution (0–4095).
•	Hardware PWM output channels (LEDC) for smooth brightness control.
•	Sufficient processing speed for real-time sensor sampling and serial reporting.
•	Built-in Wi-Fi and Bluetooth (not used in this project).
4. Circuit Design and Connections
The circuit was assembled on a breadboard. The IR sensor and the potentiometer were wired to the ESP32's digital and analog input pins respectively, and the LED was connected through a 220 Ω current-limiting resistor to a PWM-capable GPIO pin.
Component	ESP32 Pin	Notes
IR Sensor VCC	3.3 V	Power
IR Sensor GND	GND	Ground
IR Sensor OUT	GPIO 4	Digital input (Active LOW)
Potentiometer VCC	3.3 V	Power
Potentiometer GND	GND	Ground
Potentiometer OUT	GPIO 34	Analog input (ADC)
LED Anode (+)	GPIO 25	PWM output
LED Cathode (–)	GND	Through 220 Ω resistor
Notes:
•	The IR sensor used is Active LOW: its output is LOW when an object is detected and HIGH otherwise.
5. Physical Build
The completed hardware assembly on the breadboard, showing the ESP32, IR sensor, potentiometer, and LED wiring:

Figure 1 – Complete breadboard assembly: ESP32, IR obstacle sensor, potentiometer, and LED.

Figure 2 – Functional test: an object placed in front of the IR sensor triggers the LED.
6. System Architecture
At a high level, the ESP32 reads two inputs (the IR sensor and the potentiometer) and produces two outputs (the PWM-driven LED and the Serial Monitor data stream):
Block	Type	Function
IR Sensor	Digital Input	Detects presence/absence of an object
Potentiometer	Analog Input	Sets the desired brightness level
ESP32 Microcontroller	Processing	Reads inputs, computes PWM duty cycle, drives output
LED (PWM Output – GPIO 25)	Output	Brightness control via LEDC PWM channel
Serial Monitor	Data Output	Real-time display of IR state, ADC value, voltage, PWM
7. Software Development
The firmware was written in C++ using the Arduino framework within PlatformIO and organized into the following stages.
7.1  Pin Configuration and Initialization
The GPIO pins were defined as constants, and the Serial Monitor was initialized at 115200 baud:
const int IR_PIN  = 4;
const int POT_PIN = 34;
const int LED_PIN = 25;
 
Serial.begin(115200);
7.2  PWM Configuration (LEDC)
The ESP32's hardware LEDC peripheral was configured with a 5 kHz frequency and 8-bit resolution (duty cycle range 0–255), then attached to the LED pin:
ledcSetup(channel, 5000, 8);
ledcAttachPin(LED_PIN, channel);
7.3  Reading the IR Sensor
The IR sensor output is read as a digital signal. The sensor operates in Active LOW mode: a LOW reading indicates an object has been detected, while HIGH indicates no object is present.
int irState = digitalRead(IR_PIN);
7.4  Reading the Potentiometer
The potentiometer wiper voltage is sampled by the ESP32's 12-bit ADC, producing a value between 0 and 4095:
int potValue = analogRead(POT_PIN);
7.5  Voltage Calculation
The raw ADC value is converted to an equivalent voltage (relative to the 3.3 V reference) and printed to the Serial Monitor:
float voltage = potValue * (3.3 / 4095.0);
7.6  PWM Duty-Cycle Calculation
The 12-bit ADC reading (0–4095) is remapped to the 8-bit PWM duty-cycle range (0–255), allowing the LED brightness to change smoothly as the potentiometer is rotated:
int pwmValue = map(potValue, 0, 4095, 0, 255);
7.7  Control Logic
The IR sensor result determines whether the LED is allowed to operate at all. If an object is detected, the potentiometer is read and the LED brightness is updated; otherwise the LED is forced off.
IF IR detects an object:
    Read potentiometer
    Calculate PWM value
    Update LED brightness
ELSE:
    Turn LED OFF
7.8  Serial Monitoring
On every loop iteration, the ESP32 prints the IR state, potentiometer reading, calculated voltage, and PWM value, for example:
IR: 0 | Pot: 2150 | Voltage: 1.73 V | PWM: 134
8. System Operation Flow
The complete operating sequence executed by the ESP32 on each loop cycle is summarized below:
1.	Power ON and initialize the ESP32, IR sensor, and PWM (LEDC) channel.
2.	Read the IR sensor state.
3.	Decision: Is an object detected (IR == LOW)?
	If YES: read the potentiometer → map the ADC value to PWM (0–255) → update LED brightness.
	If NO: set PWM = 0, turning the LED OFF.
4.	Print the IR state, potentiometer value, voltage, and PWM value to the Serial Monitor.
5.	Repeat the loop. 
9. Complete Source Code (main.cpp)
#include <Arduino.h>
 
const int IR_PIN  = 4;
const int POT_PIN = 34;
const int LED_PIN = 25;
const int channel = 0;
 
void setup() {
  Serial.begin(115200);
  pinMode(IR_PIN, INPUT);
  ledcSetup(channel, 5000, 8);      // Channel, Frequency, Resolution (8-bit)
  ledcAttachPin(LED_PIN, channel);
}
 
void loop() {
  int irState  = digitalRead(IR_PIN);
  int potValue = analogRead(POT_PIN);
  float voltage  = potValue * (3.3 / 4095.0);
  int   pwmValue = map(potValue, 0, 4095, 0, 255);
 
  // IR sensor is Active LOW
  if (irState == LOW) {
    ledcWrite(channel, pwmValue);   // Control LED brightness
  } else {
    ledcWrite(channel, 0);          // Turn LED OFF
  }
 
  Serial.print("IR: ");     Serial.print(irState);
  Serial.print(" | Pot: "); Serial.print(potValue);
  Serial.print(" | Voltage: "); Serial.print(voltage, 2);
  Serial.print(" V | PWM: ");   Serial.println(pwmValue);
 
  delay(300);
}
10. Testing and Results
10.1  Test 1 – No Object Detected
•	IR output = HIGH
•	LED remains OFF
•	PWM output = 0
10.2  Test 2 – Object Detected
•	IR output = LOW
•	LED turns ON at a brightness set by the potentiometer
10.3  Test 3 – Potentiometer Rotation
•	Rotating the potentiometer changes the ADC reading.
•	The PWM duty cycle changes proportionally.
•	The LED brightness changes smoothly and continuously.
10.4  Test 4 – Serial Monitor Output
The Serial Monitor successfully displayed the IR state, ADC value, calculated voltage, and PWM duty cycle, with all values updating in real time. A sample of the captured output is shown below:
These results confirm that the ADC-to-voltage conversion, the ADC-to-PWM mapping, and the IR-based enable/disable logic all behave as designed. 
11. Challenges Faced
Several practical issues were encountered and resolved during implementation:
•	Configuring the ESP32 board definition correctly within PlatformIO.
•	Selecting the correct COM port for uploading the compiled firmware.
•	Understanding and correctly handling the IR sensor's Active LOW behavior.
•	Correctly configuring the ESP32 hardware PWM (LEDC) channel, frequency, and resolution.
•	Mapping the 12-bit analog input range (0–4095) to the 8-bit PWM range (0–255).
These issues were resolved through iterative testing, Serial Monitor debugging, and careful verification of both the hardware wiring and the software configuration.
12. Deliverables and Submission
•	Complete PlatformIO project source code (main.cpp).
•	Circuit connection table and hardware photographs (Figures 1–2).
•	System architecture summary and operation flow.
•	Serial Monitor test output demonstrating correct digital and analog behavior.
•	This written project report, documenting the design process, implementation, testing, and results.
13. Conclusion
This project successfully demonstrated the integration of digital sensing, analog signal processing, and PWM control using the ESP32 microcontroller. By combining an IR sensor with a potentiometer, the system provides both object detection and continuously adjustable LED brightness. The Serial Monitor enabled real-time monitoring of all key variables, making the system easy to test, debug, and verify. The project provided practical, hands-on experience with GPIO configuration, ADC sampling, hardware PWM generation, and professional embedded-software development using PlatformIO within Visual Studio Code.
