# SD Card Debugger

## What it is

Inspect, diagnose, manage, and initialize SD cards directly on reTerminal Sticky.

## Source

- Project: https://github.com/Lukilyy/SD-Card-Debugger
- License: MIT

## Package

- Version: 2.0.0
- `sd-card-debugger-v2.0.0.bin` at `0x0` (583472 bytes; ESP32-S3 / DIO / 80 MHz / 32 MB)
- Origin: byte-identical to the [v2.0.0 Release asset](https://github.com/Lukilyy/SD-Card-Debugger/releases/tag/v2.0.0).
- Version 1.0.0 remains available:
- `sticky_sd_card_debugger_merged.bin` at 0x0 (552048 bytes)

## Changes in 2.0.0

- Automatic SSD1677 / SSD2677 display detection.
- Stronger card-change protection, input filtering, and resource-release diagnostics.
- Fixed sleep/wake button handling, added power tones, and fixed SSD2677 shutdown-screen speckling.

Hold AI until the startup beep to wake; hold it for about one second to enter Deep Sleep.

## Physical-device test

The `2.0.0` release firmware was tested successfully by the author on physical reTerminal Sticky devices with SSD1677 and SSD2677 displays.

Submitted by [Lukilyy](https://github.com/Lukilyy).
