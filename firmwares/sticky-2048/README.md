# Sticky 2048

Sticky 2048 is a community 2048 game for Seeed Studio reTerminal Sticky. It
uses native ESP-IDF and the device's E-Ink display, touch controller, AI button,
battery gauge, and non-volatile storage.

The game renders directly to a 1-bit E-paper framebuffer. It does not depend on
LVGL, networking, cloud services, or a filesystem.

## Features

- Native ESP-IDF implementation
- Portrait 480 × 800 interface with automatic SSD1677 / SSD2677 detection
- 2048 game logic implemented in C++
- GT911 touch swipe control with an independent polling task
- Partial refresh during gameplay with periodic full-refresh cleanup
- AI button support
  - Short press starts a new game
  - Long press saves the current game state and powers off
- BQ27220 battery percentage display
- External-power detection
- NVS persistence for the best score and current game state
- Optional LEDC buzzer interface

## Controls

- Swipe up, down, left, or right to move the tiles.
- Tap **NEW GAME** to restart the game.
- Short-press the AI button to start a new game.
- Long-press the AI button to save the current game and power off.

## Hardware requirements

- Seeed Studio reTerminal Sticky
- ESP32-S3 with PSRAM
- 3.97-inch 800 × 480 E-Ink display
- USB data cable for building and flashing

## Firmware package

- Registry firmware version: `2.0.0`
- Source and release: <https://github.com/Lukilyy/reterminal-sticky-2048-eink-game/releases/tag/v2.0.0>
- License: MIT
- Target: ESP32-S3; 32 MB flash, DIO, 80 MHz
- File: `firmware/2.0.0/sticky-2048-v2.0.0.bin` (429,152 bytes), flash at `0x0`.
- SHA-256: `d3d553bc5c3bb6050ab91ece909a5a6ea93323a98d797cd3e48170b5fec5c8c1`
- Package origin: author-provided merged BIN, byte-identical to the v2.0.0 Release asset. Includes the bootloader, partition table and application.

## Changes in 2.0.0

- Automatic SSD1677 / SSD2677 screen detection.
- Fixed display startup with a MicroSD card inserted.
- Fixed AI-button wake while USB power is connected.
- Fixed I2C errors when entering sleep.

## Installation

Select version `2.0.0` on the Playground flashing page and connect Sticky with a USB data cable. For manual installation, flash the merged BIN at `0x0` on ESP32-S3. Source code is maintained in the upstream project linked above.

## Hardware mapping

| Function | GPIO |
|---|---:|
| AI / power button | 4 |
| External-power detect | 9 |
| E-Paper SPI | 13–18 |
| GT911 I2C | 2, 3 |
| GT911 enable / interrupt / reset | 42, 21, 41 |
| BQ27220 I2C | 0, 1 |
| Charger enable | 39 |
| Buzzer PWM | 48 |
| Power hold / lock | 45, 46 |

## Physical-device test record

Firmware version `2.0.0` was tested on a physical reTerminal Sticky by the author, who confirmed that it works correctly.

## Scope

This project focuses on the core 2048 gameplay experience and E-Ink interaction
on reTerminal Sticky. It does not include Wi-Fi, BLE, OTA, MQTT, cloud services,
LVGL, or unrelated application features.

## License

The upstream project is available under the [MIT License](https://github.com/Lukilyy/reterminal-sticky-2048-eink-game/blob/v2.0.0/LICENSE).
