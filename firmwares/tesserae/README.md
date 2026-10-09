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

## New since 1.35.0

- Touch wake has a gesture option (1.44.0). With it, the touch controller
  sits in its low-power gesture mode during deep sleep instead of scanning,
  and a double tap or a swipe wakes the Sticky. Tap wake stays the default.
- Setup over USB (1.43.0). The browser flasher at tesserae.ink/flash can set
  Wi-Fi and the server address right after flashing, with no hotspot step.
  The setup hotspot is still there and gains a Cloud tab beside Local and
  Relay.
- Lower sleep current (1.40.0, 1.42.0). The microSD slot power stays off
  through deep sleep, with the pin pull-ups switched off before each hold.
- The server can ask for the device log and is told about failed paints,
  brownouts and crashes on the next wake (1.41.0). A failed paint is retried
  on the next wake instead of being skipped.
- Touch buttons bound to a Home Assistant entity show its on/off state, and
  switches confirm their new state within a second or two of a tap (1.38.0,
  1.39.0).
- The logo splash after a cold boot is always replaced by the dashboard on
  the next poll (1.37.0).

## Package origin

Version 1.45.0 was built by the tesserae-device-firmware GitHub Actions
release pipeline from tag
[`v1.45.0`](https://github.com/dmellok/tesserae-device-firmware/releases/tag/v1.45.0)
(commit `fab26cb70188cf633492bda43eac3850220d8770`), PlatformIO environment
`seeed-reterminal-sticky`, ESP-IDF via PlatformIO `espressif32`. The four
files under `firmware/1.45.0/` are byte-identical to the images the pipeline
publishes for the tesserae.ink browser flasher
(`https://tesserae.ink/firmware/seeed-reterminal-sticky/v1.45.0/`), and the
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

Per-device options such as touch input, touch wake (tap or gesture), touch linger time and wake interval
live on the device card in the server UI.

## Physical-device test record

TODO: flash `firmware/1.45.0/` to the Sticky and record the result.

Project source, documentation and support are provided by the
[tesserae-device-firmware repository](https://github.com/dmellok/tesserae-device-firmware),
the [Tesserae docs](https://docs.tesserae.ink/) and the
[Tesserae Discord](https://tesserae.ink).
