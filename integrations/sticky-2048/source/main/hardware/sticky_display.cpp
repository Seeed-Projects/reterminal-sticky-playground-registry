#include "hardware/sticky_display.h"

#include <cstring>

#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "pin_config.h"

namespace {
constexpr const char *kTag = "sticky_display";
}

bool StickyDisplay::init()
{
    spi_bus_config_t bus_config = {};
    bus_config.mosi_io_num = PIN_EPD_MOSI;
    bus_config.miso_io_num = PIN_EPD_MISO;
    bus_config.sclk_io_num = PIN_EPD_CLK;
    bus_config.max_transfer_sz = kBufferSize;

    esp_err_t err = spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(kTag, "SPI bus init failed: %s", esp_err_to_name(err));
        return false;
    }

    spi_device_interface_config_t device_config = {};
    device_config.clock_speed_hz = 10 * 1000 * 1000;
    device_config.mode = 0;
    device_config.spics_io_num = PIN_EPD_CS;
    device_config.queue_size = 1;

    err = spi_bus_add_device(SPI2_HOST, &device_config, &spi_device_);
    if (err != ESP_OK) {
        ESP_LOGE(kTag, "SPI device init failed: %s", esp_err_to_name(err));
        return false;
    }

    seeed_epaper_panel_config_t panel_config = {};
    panel_config.spi_handle = spi_device_;
    panel_config.pin_dc = static_cast<gpio_num_t>(PIN_EPD_DC);
    panel_config.pin_rst = static_cast<gpio_num_t>(PIN_EPD_RST);
    panel_config.pin_busy = static_cast<gpio_num_t>(PIN_EPD_BUSY);
    panel_config.pin_enable = static_cast<gpio_num_t>(PIN_EPD_EN);
    panel_config.busy_timeout_ms = 10000;
    panel_config.reset_low_ms = 10;
    panel_config.reset_high_ms = 10;
    panel_config.busy_level = 1;
    panel_config.enable_level = 1;
    panel_config.mirror_x = true;

    err = seeed_epaper_new_panel(SEEED_EPAPER_PANEL_SSD1677, &panel_config, &panel_);
    if (err != ESP_OK) {
        ESP_LOGE(kTag, "Panel init failed: %s", esp_err_to_name(err));
        return false;
    }

    framebuffer_ = static_cast<uint8_t *>(
        heap_caps_malloc(kBufferSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (framebuffer_ == nullptr) {
        framebuffer_ = static_cast<uint8_t *>(heap_caps_malloc(kBufferSize, MALLOC_CAP_8BIT));
    }
    if (framebuffer_ == nullptr) {
        ESP_LOGE(kTag, "Framebuffer allocation failed");
        return false;
    }

    clear();
    ESP_LOGI(kTag, "Display ready: logical 480x800, native 800x480, rotation 270");
    return true;
}

void StickyDisplay::clear(bool white)
{
    if (framebuffer_ != nullptr) {
        std::memset(framebuffer_, white ? 0xFF : 0x00, kBufferSize);
    }
}

void StickyDisplay::draw_pixel(int x, int y, bool black)
{
    if (framebuffer_ == nullptr || x < 0 || x >= kWidth || y < 0 || y >= kHeight) {
        return;
    }

    // Rotate the logical 480x800 canvas by 270 degrees into the native 800x480 buffer.
    const int native_x = kNativeWidth - y - 1;
    const int native_y = x;
    uint8_t &byte = framebuffer_[native_y * kStrideBytes + native_x / 8];
    const uint8_t mask = static_cast<uint8_t>(0x80U >> (native_x % 8));

    if (black) {
        byte &= static_cast<uint8_t>(~mask);
    } else {
        byte |= mask;
    }
}

void StickyDisplay::draw_rect(int x, int y, int width, int height, bool black)
{
    if (width <= 0 || height <= 0) {
        return;
    }

    for (int column = x; column < x + width; ++column) {
        draw_pixel(column, y, black);
        draw_pixel(column, y + height - 1, black);
    }
    for (int row = y; row < y + height; ++row) {
        draw_pixel(x, row, black);
        draw_pixel(x + width - 1, row, black);
    }
}

bool StickyDisplay::refresh_full()
{
    return refresh(SEEED_EPAPER_REFRESH_FULL);
}

bool StickyDisplay::refresh_partial()
{
    return refresh(SEEED_EPAPER_REFRESH_PARTIAL);
}

bool StickyDisplay::refresh(seeed_epaper_refresh_mode_t mode)
{
    if (panel_ == nullptr || framebuffer_ == nullptr) {
        return false;
    }

    const seeed_epaper_area_t area = {0, 0, kNativeWidth, kNativeHeight};
    esp_err_t err = seeed_epaper_panel_prepare(panel_, mode);
    if (err == ESP_OK) {
        err = seeed_epaper_panel_write_bitmap(panel_,
                                               &area,
                                               framebuffer_,
                                               kStrideBytes,
                                               SEEED_EPAPER_PIXEL_FORMAT_MONO1_MSB,
                                               mode);
    }
    if (err == ESP_OK) {
        err = seeed_epaper_panel_commit(panel_, &area, mode);
    }

    if (err != ESP_OK) {
        ESP_LOGE(kTag,
                 "%s refresh failed: %s",
                 mode == SEEED_EPAPER_REFRESH_PARTIAL ? "Partial" : "Full",
                 esp_err_to_name(err));
        return false;
    }
    return true;
}
