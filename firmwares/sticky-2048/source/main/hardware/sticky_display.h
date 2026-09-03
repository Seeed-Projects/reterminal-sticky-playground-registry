#pragma once

#include <cstdint>

#include "driver/spi_master.h"
#include "epaper_panel.h"

class StickyDisplay {
public:
    static constexpr int kWidth = 480;
    static constexpr int kHeight = 800;

    bool init();

    void clear(bool white = true);
    void draw_pixel(int x, int y, bool black = true);
    void draw_rect(int x, int y, int width, int height, bool black = true);
    bool refresh_full();
    bool refresh_partial();

private:
    bool refresh(seeed_epaper_refresh_mode_t mode);

    static constexpr int kNativeWidth = 800;
    static constexpr int kNativeHeight = 480;
    static constexpr int kStrideBytes = kNativeWidth / 8;
    static constexpr int kBufferSize = kStrideBytes * kNativeHeight;

    spi_device_handle_t spi_device_ = nullptr;
    seeed_epaper_panel_handle_t panel_ = nullptr;
    uint8_t *framebuffer_ = nullptr;
};
