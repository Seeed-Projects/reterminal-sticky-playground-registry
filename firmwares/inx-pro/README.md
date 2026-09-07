# Inx Pro

Inx Pro is community firmware for Seeed Studio reTerminal Sticky, focused on
reading EPUB, TXT, and XTC books on the E-Ink display. It includes a local
library, image rendering, reader customization, reading statistics, Wi-Fi,
OPDS, Calibre, and KOReader sync support.

## Firmware package

- Registry version: `1.0.0`
- Source: <https://github.com/obijuankenobiii/inx-pro>
- Release: <https://github.com/obijuankenobiii/inx-pro/releases/tag/1.0.0>
- License: MIT
- Build system: PlatformIO / Arduino-ESP32
- Target: ESP32-S3
- Package: complete merged Sticky factory image at flash offset `0x0`
- Flash settings: `32MB`, `dio`, `80m`

The package was copied from the `sticky.bin` asset in the Inx Pro `1.0.0`
release and is intended for browser-based installation through the reTerminal
Sticky Playground. It is a complete merged image for browser flashing.

## Hardware requirements

- Seeed Studio reTerminal Sticky
- ESP32-S3 with PSRAM
- 3.97-inch 800 × 480 E-Ink display
- USB data cable for installation
- Compatible microSD card for books and local data

## Installation

Connect the Sticky with a USB data cable, open the Playground in desktop Chrome
or Edge, select **Inx Pro**, and follow the browser flasher prompts. Enable the
erase option when replacing another firmware. After installation, insert the
microSD card and reboot the device.

## Verification

The registry package is a locally verified 6,008,064-byte binary with matching
SHA-256 `ffbec33a47cc1fe046d02eccc1109d2f923e63b0493446bf8be063d76d2f00ea`
and MD5 `e2297cc679d2d1d7b9115cad66a0ccd6` manifest checksums. Test this exact
package on physical reTerminal Sticky hardware before merging the registry
contribution.

## Scope

Inx Pro is an independent community project and is not affiliated with Seeed
Studio. See the project repository for source code, release notes, and support.
