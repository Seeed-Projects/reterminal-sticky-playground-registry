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

## New in 1.35.0

- The CPU light-sleeps while the panel refreshes, cutting the current drawn
  during each paint.
- The battery power latch is driven at boot, so a Sticky started from its
  power button stays on after the button is released.
- A microSD card that fails to mount no longer disturbs the display bus, and
  mounting retries at lower SPI clocks for slow cards.
- Crash dumps are kept in a new 64 KB flash partition (written at USB
  install; over-the-air updates do not change the partition table).

## Package origin

Version 1.35.0 was built by the tesserae-device-firmware GitHub Actions
release pipeline from tag
[`v1.35.0`](https://github.com/dmellok/tesserae-device-firmware/releases/tag/v1.35.0)
(commit `edf493f2a14bc1c992955caff8b83a5310d10b54`), PlatformIO environment
`seeed-reterminal-sticky`, ESP-IDF via PlatformIO `espressif32`. The four
files under `firmware/1.35.0/` are byte-identical to the images the pipeline
publishes for the tesserae.ink browser flasher
(`https://tesserae.ink/firmware/seeed-reterminal-sticky/v1.35.0/`), and the
SHA-256 values in `manifest.json` match that catalog. The firmware is
distributed under the upstream AGPL-3.0 licence.

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

Per-device options such as touch input, touch linger time and wake interval
live on the device card in the server UI.

## Physical-device test record

- Device: Seeed reTerminal Sticky (production unit), tested 2026-09-12
- Package: the four files in `firmware/1.35.0/`, written byte-identical with
  esptool 5.1.2 over USB at `0x0`, `0x8000`, `0x10000` and `0x20000`
  (`--flash-mode keep --flash-freq keep --flash-size keep`, no erase); every
  part hash-verified by esptool
- Result: booted `v1.35.0` from `ota_0`, found the new `coredump` partition,
  reconnected to Wi-Fi with the credentials kept in NVS, reported battery
  (BQ27220) and environment readings to the Tesserae server, mounted the
  microSD card, painted the logo splash (full 4-gray refresh 1.23 s) and
  entered deep sleep on the server-driven schedule
- Touch: two taps, each waking the unit from deep sleep with the coordinate
  recovered by the wake stub and reported to the server; after the first the
  device downloaded and painted its 480x800 4-gray dashboard (paint 1.7 s),
  after the second it painted a re-rendered frame during the 30 s touch
  linger, then slept again
- The same 1.35.0 application image is what the upstream release pipeline
  serves over the air; this unit had run the pre-release build of the same
  tree on the bench the day before (light sleep during refresh, power latch
  on battery)

Project source, documentation and support are provided by the
[tesserae-device-firmware repository](https://github.com/dmellok/tesserae-device-firmware),
the [Tesserae docs](https://docs.tesserae.ink/) and the
[Tesserae Discord](https://tesserae.ink).
