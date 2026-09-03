#include "hardware/sticky_status.h"

#include <algorithm>
#include <cstddef>

#include "bq27220.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_random.h"
#include "pin_config.h"

namespace {
constexpr const char *kTag = "sticky_status";

constexpr const char *kMessages[] = {
    "Keep going.",
    "Small steps matter.",
    "Stay curious.",
    "Enjoy the journey.",
    "Make something.",
    "Progress over perfection.",
    "Ideas grow by making.",
    "Build one step at a time.",
    "Keep learning.",
    "Try another way.",
    "Create with care.",
    "Simple can be powerful.",
    "Learn by doing.",
    "Trust the process.",
    "Keep exploring.",
    "Start small.",
    "Build with purpose.",
    "Curiosity leads forward.",
    "One idea at a time.",
    "Make room for wonder.",
    "Better comes with practice.",
    "Keep moving forward.",
    "Turn ideas into things.",
    "Every attempt teaches.",
    "Focus on the next step.",
    "Keep the spark alive.",
    "Make and improve.",
    "Begin where you are.",
    "Think. Make. Learn.",
    "Let ideas evolve.",
    "Choose progress.",
    "Build what matters.",
    "Test and learn.",
    "Keep it simple.",
    "Create something useful.",
    "Explore the details.",
    "Keep asking questions.",
    "Practice makes progress.",
    "Shape the next idea.",
    "Learn from every try.",
    "Make space to create.",
    "Find joy in making.",
    "Solve one thing well.",
    "Let curiosity guide you.",
    "Build. Test. Improve.",
    "Your next idea matters.",
    "Tiny changes add up.",
    "Work with intention.",
    "One move at a time.",
    "Every move counts.",
    "Take your time.",
    "Enjoy the moment.",
    "Stay inspired.",
    "Create your own path.",
};

constexpr std::size_t kMessageCount = sizeof(kMessages) / sizeof(kMessages[0]);
static_assert(kMessageCount >= 50, "At least 50 status messages are required");
}

bool StickyStatus::init()
{
    gpio_config_t charger_enable_config = {};
    charger_enable_config.pin_bit_mask = 1ULL << PIN_BAT_CHG_EN;
    charger_enable_config.mode = GPIO_MODE_OUTPUT;
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_config(&charger_enable_config));
    ESP_ERROR_CHECK_WITHOUT_ABORT(
        gpio_set_level(static_cast<gpio_num_t>(PIN_BAT_CHG_EN), 0));

    gpio_config_t external_power_config = {};
    external_power_config.pin_bit_mask = 1ULL << PIN_EXTERNAL_POWER;
    external_power_config.mode = GPIO_MODE_INPUT;
    ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_config(&external_power_config));

    i2c_master_bus_config_t bus_config = {};
    bus_config.i2c_port = I2C_NUM_1;
    bus_config.sda_io_num = static_cast<gpio_num_t>(PIN_SENSOR_SDA);
    bus_config.scl_io_num = static_cast<gpio_num_t>(PIN_SENSOR_SCL);
    bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
    bus_config.glitch_ignore_cnt = 7;
    bus_config.flags.enable_internal_pullup = 1;

    esp_err_t err = i2c_new_master_bus(&bus_config, &i2c_bus_);
    if (err != ESP_OK) {
        ESP_LOGW(kTag, "Sensor I2C init failed: %s", esp_err_to_name(err));
        return false;
    }

    i2c_device_config_t device_config = {};
    device_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    device_config.device_address = BQ27220_I2C_ADDR;
    device_config.scl_speed_hz = 400000;

    err = i2c_master_bus_add_device(i2c_bus_, &device_config, &battery_device_);
    if (err != ESP_OK) {
        ESP_LOGW(kTag, "BQ27220 device init failed: %s", esp_err_to_name(err));
        return false;
    }

    battery_ready_ = bq27220_probe(battery_device_);
    ESP_LOGI(kTag, "BQ27220 %s", battery_ready_ ? "ready" : "not found");
    return battery_ready_;
}

void StickyStatus::update()
{
    std::size_t next_index = 0;
    if (message_index_ >= kMessageCount) {
        next_index = esp_random() % kMessageCount;
    } else {
        next_index = esp_random() % (kMessageCount - 1);
        if (next_index >= message_index_) {
            ++next_index;
        }
    }
    message_index_ = static_cast<uint8_t>(next_index);
}

bool StickyStatus::poll_battery_percent()
{
    uint16_t percent = 0;
    const bool percent_ok = battery_ready_ &&
        bq27220_read_state_of_charge(battery_device_, percent);

    const int next_percent = percent_ok ? std::min<int>(percent, 100) : battery_percent_;
    const bool changed = next_percent != battery_percent_;

    battery_percent_ = next_percent;
    return changed;
}

bool StickyStatus::poll_external_power()
{
    const bool next_charging =
        gpio_get_level(static_cast<gpio_num_t>(PIN_EXTERNAL_POWER)) != 0;
    const bool changed = next_charging != charging_;

    charging_ = next_charging;
    return changed;
}

const char *StickyStatus::message() const
{
    return message_index_ < kMessageCount ? kMessages[message_index_] : "Keep going.";
}

int StickyStatus::battery_percent() const
{
    return battery_percent_;
}

bool StickyStatus::is_charging() const
{
    return charging_;
}
