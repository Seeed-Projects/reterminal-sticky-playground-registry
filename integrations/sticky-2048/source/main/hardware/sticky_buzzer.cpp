#include "hardware/sticky_buzzer.h"

#include "driver/ledc.h"
#include "esp_err.h"
#include "pin_config.h"

bool StickyBuzzer::init()
{
    ledc_timer_config_t timer_config = {};
    timer_config.speed_mode = LEDC_LOW_SPEED_MODE;
    timer_config.timer_num = LEDC_TIMER_0;
    timer_config.duty_resolution = LEDC_TIMER_10_BIT;
    timer_config.freq_hz = 4000;
    timer_config.clk_cfg = LEDC_AUTO_CLK;

    ledc_channel_config_t channel_config = {};
    channel_config.gpio_num = PIN_BUZZER;
    channel_config.speed_mode = LEDC_LOW_SPEED_MODE;
    channel_config.channel = LEDC_CHANNEL_0;
    channel_config.intr_type = LEDC_INTR_DISABLE;
    channel_config.timer_sel = LEDC_TIMER_0;
    channel_config.duty = 0;

    ready_ = ledc_timer_config(&timer_config) == ESP_OK &&
             ledc_channel_config(&channel_config) == ESP_OK;
    return ready_;
}

bool StickyBuzzer::tone(uint32_t frequency_hz)
{
    if (!ready_ || frequency_hz == 0) {
        return false;
    }
    return ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, frequency_hz) != 0 &&
           ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 512) == ESP_OK &&
           ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0) == ESP_OK;
}

void StickyBuzzer::stop()
{
    if (ready_) {
        (void)ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    }
}
