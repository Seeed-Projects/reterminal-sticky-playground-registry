# Tesserae

This partner entry provides the official Tesserae firmware package for
browser-based installation on reTerminal Sticky.

[Tesserae](https://tesserae.ink) is a self-hosted e-ink dashboard server. You
compose pages from widgets (weather, calendars, Home Assistant entities,
transit, photos and a widget catalog) in the browser, bind them to devices,
and the server renders each frame into the exact bytes the panel wants. The
reTerminal Sticky runs the shared
[tesserae-device-firmware](https://github.com/dmellok/tesserae-device-firmware)
client: a deep-sleep REST client that wakes on a schedule, downloads the next
frame over Wi-Fi as a 4-level grayscale buffer, paints it and sleeps again.

## What Sticky users get

- Dashboards composed on the server, no on-device configuration beyond Wi-Fi
  and the server URL.
- 4-level grayscale rendering on the SSD1677 panel, with partial refresh for
  small updates in under a second.
- Touch: wake on tap, taps and swipes dispatched against the page's touch
  regions, and touch controls drawn on the panel.
- Battery reporting from the onboard BQ27220 fuel gauge.
- Signed over-the-air updates delivered by the Tesserae server, so browser
  flashing is only needed once.
- Lineups and schedules that rotate pages through the day, with quiet hours.

A Tesserae server on the local network is required. Install it with Docker or
the Home Assistant App: <https://docs.tesserae.ink/install/server/>.

## New since 1.45.0

- Touch off now powers the digitiser down (1.57.0). With touch disabled the
  GT911 used to stay on its rail in its last mode through every sleep, at
  about 2.5 mA average; it is now switched off, so a Sticky with touch off
  sleeps as low as one that never had it.
- Wi-Fi names in Chinese and other non-Latin scripts work in the setup
  portal (1.62.2), and a network picked from the portal's list is joined by
  its exact broadcast bytes, so a router that broadcasts a GBK name can be
  joined.
- A set-up Sticky waiting out a Wi-Fi outage no longer reopens the setup
  hotspot on every retry (1.51.0). The portal opens once after about 12
  minutes of retries; if it expires unused, the panel paints a "Wi-Fi
  unavailable" screen once and retries the stored network from deep sleep
  every 12 minutes (30 minutes after about two hours).
- Bluetooth setup from the setup portal works on the Sticky (1.53.0). The
  Wi-Fi, Bluetooth and lwIP buffers moved to PSRAM, leaving about 90 KB of
  internal RAM free with the portal up, which also stops the portal splash
  failing to paint.
- Tapping a device-drawn control to wake the Sticky no longer erases the
  other controls (1.46.0). The wake fetches the full frame instead of
  repainting a blank buffer, and taps in the linger window animate again.
- microSD cards that refuse CMD59 or time out reading the SD status register
  now mount instead of being reported absent (1.51.0).
- Serial setup gained a `bootloader` command (1.59.0): the Sticky restarts
  into the ROM download mode so esptool can flash it without holding BOOT.
- The boot and setup splashes show the new Tesserae mark (1.55.0), and the
  cold-boot splash paints before the Wi-Fi driver starts (1.56.0).

The other releases in this range add boards that do not share the Sticky
build (Inkplate 13SPECTRA, CrowPanel 5.79", epdiy v7, Huessen 13.3", M5Stack
PaperDIY, LILYGO T5 S3 Pro, Waveshare RLCD 4.2) and fix the M5Paper's battery
behaviour.

## Package origin

Version 1.62.3 was built by the tesserae-device-firmware GitHub Actions
release pipeline from tag
[`v1.62.3`](https://github.com/dmellok/tesserae-device-firmware/releases/tag/v1.62.3)
(commit `8368b78d11410506415f17f31080e47e9913d75f`), PlatformIO environment
`seeed-reterminal-sticky`, ESP-IDF via PlatformIO `espressif32`. The four
files under `firmware/1.62.3/` are byte-identical to the images the pipeline
publishes for the tesserae.ink browser flasher
(`https://tesserae.ink/firmware/seeed-reterminal-sticky/v1.62.3/`), and the
SHA-256 values in `manifest.json` match that catalog. `partitions.bin` and
`ota_data_initial.bin` are byte-identical to the 1.45.0 and 1.35.0 packages;
`bootloader.bin` is a new image. The firmware is distributed under the
upstream AGPL-3.0 licence.

| File | Offset | Purpose |
|---|---|---|
| `bootloader.bin` | `0x0` | ESP32-S3 second-stage bootloader |
| `partitions.bin` | `0x8000` | A/B OTA partition table (`ota_0` at `0x20000`, `ota_1` at `0x420000`, a 64 KB `coredump` partition at `0x820000`) |
| `ota_data_initial.bin` | `0x10000` | OTA selector, points at `ota_0` |
| `firmware.bin` | `0x20000` | Tesserae application |

The NVS region at `0x9000` is not written, so Wi-Fi credentials survive a
non-erasing reinstall.

## Setup

1. Install the Tesserae server and open its web UI.
2. Flash this package to the Sticky from the Playground page.
3. On first boot the Sticky starts a Wi-Fi captive portal. Join it and enter
   your Wi-Fi network and the Tesserae server URL. The device registers itself
   and appears under **Settings → Devices** on the server.
4. Bind a page to the device and press **Send**. The Sticky paints it on its
   next wake.

Per-device options such as touch input, touch wake (tap or gesture), touch linger time and wake interval
live on the device card in the server UI.

## Physical-device test record

- Device: Seeed reTerminal Sticky (production unit)
- Status: pending. The 1.62.3 package has not yet been installed on this
  unit; this record will be updated with the result before review. The most
  recent build run on it is 1.53.0 (a release-environment build of the same
  tree, 2026-10-23: boot, Wi-Fi, the setup portal with Bluetooth setup,
  dashboard paints, deep sleep and wake).
- `partitions.bin` and `ota_data_initial.bin` are byte-identical to the
  1.35.0 package, whose full USB write at `0x0`, `0x8000`, `0x10000` and
  `0x20000` was verified on this unit (PR #66).

Project source, documentation and support are provided by the
[tesserae-device-firmware repository](https://github.com/dmellok/tesserae-device-firmware),
the [Tesserae docs](https://docs.tesserae.ink/) and the
[Tesserae Discord](https://tesserae.ink).
