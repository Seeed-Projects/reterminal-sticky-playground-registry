#pragma once

#include "sdkconfig.h"

#include "esp_log.h"

#if defined(CONFIG_APP_DEBUG_EPAPER_TOUCH_LOG_ENABLED)
#define APP_DEBUG_EPAPER_TOUCH_LOG_ENABLED 1
#else
#define APP_DEBUG_EPAPER_TOUCH_LOG_ENABLED 0
#endif

#define APP_TOUCH_DEBUG_LOGE(tag, format, ...) \
    do { \
        if (APP_DEBUG_EPAPER_TOUCH_LOG_ENABLED) { \
            ESP_LOGE(tag, "[touch-debug] " format, ##__VA_ARGS__); \
        } \
    } while (0)
#define APP_TOUCH_DEBUG_LOGW(tag, format, ...) \
    do { \
        if (APP_DEBUG_EPAPER_TOUCH_LOG_ENABLED) { \
            ESP_LOGW(tag, "[touch-debug] " format, ##__VA_ARGS__); \
        } \
    } while (0)
#define APP_TOUCH_DEBUG_LOGI(tag, format, ...) \
    do { \
        if (APP_DEBUG_EPAPER_TOUCH_LOG_ENABLED) { \
            ESP_LOGI(tag, "[touch-debug] " format, ##__VA_ARGS__); \
        } \
    } while (0)
#define APP_TOUCH_DEBUG_LOGD(tag, format, ...) \
    do { \
        if (APP_DEBUG_EPAPER_TOUCH_LOG_ENABLED) { \
            ESP_LOGD(tag, "[touch-debug] " format, ##__VA_ARGS__); \
        } \
    } while (0)

#define APP_EPAPER_DEBUG_LOGE(tag, format, ...) \
    do { \
        if (APP_DEBUG_EPAPER_TOUCH_LOG_ENABLED) { \
            ESP_LOGE(tag, "[epaper-debug] " format, ##__VA_ARGS__); \
        } \
    } while (0)
#define APP_EPAPER_DEBUG_LOGW(tag, format, ...) \
    do { \
        if (APP_DEBUG_EPAPER_TOUCH_LOG_ENABLED) { \
            ESP_LOGW(tag, "[epaper-debug] " format, ##__VA_ARGS__); \
        } \
    } while (0)
#define APP_EPAPER_DEBUG_LOGI(tag, format, ...) \
    do { \
        if (APP_DEBUG_EPAPER_TOUCH_LOG_ENABLED) { \
            ESP_LOGI(tag, "[epaper-debug] " format, ##__VA_ARGS__); \
        } \
    } while (0)
#define APP_EPAPER_DEBUG_LOGD(tag, format, ...) \
    do { \
        if (APP_DEBUG_EPAPER_TOUCH_LOG_ENABLED) { \
            ESP_LOGD(tag, "[epaper-debug] " format, ##__VA_ARGS__); \
        } \
    } while (0)
