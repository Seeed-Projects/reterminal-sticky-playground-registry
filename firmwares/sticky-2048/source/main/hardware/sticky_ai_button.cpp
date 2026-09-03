#include "hardware/sticky_ai_button.h"

#include "button_gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "pin_config.h"

namespace {
constexpr const char *kTag = "sticky_ai_button";
constexpr uint32_t kLongPressMs = 1500;
constexpr uint32_t kShortPressMs = 180;
}

bool StickyAiButton::init()
{
    button_config_t button_config = {};
    button_config.long_press_time = kLongPressMs;
    button_config.short_press_time = kShortPressMs;

    button_gpio_config_t gpio_config = {};
    gpio_config.gpio_num = PIN_BTN_OK;
    gpio_config.active_level = 0;
    gpio_config.enable_power_save = false;
    gpio_config.disable_pull = false;

    esp_err_t err = iot_button_new_gpio_device(&button_config, &gpio_config, &handle_);
    if (err != ESP_OK || handle_ == nullptr) {
        ESP_LOGE(kTag, "AI button init failed: %s", esp_err_to_name(err));
        return false;
    }

    err = iot_button_register_cb(handle_,
                                 BUTTON_SINGLE_CLICK,
                                 nullptr,
                                 on_short_press,
                                 this);
    if (err != ESP_OK) {
        ESP_LOGE(kTag, "AI short press callback failed: %s", esp_err_to_name(err));
        return false;
    }

    err = iot_button_register_cb(handle_,
                                 BUTTON_LONG_PRESS_START,
                                 nullptr,
                                 on_long_press,
                                 this);
    if (err != ESP_OK) {
        ESP_LOGE(kTag, "AI long press callback failed: %s", esp_err_to_name(err));
        return false;
    }

    ESP_LOGI(kTag, "AI button ready on GPIO%d", PIN_BTN_OK);
    return true;
}

AiButtonEvent StickyAiButton::poll()
{
    return pending_event_.exchange(AiButtonEvent::None);
}

bool StickyAiButton::is_pressed() const
{
    return handle_ != nullptr && iot_button_get_key_level(handle_) == 1;
}

void StickyAiButton::on_short_press(void *, void *user_data)
{
    static_cast<StickyAiButton *>(user_data)->pending_event_.store(AiButtonEvent::ShortPress);
}

void StickyAiButton::on_long_press(void *, void *user_data)
{
    static_cast<StickyAiButton *>(user_data)->pending_event_.store(AiButtonEvent::LongPress);
}
