#pragma once

#include <cstdint>

#include "driver/i2c_master.h"

class StickyStatus {
public:
    bool init();
    void update();
    bool poll_battery_percent();
    bool poll_external_power();

    const char *message() const;
    int battery_percent() const;
    bool is_charging() const;

private:
    i2c_master_bus_handle_t i2c_bus_ = nullptr;
    i2c_master_dev_handle_t battery_device_ = nullptr;
    bool battery_ready_ = false;
    uint8_t message_index_ = 0xFF;
    int battery_percent_ = -1;
    bool charging_ = false;
};
