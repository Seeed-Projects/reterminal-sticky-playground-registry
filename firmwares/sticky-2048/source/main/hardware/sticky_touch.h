#pragma once

#include <cstdint>

#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "gt911.h"

enum class TouchEventType {
    None,
    Tap,
    SwipeUp,
    SwipeDown,
    SwipeLeft,
    SwipeRight,
};

struct TouchEvent {
    TouchEventType type = TouchEventType::None;
    uint16_t x = 0;
    uint16_t y = 0;
};

class StickyTouch {
public:
    bool init();
    TouchEvent poll();

private:
    struct Point {
        uint16_t x = 0;
        uint16_t y = 0;
    };

    static void touch_task_entry(void *context);
    void touch_task();
    TouchEvent read_touch();

    i2c_master_bus_handle_t i2c_bus_ = nullptr;
    GT911 controller_;
    QueueHandle_t event_queue_ = nullptr;
    bool touching_ = false;
    Point start_ = {};
    Point last_ = {};
};
