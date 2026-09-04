# ScreenTinker

This community firmware contribution packages ScreenTinker 1.0.0 for browser-based installation on Seeed Studio reTerminal Sticky.

The package was built from the official [ScreenTinker MCU repository](https://github.com/renebohne/screentinker_mcu) release tag `v1.0.0` (commit `81f738b`) and is distributed under the upstream MIT license.

## Overview

ScreenTinker turns reTerminal Sticky into an ultra-low-power, wireless digital signage display and information dashboard. It communicates with any self-hosted or cloud ScreenTinker server instance.

Key features include:
- **Direct 1-Bit SSD1677 Bitstream Streaming:** Zero MCU render overhead; the server provides pre-dithered, packed 1-bit monochome frames.
- **8 MB PSRAM Frame Caching:** Stores up to 32 frames in memory for instant (< 2s) button browsing (`UP`/`DOWN`).
- **Server-Coordinated Sleep & ETag Caching:** Automatically skips display refresh and enters deep sleep when content has not changed (`HTTP 304`).
- **Dual Onboarding / Provisioning:**
  - **Option A (Wi-Fi Hotspot):** Out of the box, broadcasts `ScreenTinker-Setup` for zero-tool smartphone configuration via captive portal (`192.168.4.1`).
  - **Option B (WebSerial Installer):** Configure directly in Chrome/Edge via USB.
- **Zero-Hardcoding 6-Digit Pairing:** Safe CSPRNG pairing flow with one-time claim token.
- **Bilingual Support:** Dynamic language switching (English / German) on the hardware buttons.

## Physical Device Test Record

The packaged build was thoroughly verified on physical production hardware:
- **Device:** Seeed Studio reTerminal Sticky (ESP32-S3R8, 8MB Octal Flash, 8MB Octal PSRAM, 3.97" 800x480 SSD1677 E-Paper).
- **Firmware Version:** 1.0.0
- **Test Results:**
  - Bootloader and partition flashing via WebSerial browser installer verified.
  - Initial boot into bilingual onboarding screen confirmed.
  - Smartphone Captive Portal configuration (`192.168.4.1`) tested and verified.
  - 6-digit server pairing handshake (`/api/embedded/pair/register` and `/status`) verified.
  - Dynamic PSRAM caching and button navigation (`UP`, `DOWN`, `OK`) verified.
  - ETag HTTP 304 cache validation and sleep cycles verified.
  - Long press (5s OK button) factory reset and NVS wiping verified.

## Links & Support

- **Firmware Source:** [https://github.com/renebohne/screentinker_mcu](https://github.com/renebohne/screentinker_mcu)
- **Server Source:** [https://github.com/screentinker/screentinker](https://github.com/screentinker/screentinker)
- **Issue Tracker:** [https://github.com/renebohne/screentinker_mcu/issues](https://github.com/renebohne/screentinker_mcu/issues)
