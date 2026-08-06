#include "epaper_panel_priv.h"

#include "esp_check.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "app_debug_log.h"

static const char *TAG = "ssd1677";

static uint64_t driver_log_ms(void)
{
    return (uint64_t)(esp_timer_get_time() / 1000ULL);
}

static const char *driver_mode_name(seeed_epaper_refresh_mode_t mode)
{
    switch (mode) {
    case SEEED_EPAPER_REFRESH_PARTIAL:
        return "partial";
    case SEEED_EPAPER_REFRESH_GRAY4:
        return "gray4";
    case SEEED_EPAPER_REFRESH_FULL:
    default:
        return "full";
    }
}

static esp_err_t write_reg8(seeed_epaper_panel_t *panel, uint8_t command, uint8_t value)
{
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, command), "ssd1677", "write command failed");
    return seeed_epaper_panel_write_data(panel, &value, 1);
}

static esp_err_t ssd1677_init_base(seeed_epaper_panel_t *panel, seeed_epaper_refresh_mode_t mode)
{
    const uint64_t start_ms = driver_log_ms();
    APP_EPAPER_DEBUG_LOGI(TAG,
             "init_base start: mode=%s size=%ux%u",
             driver_mode_name(mode),
             (unsigned)panel->info.width,
             (unsigned)panel->info.height);
    const uint8_t softstart[] = {0xAE, 0xC7, 0xC3, 0xC0, 0x80};
    const uint8_t driver_output[] = {
        (uint8_t)((panel->info.height - 1U) & 0xFF),
        (uint8_t)((panel->info.height - 1U) >> 8),
        0x02,
    };
    const uint8_t ram_x[] = {
        0x00,
        0x00,
        (uint8_t)((panel->info.width - 1U) & 0xFF),
        (uint8_t)((panel->info.width - 1U) >> 8),
    };
    const uint8_t ram_y[] = {
        0x00,
        0x00,
        (uint8_t)((panel->info.height - 1U) & 0xFF),
        (uint8_t)((panel->info.height - 1U) >> 8),
    };
    const uint8_t zero2[] = {0x00, 0x00};
    uint8_t border_waveform;
    if (mode == SEEED_EPAPER_REFRESH_GRAY4) {
        border_waveform = 0x00;                         //4_gray_surpport
    } else if (mode == SEEED_EPAPER_REFRESH_PARTIAL) {
        border_waveform = 0x80;
    } else {
        border_waveform = 0x01;
    }

    ESP_RETURN_ON_ERROR(seeed_epaper_panel_reset(panel), "ssd1677", "reset failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_wait_ready(panel), "ssd1677", "wait ready failed");
    if (mode == SEEED_EPAPER_REFRESH_FULL) {
        ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x12), "ssd1677", "write command failed");
        ESP_RETURN_ON_ERROR(seeed_epaper_panel_wait_ready(panel), "ssd1677", "wait ready failed");
    }
    ESP_RETURN_ON_ERROR(write_reg8(panel, 0x18, 0x80), "ssd1677", "write reg failed");
    ESP_RETURN_ON_ERROR(write_reg8(panel, 0x3C, border_waveform), "ssd1677", "write reg failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x0C), "ssd1677", "write command failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_data(panel, softstart, sizeof(softstart)), "ssd1677", "write softstart failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x01), "ssd1677", "write command failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_data(panel, driver_output, sizeof(driver_output)), "ssd1677", "write driver output failed");
    ESP_RETURN_ON_ERROR(write_reg8(panel, 0x11, 0x03), "ssd1677", "write reg failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x44), "ssd1677", "write command failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_data(panel, ram_x, sizeof(ram_x)), "ssd1677", "write ram x failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x45), "ssd1677", "write command failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_data(panel, ram_y, sizeof(ram_y)), "ssd1677", "write ram y failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x4E), "ssd1677", "write command failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_data(panel, zero2, sizeof(zero2)), "ssd1677", "write counter x failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x4F), "ssd1677", "write command failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_data(panel, zero2, sizeof(zero2)), "ssd1677", "write counter y failed");
    esp_err_t err = seeed_epaper_panel_wait_ready(panel);
    APP_EPAPER_DEBUG_LOGI(TAG,
             "init_base done: mode=%s elapsed=%llums err=%s",
             driver_mode_name(mode),
             (unsigned long long)(driver_log_ms() - start_ms),
             esp_err_to_name(err));
    return err;
}

static esp_err_t ssd1677_init_mode(seeed_epaper_panel_t *panel, seeed_epaper_refresh_mode_t mode)
{
    APP_EPAPER_DEBUG_LOGI(TAG, "init_mode: %s", driver_mode_name(mode));
    ESP_RETURN_ON_ERROR(ssd1677_init_base(panel, mode), "ssd1677", "base init failed");
    return ESP_OK;
}

static esp_err_t ssd1677_set_window(seeed_epaper_panel_t *panel, const seeed_epaper_area_t *area)
{
    APP_EPAPER_DEBUG_LOGI(TAG,
             "set_window: x=%u y=%u w=%u h=%u",
             (unsigned)area->x,
             (unsigned)area->y,
             (unsigned)area->width,
             (unsigned)area->height);
    const uint16_t x_end = (uint16_t)(area->x + area->width - 1U);
    const uint16_t y_end = (uint16_t)(area->y + area->height - 1U);
    const uint8_t ram_x[] = {
        (uint8_t)(area->x & 0xFF),
        (uint8_t)(area->x >> 8),
        (uint8_t)(x_end & 0xFF),
        (uint8_t)(x_end >> 8),
    };
    const uint8_t ram_y[] = {
        (uint8_t)(area->y & 0xFF),
        (uint8_t)(area->y >> 8),
        (uint8_t)(y_end & 0xFF),
        (uint8_t)(y_end >> 8),
    };
    const uint8_t cursor_x[] = {
        (uint8_t)(area->x & 0xFF),
        (uint8_t)(area->x >> 8),
    };
    const uint8_t cursor_y[] = {
        (uint8_t)(area->y & 0xFF),
        (uint8_t)(area->y >> 8),
    };

    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x44), "ssd1677", "write command failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_data(panel, ram_x, sizeof(ram_x)), "ssd1677", "write ram x failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x45), "ssd1677", "write command failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_data(panel, ram_y, sizeof(ram_y)), "ssd1677", "write ram y failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x4E), "ssd1677", "write command failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_data(panel, cursor_x, sizeof(cursor_x)), "ssd1677", "write cursor x failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x4F), "ssd1677", "write command failed");
    return seeed_epaper_panel_write_data(panel, cursor_y, sizeof(cursor_y));
}

static esp_err_t ssd1677_write_image(seeed_epaper_panel_t *panel, const uint8_t *buffer, size_t len, bool previous_plane)
{
    APP_EPAPER_DEBUG_LOGI(TAG,
             "write_image: plane=%s len=%u",
             previous_plane ? "previous" : "current",
             (unsigned)len);
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, previous_plane ? 0x26 : 0x24), "ssd1677", "write command failed");
    return seeed_epaper_panel_write_data(panel, buffer, len);
}

static esp_err_t ssd1677_refresh(seeed_epaper_panel_t *panel, seeed_epaper_refresh_mode_t mode)
{
    const uint64_t start_ms = driver_log_ms();
    APP_EPAPER_DEBUG_LOGI(TAG, "refresh start: mode=%s", driver_mode_name(mode));

    //4_gray_surpport: forced temperature before gray4 refresh, ref: Force_temprature_EPD_by_OTP_Update
    if (mode == SEEED_EPAPER_REFRESH_GRAY4) {
        const uint8_t temp_comp[] = {0x67, 0x00};
        ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x1A),
                            "ssd1677", "write temp command failed");
        ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_data(panel, temp_comp, sizeof(temp_comp)),
                            "ssd1677", "write temp data failed");
    }

    uint8_t value;
    if (mode == SEEED_EPAPER_REFRESH_GRAY4) {
        value = 0xD7;
    } else if (mode == SEEED_EPAPER_REFRESH_PARTIAL) {
        value = 0xFF;
    } else {
        value = 0xF7;
    }
    ESP_RETURN_ON_ERROR(write_reg8(panel, 0x22, value), "ssd1677", "write reg failed");
    ESP_RETURN_ON_ERROR(seeed_epaper_panel_write_command(panel, 0x20), "ssd1677", "write command failed");
    esp_err_t err = seeed_epaper_panel_wait_ready(panel);
    APP_EPAPER_DEBUG_LOGI(TAG,
             "refresh done: mode=%s elapsed=%llums err=%s",
             driver_mode_name(mode),
             (unsigned long long)(driver_log_ms() - start_ms),
             esp_err_to_name(err));
    return err;
}

static esp_err_t ssd1677_sleep(seeed_epaper_panel_t *panel)
{
    const uint64_t start_ms = driver_log_ms();
    APP_EPAPER_DEBUG_LOGI(TAG, "sleep start");
    ESP_RETURN_ON_ERROR(write_reg8(panel, 0x10, 0x03), "ssd1677", "write reg failed");
    vTaskDelay(pdMS_TO_TICKS(100));
    APP_EPAPER_DEBUG_LOGI(TAG,
             "sleep done: elapsed=%llums",
             (unsigned long long)(driver_log_ms() - start_ms));
    return ESP_OK;
}

const seeed_epaper_driver_t *seeed_epaper_ssd1677_driver(void)
{
    static const seeed_epaper_driver_t driver = {
        .name = "SSD1677",
        .info = {
            .width = 800,
            .height = 480,
            .native_bpp = 1,
            .supports_partial = true,
            .supports_gray4 = true,       //4_gray_surpport
            .supports_color = false,
        },
        .init_mode = ssd1677_init_mode,
        .set_window = ssd1677_set_window,
        .write_image = ssd1677_write_image,
        .refresh = ssd1677_refresh,
        .sleep = ssd1677_sleep,
    };
    return &driver;
}
