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
- 4-level grayscale rendering on both Sticky display controllers, SSD1677
  and SSD2677. The firmware detects the controller at boot, so one package
  runs on either. SSD1677 units also get partial refresh for small updates
  in under a second; on SSD2677 units every update is a full refresh of
  about 1.3 seconds.
- Touch: wake on tap, taps and swipes dispatched against the page's touch
  regions, and touch controls drawn on the panel.
- Battery reporting from the onboard BQ27220 fuel gauge.
- Signed over-the-air updates delivered by the Tesserae server, so browser
  flashing is only needed once.
- Lineups and schedules that rotate pages through the day, with quiet hours.

A Tesserae server on the local network is required. Install it with Docker or
the Home Assistant App: <https://docs.tesserae.ink/install/server/>.

## New since 1.62.3

- Support for the SSD2677 display controller (1.66.0). The firmware checks
  which controller is fitted at boot and drives either, and remembers the
  choice across deep sleep. Before this an SSD2677 unit never painted. On
  the SSD2677, partial refresh and the local tap echo are off, because its
  partial waveform is meant for black-and-white content and shifts the greys
  on the rest of the panel; a tap is answered by the next frame from the
  server. SSD1677 units behave as before.
- A tap that wakes the Sticky is reported as soon as Wi-Fi is up, before the
  frame request (1.66.0). It used to go out after the wake's frame had been
  painted, so that frame could not show the tap's result.
- After a power cycle or a RESET press the panel no longer stays on the
  Tesserae logo until the dashboard changes (1.63.0).
- The microSD card mounts again on a boot that draws the logo (1.63.0), so
  the first wake after a power cycle has its frame cache.
- HTTPS certificates are checked for their validity dates as well as their
  chain and hostname (1.63.0).
- The setup portal uses Tesserae's Paper theme and current logo (1.63.0),
  and its text names the Sticky's power button instead of a Refresh button
  the Sticky does not have (1.63.1).
- Over-the-air updates wait for 20 percent charge, read through the board's
  charge table, instead of a fixed voltage (1.64.0), and the Sticky sends
  the server a log of each update.

The other changes in this range are for boards that do not share the Sticky
build (XIAO 7.5" ePaper panel, M5Stack PaperS3).

## Package origin

Version 1.66.0 was built by the tesserae-device-firmware GitHub Actions
release pipeline from tag
[`v1.66.0`](https://github.com/dmellok/tesserae-device-firmware/releases/tag/v1.66.0)
(commit `7772f6bfe0703b3c2c20cfd5c1b4716f88aca855`), PlatformIO environment
`seeed-reterminal-sticky`, ESP-IDF via PlatformIO `espressif32`. The four
files under `firmware/1.66.0/` are byte-identical to the images the pipeline
publishes for the tesserae.ink browser flasher
(`https://tesserae.ink/firmware/seeed-reterminal-sticky/v1.66.0/`), and the
SHA-256 values in `manifest.json` match that catalog. `partitions.bin` and
`ota_data_initial.bin` are byte-identical to the 1.62.3, 1.45.0 and 1.35.0
packages; `bootloader.bin` is a new image. The firmware is distributed under
the upstream AGPL-3.0 licence.

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

SSD1677 unit (Seeed reTerminal Sticky, production unit), 2026-11-13:

- The four files of this package were written over USB with esptool at
  `0x0`, `0x8000`, `0x10000` and `0x20000`, without erasing.
- First boot reported firmware 1.66.0 and detected the SSD1677. The stored
  Wi-Fi and server settings survived, the dashboard painted (refresh
  1230 ms), and the unit slept and woke on its 60 second schedule from
  `ota_0`.
- Touch wake, the quick local tap echo (a windowed partial of 619 ms) and
  the repaint after a tap action were checked on this unit the same day with
  a local build of the same source. They were not repeated on the package
  itself, and the hardware button was not exercised.

SSD2677 unit (a community tester's Sticky), 2026-11-11 to 2026-11-12:

- Tested with local builds of the same source, not with this package:
  controller detection, full 4-level grayscale paints (1.3 s), setup and
  registration, deep sleep and wake, and taps answered by the server's next
  frame. The record is in
  [tesserae-device-firmware issue 71](https://github.com/dmellok/tesserae-device-firmware/issues/71).

Project source, documentation and support are provided by the
[tesserae-device-firmware repository](https://github.com/dmellok/tesserae-device-firmware),
the [Tesserae docs](https://docs.tesserae.ink/) and the
[Tesserae Discord](https://tesserae.ink).
