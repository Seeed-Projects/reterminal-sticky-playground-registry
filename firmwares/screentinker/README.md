# ScreenTinker

This community firmware contribution packages ScreenTinker 1.1.0 for browser-based installation on Seeed Studio reTerminal Sticky.

The package was built from the official [ScreenTinker MCU repository](https://github.com/renebohne/screentinker_mcu) release tag `v1.1.0` (commit `9e9ddf7`) and is distributed under the upstream MIT license.

## Overview

ScreenTinker turns reTerminal Sticky into an ultra-low-power, wireless digital signage display and information dashboard. It communicates with any self-hosted or cloud ScreenTinker server instance.

Key features include:
- **Direct 1-Bit SSD1677 Bitstream Streaming:** Zero MCU render overhead; the server provides pre-dithered, packed 1-bit monochome frames.
- **8 MB PSRAM Frame Caching:** Stores up to 32 frames in memory for instant (< 2s) button browsing (`UP`/`DOWN`).
- **Multi-Zone Layout Rendering:** Switch between standard Fullscreen slides and multi-zone layout rendering directly from the On-Device System Menu, Captive Portal, or WebSerial commands. Automatically queries `/api/embedded/render-layout` to render complex screen layouts divided into multiple independent zones.
- **Interactive On-Screen System Menu:** Hold the `OK` button for 1.5 seconds to open the menu with options: *Status & Info*, *Wi-Fi TX Power* (19.5 / 15 / 11 dBm), *Language* (EN / DE), *Unpair Device*, *Power Off*, *Factory Reset*, and *Close*.
- **Hardware Power-Off Latch & QR Code Screen:** Releases battery power latches for 0 µA shutdown and displays a dedicated power-off screen with a scannable QR Code and GitHub repository link.
- **Adaptive Deep Sleep & ETag Caching:** Automatically skips display refresh and enters deep sleep (~15 µA) when content has not changed (`HTTP 304`), dynamically extending sleep intervals up to 30 minutes.
- **Instant Hardware RTC Button Interrupts:** Wakes from deep sleep in milliseconds upon pressing `OK`, `UP`, or `DOWN`.
- **Dual Onboarding / Provisioning:**
  - **Option A (Wi-Fi Hotspot):** Out of the box, broadcasts `ScreenTinker-Setup` for zero-tool smartphone configuration via captive portal (`192.168.4.1`). Layout mode can be selected during setup.
  - **Option B (WebSerial Installer):** Configure directly in Chrome/Edge via USB using the WebSerial Config Protocol (WSCP) v1.0.
- **Zero-Hardcoding 6-Digit Pairing:** Safe CSPRNG pairing flow with one-time claim token.
- **Bilingual Support:** Dynamic language switching (English default / German toggle) across all system screens and menus.

## Physical Device Test Record

The packaged build was thoroughly verified on physical production hardware:
- **Device:** Seeed Studio reTerminal Sticky (ESP32-S3R8, 8MB Octal Flash, 8MB Octal PSRAM, 3.97" 800x480 SSD1677 E-Paper).
- **Firmware Version:** 1.1.0
- **Test Results:**
  - Bootloader and partition flashing via WebSerial browser installer verified.
  - Initial boot into bilingual onboarding screen confirmed.
  - Smartphone Captive Portal configuration (`192.168.4.1`) tested and verified.
  - 6-digit server pairing handshake (`/api/embedded/pair/register` and `/status`) verified.
  - Dynamic PSRAM caching and button navigation (`UP`, `DOWN`, `OK`) verified.
  - Long press (1.5s on OK button) interactive System Menu with Wi-Fi Power and Status Screen verified.
  - Power Off action with QR Code screen and 0 µA battery latch release verified.
  - Wakeup from power-off using OK button verified.
  - ETag HTTP 304 cache validation, adaptive deep sleep, and RTC button interrupts verified.
  - Factory reset and NVS wiping from menu verified.

## Links & Support

- **Firmware Source & Releases:** [https://github.com/renebohne/screentinker_mcu](https://github.com/renebohne/screentinker_mcu)
- **Release v1.1.0:** [https://github.com/renebohne/screentinker_mcu/releases/tag/v1.1.0](https://github.com/renebohne/screentinker_mcu/releases/tag/v1.1.0)
- **Server Source:** [https://github.com/screentinker/screentinker](https://github.com/screentinker/screentinker)
- **Issue Tracker:** [https://github.com/renebohne/screentinker_mcu/issues](https://github.com/renebohne/screentinker_mcu/issues)
