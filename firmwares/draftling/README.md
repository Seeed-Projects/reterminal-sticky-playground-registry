# Draftling

This community entry publishes the Draftling 1.0.29 build for reTerminal
Sticky for installation from the browser, in two variants:

| Variant | Keyboard input |
|---|---|
| Default | US English, UK, Ukrainian, Hebrew, German, French, Swiss German, Swiss French, Italian, Portuguese, Spanish, Swedish / Finnish, Norwegian, Danish, Polish, Czech, Slovak, Hungarian, Romanian, Croatian / Slovenian, Turkish Q, Canadian French, and Crimean Tatar (two variants) |
| Chinese | US English and Simplified Chinese pinyin |

Draftling is a writerdeck firmware. Its minimalist interface lets you edit
Markdown files with no distractions, using any Bluetooth Low Energy keyboard.
Project homepage: [draftling.art](https://draftling.art/).

## Package origin

The binaries come unchanged from the upstream web flasher
([clackups/draftling-flasher](https://github.com/clackups/draftling-flasher),
`main` branch, commit `a39d7e26a1230736371e4ec555f7b4c426715a68`,
`firmware/v1.0.29/draftling-seeed_reterminal_sticky*.bin`). They were built
from [clackups/draftling](https://github.com/clackups/draftling) release tag
`v1.0.29` (commit `91447c7feef30f1dc546a9ea91b5be89ea2e914b`) and are distributed under the upstream MIT license.

Each variant lives in its own directory under `firmware/1.0.29/`:

| Part | Offset | Upstream file (`default/`) | Upstream file (`chinese/`) |
|---|---|---|---|
| `bootloader.bin` | `0x0` | `draftling-seeed_reterminal_sticky-bootloader.bin` | `draftling-seeed_reterminal_sticky-chinese-bootloader.bin` |
| `partition-table.bin` | `0x8000` | `draftling-seeed_reterminal_sticky-partition-table.bin` | `draftling-seeed_reterminal_sticky-chinese-partition-table.bin` |
| `otadata.bin` | `0xd000` | `draftling-seeed_reterminal_sticky-otadata.bin` | `draftling-seeed_reterminal_sticky-chinese-otadata.bin` |
| `draftling.bin` | `0x10000` | `draftling-seeed_reterminal_sticky.bin` | `draftling-seeed_reterminal_sticky-chinese.bin` |

The partition table uses the standard ESP-IDF dual-OTA layout. Draftling runs
from `ota_0`, so the device can switch to other Sticky firmware that expects an
`otadata` partition, and back again, without rewriting the partition table.

## Features

- WYSIWYG Markdown editing: headings, lists, blockquotes, code fences, and
  inline styles appear formatted, and the markup shows only on the cursor line.
- Fountain screenplay mode for `.fountain` files and unformatted plain text
  for `.txt` files.
- A file browser to open, create, and rename files on the microSD card.
- Split-screen editing of two files, or two views of the same file.
- Portrait or landscape orientation.
- Built-in Git sync over Wi-Fi (commit, fetch, rebase or merge, push) with
  GitHub or any smart-HTTP Git server.
- Cursor and scroll position saved for each file.
- Chinese variant: type pinyin and pick hanzi with Space or 1-9 from the
  status bar. See the upstream
  [language pack guide](https://github.com/clackups/draftling/blob/main/docs/language-packs.md).

## Controls on reTerminal Sticky

| Input | Action |
|---|---|
| Bluetooth keyboard | Text entry and all shortcuts; F1 opens the main menu, F10 shows help |
| Up / Down buttons | Page Up / Page Down |
| Power button, short press | Deep sleep (asks first about unsaved changes) |
| Power button, 2 s hold | Forget every stored BLE keyboard |
| Touchscreen | GT911 touch input |

The full keyboard shortcut list is in the
[upstream README](https://github.com/clackups/draftling#keyboard-shortcuts).

## Hardware notes

- Units ship with an SSD1677 or SSD2677 panel controller. The firmware detects
  which one at boot.
- The USB-C port connects through a CH343 USB-to-UART bridge, so the
  "SD card via USB" feature is not available on this board.
- A microSD card is required for documents and for the `wifi.cfg` and
  `git.cfg` settings files.

More hardware details are in the upstream
[HARDWARE.md](https://github.com/clackups/draftling/blob/main/HARDWARE.md#seeed-reterminal-sticky).

## Physical device test

- Hardware: reTerminal Sticky production unit
- Firmware version: 1.0.29, Default and Chinese variants
- Package: the four parts listed above for each variant, at the offsets in
  its `manifest.json`
- Result: TODO: fill in after testing both variants on the device.
