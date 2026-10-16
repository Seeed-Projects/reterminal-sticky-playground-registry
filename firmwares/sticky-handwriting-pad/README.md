# Sticky Handwriting Pad

An offline community handwriting notebook by [limengdu](https://github.com/limengdu) for Seeed Studio reTerminal Sticky.

- [Project and build instructions](https://github.com/limengdu/reterminal_sticky_handwriting_pad)
- [Hardware-tested 0.1.0 firmware](https://github.com/limengdu/reterminal_sticky_handwriting_pad/releases/tag/v0.1.0)
- [Support](https://github.com/limengdu/reterminal_sticky_handwriting_pad/issues)
- [MIT license](https://github.com/limengdu/reterminal_sticky_handwriting_pad/blob/d30955bd6a497dd6d14d660e1d634f3ef97c5112/LICENSE)

![Sticky Handwriting Pad running on reTerminal Sticky](assets/preview.jpg)

## Features

- Finger-operated writing and sketching, with up to 20 notebook pages.
- Round, calligraphy, pencil, and speed-sensitive brush styles.
- Four pen sizes; black, dark gray, light gray, and white eraser.
- Per-page undo and redo, up to 50 steps per page within the shared memory budget.
- Floating touch toolbar and physical buttons for page navigation.
- Notebook pages, pen settings, and undo history saved before sleep and restored on wake or a saved power cycle.
- Automatic save and deep sleep after one minute of inactivity.
- Local operation with no network account or external storage required.

## Controls

Use portrait orientation with the hardware buttons on the right.

| Control | Action |
| --- | --- |
| Touch the page | Write or erase |
| Page Up | Previous page |
| Page Down | Next page; create a new one at the end, up to 20 |
| AI button, short press | Save and sleep; press again to wake |
| AI button, hold for one second | Open the clear-page confirmation |
| Undo / redo in the top bar | Undo or redo an edit on this page |
| Tap the bottom handle or swipe up from the bottom | Open the pen toolbar |
| Swipe down, tap the toolbar grip, or tap outside | Close the toolbar |

Start writing above the bottom 48-pixel gesture area. Before disconnecting power, press the AI button and let the device finish saving and enter sleep, indicated by the moon icon.

## Firmware package

- Version: `0.1.0`, experimental.
- Source: [https://github.com/limengdu/reterminal_sticky_handwriting_pad](https://github.com/limengdu/reterminal_sticky_handwriting_pad), commit `d30955bd6a497dd6d14d660e1d634f3ef97c5112`.
- License: MIT, copyright 2026 limengdu.
- Artifact origin: original PlatformIO `sticky-release` binaries from the upstream project's hardware-tested local build, also published in its `v0.1.0` release.
- Build configuration: `espressif32@6.11.0`, ESP-IDF 5.4.1, ESP32-S3R8, 32 MB flash.
- Flash settings: DIO, 80 MHz, 460800 baud.
- Delivery: a firmware-only package with its maintained upstream source linked above.

The manifest records the flash layout, byte size, and SHA-256 of every file:

| File | Offset | Bytes | SHA-256 |
| --- | --- | --- | --- |
| `bootloader.bin` | `0x0` | 22464 | `b5176b7b1a0ace49e2dde1f21f7e51565cca2a650b21a7cbf2e223b02bef1e18` |
| `partitions.bin` | `0x8000` | 3072 | `87c890a6a25424bf3c16606f8dfc63edfa88a535f562cc4a87b0768e704244c9` |
| `firmware.bin` | `0x10000` | 378320 | `20e0eca585463b8086d4efade67c3b7ef2cbe3c003e9ee525b5db320b1515f58` |

The notebook uses the `notes` partition (2 MB) for pages and settings and the `history` partition (4 MB) for undo/redo records.

## Installation

Open the Sticky Playground firmware page, connect the device with a USB data cable, and install version `0.1.0` using desktop Chrome or Edge. The supplied manifest includes all three files needed for a fresh installation.

For a local source build and upload:

```sh
pio run -e sticky-release
pio run -e sticky-release -t upload
pio device monitor -b 115200
```

Run these commands from the upstream project root.

## Hardware test record

| Item | Record |
| --- | --- |
| Device | Physical reTerminal Sticky |
| Firmware | Original `sticky-release` application, version `0.1.0` |
| Test date | 2026-10-16 |
| Device testing | The project maintainer confirmed this build was tested on-device |
| Preview | Maintainer-provided photo of handwriting and the pen toolbar on the device |
| Source build | A clean copy of the published source compiled successfully with the pinned PlatformIO platform |
| Package integrity | Packaged binaries retain the original build's SHA-256 values |

## Device verification steps

1. Install the package and check the notebook opens.
2. Write continuous strokes; try every pen style, size, and color.
3. Exercise undo, redo, clear confirmation, and clear cancellation.
4. Create and switch pages using the side buttons.
5. Save with the AI button, wake, and check pages, pen settings, and undo history. Repeat with a power cycle after saving.
6. Leave the notebook idle for one minute and check automatic sleep.
