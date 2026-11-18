# PocketPlay

## What it is

Games only firmware for the Seeed reTerminal Sticky

## Source

- Project: https://github.com/jsrijith/pocketplay
- License: MIT

## Package

- Version: 0.4.0
- `pocketplay-full.bin` at 0x0 (2830592 bytes)

Submitted by Srijith.

## Physical-device test record

Reported by the project author, Srijith, in [PR #137](https://github.com/Seeed-Projects/reterminal-sticky-playground-registry/pull/137#issuecomment-6952527861). These are the author's results, not an independent maintainer retest.

- Hardware: Seeed Studio reTerminal Sticky.
- Firmware: PocketPlay 0.4.0, `pocketplay-full.bin` installed at `0x0`.
- SHA-256: `aac0b1710ba118223709b29fb57605459a77dd7ddc1ae52177273598c3d13625`.
- Toolchain: ESP-IDF 5.5.5 (Arduino core 3.3.11, pioarduino platform 55.03.311).

| Check | Author-reported result |
| --- | --- |
| Power-on: boots to the splash screen, then the home page | PASS |
| Main workflow: open a game from the home page and play it | PASS |
| Touch and buttons: taps, Up/Down to page, OK to sleep and wake | PASS |
| Reboot and saved state: sleep, wake, the same game and position come back | PASS |
| USB reconnection and repeat install over the same image | PASS |

The preview is the author's [on-device photo](https://github.com/user-attachments/assets/572403e7-b4da-47b9-a513-772b42f36474), showing the 0.4.0 splash screen and games home page. The firmware package is unchanged; the newer 0.5.0 release is not included.
