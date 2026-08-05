# SEEED_EPAPER

`SEEED_EPAPER` 是一个面向 ESP-IDF 的模块化墨水屏组件，当前先支持：

- `UC8179`：reTerminal E1001
- `SSD1677`：reTerminal E1005

设计目标：

- SPI 总线由外部创建，组件只接收 `spi_device_handle_t`
- `panel` 核心与 `driver` 控制器实现分层，后续加新屏只需补 driver
- 对外统一支持整屏刷新、任意区域局刷、1bpp 原生输入和 4/8/16bit 转 1bpp
- 为后续灰度 / 彩色扩展预留能力描述与像素格式枚举

## 对外 API

- `seeed_epaper_new_panel()`：创建面板句柄
- `seeed_epaper_panel_prepare()`：准备一次 full / partial 会话
- `seeed_epaper_panel_write_bitmap()`：写入一块区域到显存
- `seeed_epaper_panel_commit()`：触发物理刷新
- `seeed_epaper_panel_refresh_area()`：单次区域写入并刷新
- `seeed_epaper_panel_sleep()`：进入休眠

推荐给 LVGL9 的接法：

1. `flush_cb` 中对每个 chunk 调 `prepare + write_bitmap`
2. `lv_display_flush_is_last(display)` 为真时再 `commit`
3. 首帧走 `FULL`，后续走 `PARTIAL`
4. `LV_COLOR_DEPTH=1` 时直接传 `MONO1_MSB`
5. `LV_COLOR_DEPTH=8/16` 时传 `GRAY8` / `RGB565`，组件内部阈值转换为 1bpp

## 目录结构

- `epaper_panel.h`：统一公共接口
- `epaper_panel.c`：GPIO / SPI / 像素转换 / 会话管理
- `driver/uc8179.c`：UC8179 控制器实现
- `driver/ssd1677.c`：SSD1677 控制器实现

