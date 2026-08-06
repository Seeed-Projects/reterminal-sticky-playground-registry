#include "epaper_panel_priv.h"

#include <stddef.h>

#include "esp_check.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "app_debug_log.h"

static const char *TAG = "uc8179";

static uint64_t driver_log_ms(void)
{
    return (uint64_t)(esp_timer_get_time() / 1000ULL);
}

static const char *driver_mode_name(seeed_epaper_refresh_mode_t mode)
{
    return mode == SEEED_EPAPER_REFRESH_PARTIAL ? "partial" : "full";
}

static const uint8_t LUT_VCOM[42] = {
    0x26, 0x0F, 0x18, 0x18, 0x14, 0x01, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const uint8_t LUT_WW[42] = {
    0x55, 0x06, 0x0C, 0x17, 0x02, 0x01, 0x2A, 0x02, 0x1C, 0x02, 0x0D, 0x01,
    0x80, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const uint8_t LUT_KW[42] = {
    0x55, 0x06, 0x0C, 0x17, 0x02, 0x01, 0x2A, 0x02, 0x1C, 0x02, 0x0D, 0x01,
    0x80, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const uint8_t LUT_WK[42] = {
    0xAA, 0x06, 0x0C, 0x17, 0x02, 0x01, 0x15, 0x02, 0x1C, 0x02, 0x0D, 0x01,
    0x40, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const uint8_t LUT_KK[42] = {
    0xAA, 0x06, 0x0C, 0x17, 0x02, 0x01, 0x15, 0x02, 0x1C, 0x02, 0x0D, 0x01,
    0x40, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const uint8_t CMD_USER[6] = {0x17, 0x3F, 0x3F, 0x09, 0x06, 0x16};

static esp_err_t write_reg8(seeed_epaper_panel_t *panel, uint8_t command, uint8_t value)
{
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, command), "uc8179", "write command failed");
    return seeed_epaper_panel_write_data(panel, &value, 1);
}

static esp_err_t write_lut(seeed_epaper_panel_t *panel)
{
    static const struct {
        uint8_t command;
        const uint8_t *data;
    } lut_map[] = {
        {0x20, LUT_VCOM},
        {0x21, LUT_WW},
        {0x22, LUT_KW},
        {0x23, LUT_WK},
        {0x24, LUT_KK},
    };

    for (size_t i = 0; i < sizeof(lut_map) / sizeof(lut_map[0]); i++) {
        APP_EPAPER_DEBUG_LOGI(TAG, "write LUT step %u command=0x%02X", (unsigned)i, lut_map[i].command);
        ESP_RETURN_ON_ERROR(seeed_epaper_panel_wait_ready(panel), "uc8179", "wait ready failed");
        ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, lut_map[i].command), "uc8179", "write command failed");
        ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_data(panel, lut_map[i].data, 42), "uc8179", "write lut failed");
    }
    return ESP_OK;
}

static esp_err_t uc8179_init_fast(seeed_epaper_panel_t *panel)
{
    const uint64_t start_ms = driver_log_ms();
    APP_EPAPER_DEBUG_LOGI(TAG,
             "init_fast start: size=%ux%u",
             (unsigned)panel->info.width,
             (unsigned)panel->info.height);
    const uint8_t resolution[] = {
        (uint8_t)(panel->info.width >> 8),
        (uint8_t)(panel->info.width & 0xFF),
        (uint8_t)(panel->info.height >> 8),
        (uint8_t)(panel->info.height & 0xFF),
    };
    const uint8_t power_setting[] = {0x07, CMD_USER[0], CMD_USER[1], CMD_USER[2], CMD_USER[3]};
    const uint8_t booster[] = {0x17, 0x17, 0x28, 0x17};
    const uint8_t data_interval[] = {0x10, 0x07};

    ESP_RETURN_ON_ERROR(seeed_epaper_panel_reset(panel), "uc8179", "reset failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_wait_ready(panel), "uc8179", "wait ready failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x01), "uc8179", "write command failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_data(panel, power_setting, sizeof(power_setting)), "uc8179", "write data failed");
    ESP_RETURN_ON_ERROR(write_reg8(panel, 0x30, CMD_USER[4]), "uc8179", "write reg failed");
    ESP_RETURN_ON_ERROR(write_reg8(panel, 0x82, CMD_USER[5]), "uc8179", "write reg failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x06), "uc8179", "write command failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_data(panel, booster, sizeof(booster)), "uc8179", "write data failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x04), "uc8179", "write command failed");
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_wait_ready(panel), "uc8179", "wait ready failed");
    ESP_RETURN_ON_ERROR(write_reg8(panel, 0x00, 0x3F), "uc8179", "write reg failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x61), "uc8179", "write command failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_data(panel, resolution, sizeof(resolution)), "uc8179", "write resolution failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x50), "uc8179", "write command failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_data(panel, data_interval, sizeof(data_interval)), "uc8179", "write data interval failed");
    esp_err_t err = write_lut(panel);
    APP_EPAPER_DEBUG_LOGI(TAG,
             "init_fast done: elapsed=%llums err=%s",
             (unsigned long long)(driver_log_ms() - start_ms),
             esp_err_to_name(err));
    return err;
}

static esp_err_t uc8179_init_partial(seeed_epaper_panel_t *panel)
{
    const uint64_t start_ms = driver_log_ms();
    APP_EPAPER_DEBUG_LOGI(TAG, "init_partial start");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_reset(panel), "uc8179", "reset failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_wait_ready(panel), "uc8179", "wait ready failed");
    ESP_RETURN_ON_ERROR(write_reg8(panel, 0x00, 0x1F), "uc8179", "write reg failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x04), "uc8179", "write command failed");
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_wait_ready(panel), "uc8179", "wait ready failed");
    ESP_RETURN_ON_ERROR(write_reg8(panel, 0xE0, 0x02), "uc8179", "write reg failed");
    esp_err_t err = write_reg8(panel, 0xE5, 0x6E);
    APP_EPAPER_DEBUG_LOGI(TAG,
             "init_partial done: elapsed=%llums err=%s",
             (unsigned long long)(driver_log_ms() - start_ms),
             esp_err_to_name(err));
    return err;
}

static esp_err_t uc8179_init_mode(seeed_epaper_panel_t *panel, seeed_epaper_refresh_mode_t mode)
{
    APP_EPAPER_DEBUG_LOGI(TAG, "init_mode: %s", driver_mode_name(mode));
    if (mode == SEEED_EPAPER_REFRESH_PARTIAL) {
        return uc8179_init_partial(panel);
    }
    return uc8179_init_fast(panel);
}

static esp_err_t uc8179_set_window(seeed_epaper_panel_t *panel, const seeed_epaper_area_t *area)
{
    APP_EPAPER_DEBUG_LOGI(TAG,
             "set_window: x=%u y=%u w=%u h=%u",
             (unsigned)area->x,
             (unsigned)area->y,
             (unsigned)area->width,
             (unsigned)area->height);
    const uint16_t x_end = (uint16_t)(area->x + area->width - 1U);
    const uint16_t y_end = (uint16_t)(area->y + area->height - 1U);
    const uint8_t payload[] = {
        (uint8_t)(area->x >> 8),
        (uint8_t)(area->x & 0xFF),
        (uint8_t)(x_end >> 8),
        (uint8_t)(x_end & 0xFF),
        (uint8_t)(area->y >> 8),
        (uint8_t)(area->y & 0xFF),
        (uint8_t)(y_end >> 8),
        (uint8_t)(y_end & 0xFF),
        0x01,
    };
    const uint8_t data_interval[] = {0xA9, 0x07};

    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x50), "uc8179", "write command failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_data(panel, data_interval, sizeof(data_interval)), "uc8179", "write data failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x91), "uc8179", "write command failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x90), "uc8179", "write command failed");
    return seeed_epaper_panel_write_data(panel, payload, sizeof(payload));
}

static esp_err_t uc8179_write_image(seeed_epaper_panel_t *panel, const uint8_t *buffer, size_t len, bool previous_plane)
{
    APP_EPAPER_DEBUG_LOGI(TAG,
             "write_image: plane=%s len=%u",
             previous_plane ? "previous" : "current",
             (unsigned)len);
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, previous_plane ? 0x10 : 0x13), "uc8179", "write command failed");
    return seeed_epaper_panel_write_data(panel, buffer, len);
}

static esp_err_t uc8179_refresh(seeed_epaper_panel_t *panel, seeed_epaper_refresh_mode_t mode)
{
    const uint64_t start_ms = driver_log_ms();
    APP_EPAPER_DEBUG_LOGI(TAG, "refresh start: mode=%s", driver_mode_name(mode));
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x12), "uc8179", "write command failed");
    vTaskDelay(pdMS_TO_TICKS(1));
    esp_err_t err = seeed_epaper_panel_wait_ready(panel);
    APP_EPAPER_DEBUG_LOGI(TAG,
             "refresh done: mode=%s elapsed=%llums err=%s",
             driver_mode_name(mode),
             (unsigned long long)(driver_log_ms() - start_ms),
             esp_err_to_name(err));
    return err;
}

static esp_err_t uc8179_sleep(seeed_epaper_panel_t *panel)
{
    const uint64_t start_ms = driver_log_ms();
    APP_EPAPER_DEBUG_LOGI(TAG, "sleep start");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x50), "uc8179", "write command failed");
    {
        const uint8_t data = 0xF7;
        ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_data(panel, &data, 1), "uc8179", "write data failed");
    }
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x02), "uc8179", "write command failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_wait_ready(panel), "uc8179", "wait ready failed");
    esp_err_t err = write_reg8(panel, 0x07, 0xA5);
    APP_EPAPER_DEBUG_LOGI(TAG,
             "sleep done: elapsed=%llums err=%s",
             (unsigned long long)(driver_log_ms() - start_ms),
             esp_err_to_name(err));
    return err;
}

const seeed_epaper_driver_t *seeed_epaper_uc8179_driver(void)
{
    static const seeed_epaper_driver_t driver = {
        .name = "UC8179",
        .info = {
            .width = 800,
            .height = 480,
            .native_bpp = 1,
            .supports_partial = true,
            .supports_gray4 = true,
            .supports_color = false,
        },
        .init_mode = uc8179_init_mode,
        .set_window = uc8179_set_window,
        .write_image = uc8179_write_image,
        .refresh = uc8179_refresh,
        .sleep = uc8179_sleep,
    };
    return &driver;
}
