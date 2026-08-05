#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "epaper_panel.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#ifndef GPIO_NUM_NC
#define GPIO_NUM_NC ((gpio_num_t)-1)
#endif

typedef struct seeed_epaper_driver_t seeed_epaper_driver_t;

typedef struct seeed_epaper_panel_t {
    seeed_epaper_panel_model_t model;
    seeed_epaper_panel_config_t config;
    seeed_epaper_panel_info_t info;
    const seeed_epaper_driver_t *driver;
    bool prepared;
    bool sleeping;
    bool partial_ready;
    seeed_epaper_refresh_mode_t active_mode;
    bool dirty_valid;
    seeed_epaper_area_t dirty_area;
    uint8_t *framebuffer;
    size_t framebuffer_stride;
    size_t framebuffer_len;
    uint8_t *spi_tx_buffer;
    size_t spi_tx_buffer_len;
    SemaphoreHandle_t busy_sem;
    bool busy_isr_attached;
} seeed_epaper_panel_t;

struct seeed_epaper_driver_t {
    const char *name;
    seeed_epaper_panel_info_t info;
    esp_err_t (*init_mode)(seeed_epaper_panel_t *panel, seeed_epaper_refresh_mode_t mode);
    esp_err_t (*set_window)(seeed_epaper_panel_t *panel, const seeed_epaper_area_t *area);
    esp_err_t (*write_image)(seeed_epaper_panel_t *panel, const uint8_t *buffer, size_t len, bool previous_plane);
    esp_err_t (*refresh)(seeed_epaper_panel_t *panel, seeed_epaper_refresh_mode_t mode);
    esp_err_t (*sleep)(seeed_epaper_panel_t *panel);
};

esp_err_t seeed_epaper_panel_reset(seeed_epaper_panel_t *panel);
esp_err_t seeed_epaper_panel_wait_ready(seeed_epaper_panel_t *panel);
esp_err_t seeed_epaper_panel_write_command(seeed_epaper_panel_t *panel, uint8_t command);
esp_err_t seeed_epaper_panel_write_data(seeed_epaper_panel_t *panel, const void *data, size_t len);
void seeed_epaper_panel_set_dirty(seeed_epaper_panel_t *panel, const seeed_epaper_area_t *area);
bool seeed_epaper_panel_resolve_area(seeed_epaper_panel_t *panel,
                                     const seeed_epaper_area_t *requested,
                                     seeed_epaper_area_t *resolved);
void seeed_epaper_panel_mirror_x(uint8_t *buffer, uint16_t width, uint16_t height);

const seeed_epaper_driver_t *seeed_epaper_uc8179_driver(void);
const seeed_epaper_driver_t *seeed_epaper_ssd1677_driver(void);
