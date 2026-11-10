# Sticky Calculator

## What it is

A retro and modern calculator demo for reTerminal Sticky with TI-2500 Datamath and smartphone-style calculator interfaces.

## Source

- Project: https://github.com/Lukilyy/reterminal-sticky-calculator
- License: MIT

## Package

- Version: 2.0.0
- `reterminal-sticky-calculator-v2.0.0.bin` at `0x0` (549168 bytes; ESP32-S3 / DIO / 80 MHz / 32 MB)
- Origin: byte-identical to the [v2.0.0 Release asset](https://github.com/Lukilyy/reterminal-sticky-calculator/releases/tag/v2.0.0).
- Version 1.0.0 remains available:
- `reterminal-sticky-calculator-v1.0.0.bin` at 0x0 (531392 bytes)

## Changes in 2.0.0

- Automatic SSD1677 / SSD2677 display support.
- Fixed duplicate GPIO ISR initialization, periodic full refresh after mode switches, and history synchronization.
- Clean touch-polling shutdown before power-off; wake-up hold confirmation reduced to 500 ms.

Hold the AI button until the startup beep to open Calculator.

## Physical-device test

Version `2.0.0` was tested successfully on a physical reTerminal Sticky by the author.

Submitted by [Lukilyy](https://github.com/Lukilyy).
