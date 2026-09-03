#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"

#ifndef GPIO_NUM_NC
#define GPIO_NUM_NC ((gpio_num_t)-1)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct seeed_epaper_panel_t *seeed_epaper_panel_handle_t;

typedef enum {
    SEEED_EPAPER_PANEL_UC8179 = 0,
    SEEED_EPAPER_PANEL_SSD1677,
} seeed_epaper_panel_model_t;

typedef enum {
    SEEED_EPAPER_REFRESH_FULL = 0,
    SEEED_EPAPER_REFRESH_PARTIAL,
    SEEED_EPAPER_REFRESH_GRAY4,  //4_gray_surpport
} seeed_epaper_refresh_mode_t;

typedef enum {
    SEEED_EPAPER_PIXEL_FORMAT_MONO1_MSB = 0,
    SEEED_EPAPER_PIXEL_FORMAT_GRAY4,
    SEEED_EPAPER_PIXEL_FORMAT_GRAY8,
    SEEED_EPAPER_PIXEL_FORMAT_RGB565,
} seeed_epaper_pixel_format_t;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
} seeed_epaper_area_t;

typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t native_bpp;
    bool supports_partial;
    bool supports_gray4;
    bool supports_color;
} seeed_epaper_panel_info_t;

typedef struct {
    spi_device_handle_t spi_handle;
    gpio_num_t pin_dc;
    gpio_num_t pin_rst;
    gpio_num_t pin_busy;
    gpio_num_t pin_enable;
    uint32_t busy_timeout_ms;
    uint16_t reset_low_ms;
    uint16_t reset_high_ms;
    uint8_t busy_level;
    uint8_t enable_level;
    bool mirror_x;
} seeed_epaper_panel_config_t;

esp_err_t seeed_epaper_new_panel(seeed_epaper_panel_model_t model,
                                 const seeed_epaper_panel_config_t *config,
                                 seeed_epaper_panel_handle_t *ret_panel);

esp_err_t seeed_epaper_panel_del(seeed_epaper_panel_handle_t panel);

esp_err_t seeed_epaper_panel_get_info(seeed_epaper_panel_handle_t panel,
                                      seeed_epaper_panel_info_t *ret_info);

esp_err_t seeed_epaper_panel_prepare(seeed_epaper_panel_handle_t panel,
                                     seeed_epaper_refresh_mode_t mode);

esp_err_t seeed_epaper_panel_write_bitmap(seeed_epaper_panel_handle_t panel,
                                          const seeed_epaper_area_t *area,
                                          const void *pixels,
                                          size_t src_stride_bytes,
                                          seeed_epaper_pixel_format_t input_format,
                                          seeed_epaper_refresh_mode_t mode);

esp_err_t seeed_epaper_panel_write_bitmap_diff(seeed_epaper_panel_handle_t panel,
                                               const seeed_epaper_area_t *area,
                                               const void *previous_pixels,
                                               const void *current_pixels,
                                               size_t src_stride_bytes,
                                               seeed_epaper_pixel_format_t input_format,
                                               seeed_epaper_refresh_mode_t mode);

//4_gray_surpport: write 2bpp gray4 data
esp_err_t seeed_epaper_panel_write_bitmap_gray4(
    seeed_epaper_panel_handle_t panel,
    const seeed_epaper_area_t *area,
    const uint8_t *pixels,
    size_t src_stride_bytes);

esp_err_t seeed_epaper_panel_commit(seeed_epaper_panel_handle_t panel,
                                    const seeed_epaper_area_t *area,
                                    seeed_epaper_refresh_mode_t mode);

esp_err_t seeed_epaper_panel_refresh_area(seeed_epaper_panel_handle_t panel,
                                          const seeed_epaper_area_t *area,
                                          const void *pixels,
                                          size_t src_stride_bytes,
                                          seeed_epaper_pixel_format_t input_format,
                                          seeed_epaper_refresh_mode_t mode);

esp_err_t seeed_epaper_panel_clear(seeed_epaper_panel_handle_t panel,
                                   bool white,
                                   seeed_epaper_refresh_mode_t mode);

esp_err_t seeed_epaper_panel_sleep(seeed_epaper_panel_handle_t panel);

esp_err_t seeed_epaper_panel_wakeup(seeed_epaper_panel_handle_t panel,
                                    seeed_epaper_refresh_mode_t mode);

bool seeed_epaper_panel_busy_wait_active(void);

#ifdef __cplusplus
}
#endif
