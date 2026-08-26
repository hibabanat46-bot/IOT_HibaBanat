#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"


#define LED1_PIN        GPIO_NUM_19
#define IR_PIN          GPIO_NUM_18
#define LED2_PIN        GPIO_NUM_21

#define POT_CHANNEL     ADC_CHANNEL_6       // GPIO34

#define LEDC_TIMER      LEDC_TIMER_0
#define LEDC_MODE       LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL    LEDC_CHANNEL_0
#define LEDC_DUTY_RES   LEDC_TIMER_8_BIT
#define LEDC_FREQUENCY  5000

#define SENSOR_STACK_SIZE   4096
#define OUTPUT_STACK_SIZE   3072

#define SENSOR_PRIORITY     1
#define OUTPUT_PRIORITY     2


static const char *SENSOR_TAG = "SENSOR_TASK";
static const char *OUTPUT_TAG = "OUTPUT_TASK";
static const char *MAIN_TAG = "MAIN";


// Shared between Sensor Task and Output Task
volatile int pot_value = 0;
volatile int ir_state = 0;



static adc_oneshot_unit_handle_t adc_handle;


// Task A - Sensor Task

void sensor_task(void *pvParameters)
{
    while (1)
    {
        // Read IR sensor
        ir_state = gpio_get_level(IR_PIN);

        // Read potentiometer
        int new_pot_value = 0;

        adc_oneshot_read(
            adc_handle,
            POT_CHANNEL,
            &new_pot_value
        );

        pot_value = new_pot_value;


        // Log sensor readings
        ESP_LOGI(
            SENSOR_TAG,
            "IR = %d | POT = %d",
            ir_state,
            pot_value
        );


        // Give the scheduler a chance to run other tasks
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}



void output_task(void *pvParameters)
{
    while (1)
    {
        // Convert ADC value (0-4095)
        // to PWM duty cycle (0-255)
        int pwm_duty = (pot_value * 255) / 4095;


        // Apply PWM duty cycle
        ledc_set_duty(
            LEDC_MODE,
            LEDC_CHANNEL,
            pwm_duty
        );

        ledc_update_duty(
            LEDC_MODE,
            LEDC_CHANNEL
        );


        // IR = 0 -> Object detected
        // IR = 1 -> No object

        gpio_set_level(
            LED2_PIN,
            !ir_state
        );


        // Log output values
        ESP_LOGI(
            OUTPUT_TAG,
            "PWM Duty = %d | IR LED = %d",
            pwm_duty,
            !ir_state
        );


        // Give the scheduler a chance to run other tasks
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


void app_main(void)
{
    ESP_LOGI(
        MAIN_TAG,
        "Starting Task 16 - FreeRTOS Two Task System"
    );


    // LED 1 - PWM output
    gpio_set_direction(
        LED1_PIN,
        GPIO_MODE_OUTPUT
    );

    // IR sensor - digital input
    gpio_set_direction(
        IR_PIN,
        GPIO_MODE_INPUT
    );

    // LED 2 - digital output
    gpio_set_direction(
        LED2_PIN,
        GPIO_MODE_OUTPUT
    );


    // ADC Configuration

    adc_oneshot_unit_init_cfg_t adc_init_config = {
        .unit_id = ADC_UNIT_1,
    };

    adc_oneshot_new_unit(
        &adc_init_config,
        &adc_handle
    );


    adc_oneshot_chan_cfg_t adc_channel_config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };

    adc_oneshot_config_channel(
        adc_handle,
        POT_CHANNEL,
        &adc_channel_config
    );

    // PWM Timer Configuration

    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };

    ledc_timer_config(&ledc_timer);


    // PWM Channel Configuration

    ledc_channel_config_t ledc_channel = {
        .gpio_num       = LED1_PIN,
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER,
        .duty           = 0,
        .hpoint         = 0
    };

    ledc_channel_config(&ledc_channel);

    // Create Sensor Task

    xTaskCreate(
        sensor_task,
        "SensorTask",
        SENSOR_STACK_SIZE,
        NULL,
        SENSOR_PRIORITY,
        NULL
    );

    // Create Output Task 

    xTaskCreate(
        output_task,
        "OutputTask",
        OUTPUT_STACK_SIZE,
        NULL,
        OUTPUT_PRIORITY,
        NULL
    );


    ESP_LOGI(
        MAIN_TAG,
        "Sensor Task and Output Task created successfully"
    );
}