# ScreenTinker

This community firmware contribution packages ScreenTinker 1.4.4 for browser-based installation on Seeed Studio reTerminal Sticky.

The package was built from the official [ScreenTinker MCU repository](https://github.com/renebohne/screentinker_mcu) release tag `v1.4.4` (commit `8f55141`) and is distributed under the upstream MIT license.

## Overview

ScreenTinker turns reTerminal Sticky into an ultra-low-power, wireless digital signage display and information dashboard. It communicates with any self-hosted or cloud ScreenTinker server instance.

Key features include:
- **Dual E-Paper Display Controller Support (SSD1677 & SSD2677):** Automatic runtime probe detection queries register `0x70` over 3-wire half-duplex SPI to automatically identify newer SSD2677 revisions (`chip_id = 0x07`) versus original SSD1677 displays, adjusting BUSY polarity (Active LOW vs Active HIGH) and waveform initialization on the fly.
- **Direct 1-Bit & Gray4 E-Paper Streaming:** Zero MCU render overhead; the server provides pre-dithered, packed 1-bit monochrome frames or 4-level grayscale (2bpp Gray4) frames.
- **Human-Friendly Serial CLI & JSON Export:** Full interactive command-line interface over 115200 baud USB serial (`help`, `status`, `get all`, `set <key> <val>`, `scan`, `refresh`, `reboot`, etc.) alongside full configuration export (`export` / `export pretty`) with automatic Wi-Fi password masking.
- **Capacitive Touchscreen Navigation:** GT911 hardware initialization and direct touch hit-testing for System Menu items and tap-to-return from Status screen. Placed into low-power sleep mode during slide display to conserve battery.
- **TI BQ25616 Battery Charging & Power Protection:** Automatic battery charging management held active across deep sleep so connected batteries charge while running or sleeping. Enhanced 5% critical battery shutdown cutoff protects against brownout loops.
- **8 MB PSRAM Frame Caching:** Stores up to 32 frames in memory for instant (< 2s) button browsing (`UP`/`DOWN`).
- **Multi-Zone Layout Rendering:** Switch between standard Fullscreen slides and multi-zone layout rendering directly from the On-Device System Menu, Captive Portal, or WebSerial commands. Automatically queries `/api/embedded/render-layout` to render complex screen layouts divided into multiple independent zones.
- **Interactive On-Screen System Menu:** Hold the `OK` button for 1.5 seconds to open the menu with options: *Status & Info*, *Wi-Fi TX Power* (19.5 / 15 / 11 dBm), *Language* (EN / DE), *Unpair Device*, *Power Off*, *Factory Reset*, and *Close*.
- **Hardware Power-Off Latch & QR Code Screen:** Releases battery power latches for 0 µA shutdown and displays a dedicated power-off screen with a scannable QR Code and GitHub repository link.
- **Adaptive Deep Sleep & ETag Caching:** Automatically skips display refresh and enters deep sleep (~15 µA) when content has not changed (`HTTP 304`), dynamically extending sleep intervals up to 30 minutes. Optional `wifi_always_on` mode is available for line-powered operation.
- **Instant Hardware RTC Button Interrupts:** Wakes from deep sleep in milliseconds upon pressing `OK`, `UP`, or `DOWN`.
- **Environmental Telemetry & Fuel Gauge:** Reports Sensirion SHT40 temperature/humidity, TI BQ27220 fuel gauge battery percentage/voltage, and charging state telemetry headers.
- **Dual-Partition Over-The-Air (OTA) Updates:** Dual-partition A/B firmware update engine with automated boot rollback protection.
- **Rolling Diagnostic Log Buffer:** 100-entry in-memory ring buffer with remote log upload via `X-ST-Command: upload_logs`.
- **Dual Onboarding / Provisioning:**
  - **Option A (Wi-Fi Hotspot):** Out of the box, broadcasts `ScreenTinker-Setup` for zero-tool smartphone configuration via captive portal (`192.168.4.1`). Layout mode can be selected during setup.
  - **Option B (WebSerial Installer):** Configure directly in Chrome/Edge via USB using the WebSerial Config Protocol (WSCP) v1.0.
- **Zero-Hardcoding 6-Digit Pairing:** Safe CSPRNG pairing flow with one-time claim token.
- **Bilingual Support:** Dynamic language switching (English default / German toggle) across all system screens and menus.

## Physical Device Test Record

The packaged build was thoroughly verified on physical production hardware across both controller revisions:
- **Hardware Tested:**
  - **Unit 1 (SSD2677 Revision):** Seeed Studio reTerminal Sticky (ESP32-S3R8, 8MB Octal Flash, 8MB Octal PSRAM, 3.97" 800×480 SSD2677 E-Paper, Active-LOW BUSY).
  - **Unit 2 (SSD1677 Revision):** Seeed Studio reTerminal Sticky (ESP32-S3R8, 8MB Octal Flash, 8MB Octal PSRAM, 3.97" 800×480 SSD1677 E-Paper, Active-HIGH BUSY).
- **Firmware Version:** 1.4.4
- **Test Results:**
  - Bootloader and partition flashing via WebSerial browser installer verified on both units.
  - Automatic SPI runtime probing (`0x70`) successfully identified `SSD2677` (Unit 1, full refresh ~1391 ms) and `SSD1677` (Unit 2, full refresh ~1918 ms).
  - Verified horizontal orientation and mirroring symmetry across onboarding, boot splash, and content playback on both controller variants.
  - Smartphone Captive Portal configuration (`192.168.4.1`) tested and verified.
  - Interactive USB Serial CLI (`help`, `status`, `get all`, `export pretty`) verified at 115200 baud.
  - 6-digit server pairing handshake (`/api/embedded/pair/register` and `/status`) verified.
  - Dynamic PSRAM caching and button navigation (`UP`, `DOWN`, `OK`) verified.
  - Long press (1.5s on OK button) interactive System Menu with Wi-Fi Power and Status Screen verified.
  - Capacitive touch hit-testing on System Menu and tap-to-return from Status screen verified.
  - Power Off action with QR Code screen and 0 µA battery latch release verified.
  - Wakeup from power-off using OK button verified.
  - ETag HTTP 304 cache validation, adaptive deep sleep, and RTC button interrupts verified.
  - Battery charging via TI BQ25616 verified across runtime and deep sleep.
  - Factory reset and NVS wiping from menu verified.

## Links & Support

- **Firmware Source & Releases:** [https://github.com/renebohne/screentinker_mcu](https://github.com/renebohne/screentinker_mcu)
- **Release v1.4.4:** [https://github.com/renebohne/screentinker_mcu/releases/tag/v1.4.4](https://github.com/renebohne/screentinker_mcu/releases/tag/v1.4.4)
- **Release v1.3.1:** [https://github.com/renebohne/screentinker_mcu/releases/tag/v1.3.1](https://github.com/renebohne/screentinker_mcu/releases/tag/v1.3.1)
- **Release v1.1.0:** [https://github.com/renebohne/screentinker_mcu/releases/tag/v1.1.0](https://github.com/renebohne/screentinker_mcu/releases/tag/v1.1.0)
- **Server Source:** [https://github.com/screentinker/screentinker](https://github.com/screentinker/screentinker)
- **Issue Tracker:** [https://github.com/renebohne/screentinker_mcu/issues](https://github.com/renebohne/screentinker_mcu/issues)
