#include "hardware/sticky_touch.h"

#include <algorithm>
#include <cstdlib>

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pin_config.h"

namespace {
constexpr const char *kTag = "sticky_touch";
constexpr uint16_t kNativeWidth = 800;
constexpr uint16_t kNativeHeight = 480;
constexpr uint16_t kLogicalWidth = 480;
constexpr uint16_t kLogicalHeight = 800;
constexpr int kMinimumSwipeDistance = 40;
constexpr int kDirectionRatioNumerator = 6;
constexpr int kDirectionRatioDenominator = 5;
constexpr TickType_t kPollInterval = pdMS_TO_TICKS(20);
constexpr uint32_t kTouchTaskStackSize = 4096;
constexpr UBaseType_t kTouchTaskPriority = 5;

uint16_t scale_coordinate(uint16_t value, uint16_t source_max, uint16_t target_max)
{
    const uint16_t clamped = std::min(value, source_max);
    return static_cast<uint16_t>(
        (static_cast<uint32_t>(clamped) * target_max + source_max / 2U) / source_max);
}

TouchEventType classify_touch(uint16_t start_x,
                              uint16_t start_y,
                              uint16_t end_x,
                              uint16_t end_y)
{
    const int dx = static_cast<int>(end_x) - static_cast<int>(start_x);
    const int dy = static_cast<int>(end_y) - static_cast<int>(start_y);
    const int distance_x = std::abs(dx);
    const int distance_y = std::abs(dy);

    if (distance_x < kMinimumSwipeDistance && distance_y < kMinimumSwipeDistance) {
        return TouchEventType::Tap;
    }
    if (distance_x >= kMinimumSwipeDistance &&
        distance_x * kDirectionRatioDenominator >= distance_y * kDirectionRatioNumerator) {
        return dx < 0 ? TouchEventType::SwipeLeft : TouchEventType::SwipeRight;
    }
    if (distance_y >= kMinimumSwipeDistance &&
        distance_y * kDirectionRatioDenominator >= distance_x * kDirectionRatioNumerator) {
        return dy < 0 ? TouchEventType::SwipeUp : TouchEventType::SwipeDown;
    }
    return TouchEventType::None;
}

const char *event_name(TouchEventType type)
{
    switch (type) {
    case TouchEventType::Tap: return "Tap";
    case TouchEventType::SwipeUp: return "SwipeUp";
    case TouchEventType::SwipeDown: return "SwipeDown";
    case TouchEventType::SwipeLeft: return "SwipeLeft";
    case TouchEventType::SwipeRight: return "SwipeRight";
    case TouchEventType::None: return "None";
    }
    return "None";
}
}

bool StickyTouch::init()
{
    gpio_config_t power_config = {};
    power_config.pin_bit_mask = 1ULL << PIN_TOUCH_EN;
    power_config.mode = GPIO_MODE_OUTPUT;
    power_config.intr_type = GPIO_INTR_DISABLE;
    esp_err_t err = gpio_config(&power_config);
    if (err != ESP_OK) {
        ESP_LOGE(kTag, "Touch power GPIO init failed: %s", esp_err_to_name(err));
        return false;
    }
    gpio_set_level(static_cast<gpio_num_t>(PIN_TOUCH_EN), 1);
    vTaskDelay(pdMS_TO_TICKS(250));

    i2c_master_bus_config_t bus_config = {};
    bus_config.i2c_port = I2C_NUM_0;
    bus_config.sda_io_num = static_cast<gpio_num_t>(PIN_TOUCH_SDA);
    bus_config.scl_io_num = static_cast<gpio_num_t>(PIN_TOUCH_SCL);
    bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
    bus_config.glitch_ignore_cnt = 7;
    bus_config.flags.enable_internal_pullup = 1;

    err = i2c_new_master_bus(&bus_config, &i2c_bus_);
    if (err != ESP_OK) {
        ESP_LOGE(kTag, "Touch I2C init failed: %s", esp_err_to_name(err));
        return false;
    }

    if (!controller_.begin(PIN_TOUCH_INT,
                           PIN_TOUCH_RST,
                           kNativeWidth,
                           kNativeHeight,
                           i2c_bus_)) {
        ESP_LOGE(kTag, "GT911 init failed");
        return false;
    }

    uint16_t sensor_width = 0;
    uint16_t sensor_height = 0;
    controller_.readResolution(sensor_width, sensor_height);
    ESP_LOGI(kTag,
             "GT911 ready: sensor=%ux%u logical=%ux%u portrait transform address=0x%02X",
             sensor_width,
             sensor_height,
             kLogicalWidth,
             kLogicalHeight,
             controller_.address());

    event_queue_ = xQueueCreate(1, sizeof(TouchEvent));
    if (event_queue_ == nullptr) {
        ESP_LOGE(kTag, "Touch event queue creation failed");
        return false;
    }

    if (xTaskCreate(touch_task_entry,
                    "sticky_touch",
                    kTouchTaskStackSize,
                    this,
                    kTouchTaskPriority,
                    nullptr) != pdPASS) {
        ESP_LOGE(kTag, "Touch polling task creation failed");
        vQueueDelete(event_queue_);
        event_queue_ = nullptr;
        return false;
    }
    return true;
}

TouchEvent StickyTouch::poll()
{
    TouchEvent event = {};
    if (event_queue_ != nullptr && xQueueReceive(event_queue_, &event, 0) == pdTRUE) {
        return event;
    }
    return {};
}

void StickyTouch::touch_task_entry(void *context)
{
    static_cast<StickyTouch *>(context)->touch_task();
}

void StickyTouch::touch_task()
{
    TickType_t next_poll = xTaskGetTickCount();
    while (true) {
        const TouchEvent event = read_touch();
        if (event.type != TouchEventType::None) {
            xQueueOverwrite(event_queue_, &event);
        }
        vTaskDelayUntil(&next_poll, kPollInterval);
    }
}

TouchEvent StickyTouch::read_touch()
{
    GTPoint point = {};
    const int8_t count = controller_.read_points(&point, 1);

    if (count > 0) {
        // GT911 already maps its native 480x800 sensor into an 800x480 range.
        // Combine the product firmware's touch swap with the display's 270-degree
        // rotation so this layer directly returns portrait 480x800 coordinates.
        const uint16_t logical_x = scale_coordinate(point.x,
                                                     kNativeWidth,
                                                     kLogicalWidth - 1);
        const uint16_t mapped_y = std::min<uint16_t>(point.y, kNativeHeight);
        const uint16_t logical_y = scale_coordinate(kNativeHeight - mapped_y,
                                                     kNativeHeight,
                                                     kLogicalHeight - 1);

        if (!touching_) {
            start_ = {logical_x, logical_y};
            ESP_LOGI(kTag, "touch down: (%u, %u)", start_.x, start_.y);
            touching_ = true;
        }
        last_ = {logical_x, logical_y};
        return {};
    }

    if (count < 0) {
        ESP_LOGW(kTag, "GT911 read failed");
        touching_ = false;
        start_ = {};
        last_ = {};
        return {};
    }

    if (touching_) {
        const Point start = start_;
        const Point end = last_;
        touching_ = false;
        start_ = {};
        last_ = {};
        const TouchEvent event = {
            classify_touch(start.x, start.y, end.x, end.y),
            end.x,
            end.y,
        };
        ESP_LOGI(kTag,
                 "touch release: (%u, %u), delta=(%d, %d), gesture=%s",
                 end.x,
                 end.y,
                 static_cast<int>(end.x) - static_cast<int>(start.x),
                 static_cast<int>(end.y) - static_cast<int>(start.y),
                 event_name(event.type));
        return event;
    }
    return {};
}
