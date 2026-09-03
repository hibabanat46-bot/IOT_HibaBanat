#FreeRTOS 

## Overview

This project converts the previous single-loop ESP32 application into two independent FreeRTOS tasks using ESP-IDF.

The system reads a potentiometer through ADC and an IR sensor through GPIO. The potentiometer controls the brightness of an LED using PWM, while the IR sensor controls a second digital LED.

The main purpose of this task is to understand FreeRTOS task scheduling, task priorities, task delays, and communication between tasks using shared global variables.

## Objectives

* Create two independent FreeRTOS tasks using `xTaskCreate()`.
* Separate sensor reading and output control into different tasks.
* Assign different priorities and stack sizes to the tasks.
* Use `vTaskDelay()` instead of busy-waiting.
* Replace `printf` with ESP-IDF logging using `ESP_LOGI()`.
* Observe the interleaved execution of the two tasks through the Serial Monitor.
* Understand the limitations of using plain global variables for communication between tasks.

## Hardware

| Component         | ESP32 Pin              |
| ----------------- | ---------------------- |
| Potentiometer     | GPIO34 / ADC Channel 6 |
| PWM LED           | GPIO19                 |
| IR Sensor         | GPIO18                 |
| IR-Controlled LED | GPIO21                 |

## PWM Configuration

* PWM Timer: `LEDC_TIMER_0`
* PWM Channel: `LEDC_CHANNEL_0`
* Frequency: 5000 Hz
* Resolution: 8-bit
* PWM duty range: 0–255

The ADC value ranges from approximately 0 to 4095 and is converted to an 8-bit PWM duty cycle:

```text
PWM Duty = (ADC Value × 255) / 4095
```

For example:

```text
ADC = 1600
PWM ≈ 99
```

## FreeRTOS Task Structure

### Sensor Task

The Sensor Task is responsible for:

* Reading the potentiometer using ADC.
* Reading the IR sensor using GPIO.
* Updating the shared global variables.
* Logging the sensor readings.

Configuration:

```text
Task Name: SensorTask
Priority: 1
Stack Size: 4096 bytes
Delay: 200 ms
```

### Output Task

The Output Task is responsible for:

* Reading the shared potentiometer value.
* Calculating the PWM duty cycle.
* Updating the PWM output.
* Reading the shared IR state.
* Controlling the IR-triggered LED.
* Logging the output values.

Configuration:

```text
Task Name: OutputTask
Priority: 2
Stack Size: 3072 bytes
Delay: 100 ms
```

The Output Task has a higher priority than the Sensor Task.

## Data Sharing

The two tasks communicate using plain global variables:

```c
volatile int pot_value = 0;
volatile int ir_state = 0;
```

The Sensor Task writes the sensor values, while the Output Task reads them.

No Queue, Mutex, or Semaphore is used in this task intentionally. This is the basic or "naive" method of sharing data between tasks.

The synchronization and safety limitations of this approach will be addressed in a later task.

## Logging

All project logging is performed using ESP-IDF logging functions.

Each task has its own logging TAG:

```text
SENSOR_TASK
OUTPUT_TASK
MAIN
```

Example Serial Monitor output:

```text
I (...) SENSOR_TASK: IR = 1 | POT = 1619
I (...) OUTPUT_TASK: PWM Duty = 99 | IR LED = 0
I (...) OUTPUT_TASK: PWM Duty = 99 | IR LED = 0
I (...) SENSOR_TASK: IR = 0 | POT = 1593
I (...) OUTPUT_TASK: PWM Duty = 99 | IR LED = 1
```

No `printf()` statements are used in the project.

## Task Scheduling Observation

The Serial Monitor timestamps demonstrate that the two tasks execute independently and their output is interleaved.

The Output Task runs approximately every 100 ms, while the Sensor Task runs approximately every 200 ms.

Therefore, the output commonly appears in a pattern similar to:

```text
OUTPUT_TASK
SENSOR_TASK
OUTPUT_TASK
OUTPUT_TASK
SENSOR_TASK
OUTPUT_TASK
```

When both tasks are ready to run at approximately the same time, the higher-priority Output Task has scheduling priority.

## Testing Results

### Potentiometer and PWM

The potentiometer was tested at different values. The ADC reading changed according to the potentiometer position, and the PWM duty cycle changed accordingly.

Example:

```text
POT = 1600
PWM Duty = 99
```

This confirms that the ADC-to-PWM conversion is working correctly.

### IR Sensor

The IR sensor was tested with and without object detection.

Observed behavior:

```text
IR = 1 → IR LED = 0
IR = 0 → IR LED = 1
```

This confirms that the IR sensor correctly controls the digital LED.

### FreeRTOS Task Execution

Both tasks were observed running through the Serial Monitor. Their timestamps demonstrate independent and interleaved execution rather than a single sequential loop.

## Important FreeRTOS Concepts

This project demonstrates the following FreeRTOS concepts:

* Tasks
* Task priorities
* Task states
* Scheduler
* `xTaskCreate()`
* `vTaskDelay()`
* Shared global variables
* Independent task execution

The ESP32 is a dual-core microcontroller, but using `xTaskCreate()` does not mean that each task is automatically assigned to a different core. Tasks may be scheduled according to the FreeRTOS configuration. Core-specific execution can be controlled using task-pinning APIs when required.

## Limitations

Using plain global variables is simple but does not provide proper synchronization between tasks.

A task may read a value that was not updated at the expected time, resulting in stale or inconsistent data. This project intentionally does not solve this problem because synchronization mechanisms such as queues and mutexes are introduced in later tasks.

## Conclusion

Task 16 successfully converts the previous ESP32 single-loop application into a FreeRTOS-based two-task system.

The Sensor Task handles sensor acquisition, while the Output Task handles PWM and digital outputs. Different task priorities and delays allow the FreeRTOS scheduler to manage their execution independently.

Testing confirmed correct potentiometer-to-PWM control, IR sensor operation, ESP-IDF logging, and interleaved execution of both FreeRTOS tasks.
