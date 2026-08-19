#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include <stdio.h>

#define LED1_PIN GPIO_NUM_19
#define IR_PIN   GPIO_NUM_18
#define LED2_PIN GPIO_NUM_21

#define POT_CHANNEL ADC_CHANNEL_6   // GPIO34

#define LEDC_TIMER      LEDC_TIMER_0
#define LEDC_MODE       LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL    LEDC_CHANNEL_0
#define LEDC_DUTY_RES   LEDC_TIMER_8_BIT
#define LEDC_FREQUENCY  5000

void app_main(void)
{

    // LED 1 will be controlled using PWM
    gpio_set_direction(LED1_PIN, GPIO_MODE_OUTPUT);

    // IR sensor is a digital input
    gpio_set_direction(IR_PIN, GPIO_MODE_INPUT);

    // LED 2 is a digital output
    gpio_set_direction(LED2_PIN, GPIO_MODE_OUTPUT);


    adc_oneshot_unit_handle_t adc_handle;

    adc_oneshot_unit_init_cfg_t adc_init_config = {
        .unit_id = ADC_UNIT_1,
    };

    adc_oneshot_new_unit(&adc_init_config, &adc_handle);

    adc_oneshot_chan_cfg_t adc_channel_config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };

    adc_oneshot_config_channel(
        adc_handle,
        POT_CHANNEL,
        &adc_channel_config
    );

    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };

    ledc_timer_config(&ledc_timer);

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

    while (1)
    {

        int ir_state = gpio_get_level(IR_PIN);

        int pot_value = 0;

        adc_oneshot_read(
            adc_handle,
            POT_CHANNEL,
            &pot_value
        );

        // Convert ADC value to PWM duty

        int pwm_duty = (pot_value * 255) / 4095;

        ledc_set_duty(
            LEDC_MODE,
            LEDC_CHANNEL,
            pwm_duty
        );

        ledc_update_duty(
            LEDC_MODE,
            LEDC_CHANNEL
        );

        // IR = 0 → Object detected
        // IR = 1 → No object

        gpio_set_level(LED2_PIN, !ir_state);

        printf(
            "IR = %d | POT = %d | PWM = %d\n",
            ir_state,
            pot_value,
            pwm_duty
        );

        for (volatile int i = 0; i < 1000000; i++);
    }
}