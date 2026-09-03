# Sticky MicroNotes

Sticky MicroNotes is a community writing firmware for Seeed Studio reTerminal
Sticky. It is a port/sticky focused update of MicroSlate to the Sticky's ESP32-S3, e-ink panel, 
GT911 touch, and power rails.

Pair a Bluetooth LE keyboard, write notes, and store notes as
plain `.txt` files on a microSD card. Move files to a phone or laptop over Wi-Fi
at http://sticky.local

## Features

- **Bluetooth keyboard** — BLE HID host. Stores up to 4 keyboards and
  auto-reconnects. Classic Bluetooth keyboards will not connect (ESP32-S3 BLE
  only).
- **Note management** — browse, create, rename, and delete notes on the SD card.
- **Named notes** — each note has a title. New notes default to MicroNote,
  then MicroNote_2, and so on.
- **Text editor** — cursor navigation, word-wrap, fast e-paper refresh.
- **Writing modes**
  - *Scroll* — standard scrolling editor (default)
  - *Typewriter* — current line only, centered
  - *Pagination* — page flips instead of per-line scroll
- **Auto-save** — after 10 seconds idle or every 2 minutes while typing. Back,
  Esc, power, and sleep also save.
- **Safe writes** — write-verify plus `.bak` rotation. A failed write does not
  replace the last good file.
- **Clean mode** — hide editor chrome (Ctrl+Z).
- **Dark mode** — inverted panel.
- **Orientation** — portrait, landscape, and inverted variants. Optional gyro
  auto-rotate in Settings.
- **Transfer** — join a 2.4 GHz network, then open http://sticky.local/ to
  upload, open, download, or delete `.txt` notes.
- **Sleep** — hold power. From the editor, the current note can stay on the
  panel without editor chrome.
- **Settings backup** — BLE pairings, Wi-Fi credentials, and UI prefs are stored
  under `/sticky/` on the SD card.


## Controls

Sticky has three physical keys plus a bottom touch bar and a BLE
keyboard. The touch bar is Select / Back / Up / Down on every menu.

| Control | Action |
| --- | --- |
| Touch bar Select, or short power press | Confirm |
| Touch bar Back | Back / cancel |
| Touch bar Up / Down | Move |
| Hold power (~3 s) | Sleep (does not confirm the highlighted item) |
| BLE keyboard | Type and use the shortcuts below |

### Main menu

Browse Notes, New Note, Settings, Transfer.

| Key | Action |
| --- | --- |
| Up / Down | Move |
| Enter / Select | Open |
| Esc / Back | Stay / no-op |

### Editor

| Key | Action |
| --- | --- |
| Arrow keys | Move cursor |
| Home / End | Start / end of line |
| Backspace / Delete | Edit |
| Tab | Cycle Scroll → Typewriter → Pagination |
| Ctrl+S | Save now |
| Ctrl+N | Edit title |
| Ctrl+Z | Clean mode |
| Ctrl+T | Typewriter |
| Ctrl+P | Pagination |
| Ctrl+Left / Right | Previous / next page (Pagination) |
| Esc / Back | Save and leave |

Header badges: **[S]** Scroll, **[T]** Typewriter, **[P]** Pagination.

### Settings

| Item | What it does |
| --- | --- |
| Dark mode | Invert black / white |
| Orientation | Portrait, landscape, and inverted |
| Gyro | Auto-rotate from the IMU |
| Font size | Editor type size |
| Word count | Show or hide in the editor header |
| Bluetooth | Scan, pair and manage saved keyboards |

| Key | Action |
| --- | --- |
| Up / Down | Move |
| Enter / Select | Toggle or open |
| Esc / Back | Main menu |

### Bluetooth

| Key | Action |
| --- | --- |
| Up / Down | Move |
| Enter / Select | Connect |
| Esc / Back | Cancel / settings |

Put the keyboard in pairing mode before you scan. Saved keyboards reconnect on
boot. Status in the header updates when the link comes up (`KB connected`).

### Transfer

Pick a 2.4 GHz network (saved networks still use the stored password when you
select them; the device does not auto-join). After connect, the panel shows
http://sticky.local/ and the IP.

On a phone or computer on the same LAN, open that URL to list, open, download,
upload, or delete notes. Back turns Wi-Fi off.

| Key | Action |
| --- | --- |
| Up / Down | Network list |
| Enter / Select | Join / continue |
| Esc / Back | Cancel and stop Wi-Fi |

## Hardware requirements

- Seeed Studio reTerminal Sticky (ESP32-S3R8, 800×480 SSD1677, GT911)
- microSD card formatted FAT32
- USB data cable (WCH UART bridge, not native USB-CDC)
- Bluetooth LE HID keyboard

## First boot

1. Insert the SD card.
2. Flash from Playground or from the factory image in `firmware/1.0.0/`.
3. Open **Settings → Bluetooth**, scan, and pair a keyboard.
4. **New Note** to write, or **Transfer** to pull files from a phone.

## File format

Notes are `.txt` files in `/notes/`. Titles become lowercase filenames with
spaces as underscores (`My Note` → `my_note.txt`). Drop extra `.txt` files into
`/notes/` from a computer if you want; the on-device title is derived from the
name.

Settings live in `/sticky/` (`ui_prefs.json`, `wifi.json`, `ble_kb.json`).

## Firmware package

- Registry firmware version: `1.0.0`
- License: MIT (Copyright 2026 Joshua Hinton; Sticky port: LowFlowIO)
- Build system: PlatformIO (Arduino + ESP-IDF). Playground CI does not build
  PlatformIO, so this entry is firmware-only.
- Flash: merged image at offset `0`, DIO, 40 MHz, 32 MB declared size.
- Do not flash QIO / 80 MHz. That bootlooped this board.

## Build and flash locally

```bash
pio run -e reterminal_sticky
./scripts/flash_sticky.sh PORT app      # already running this firmware
./scripts/flash_sticky.sh PORT full     # first install
```

Factory image for Playground:

```bash
python3 -m esptool --chip esp32s3 merge_bin \
  -o sticky-micronotes-1.0.0-reterminal-sticky.bin \
  --flash_mode dio --flash_freq 40m --flash_size 32MB \
  0x0     .pio/build/reterminal_sticky/bootloader.bin \
  0x8000  .pio/build/reterminal_sticky/partitions.bin \
  0x10000 .pio/build/reterminal_sticky/firmware.bin
```

## Hardware test record

| Item | Result |
| --- | --- |
| Hardware | reTerminal Sticky production hardware |
| Firmware version | `1.0.0` |
| Power-on and first boot | Passed |
| Touch bar menus | Passed |
| BLE keyboard pair / reconnect | Passed |
| Editor, auto-save, sleep-with-note | Passed |
| Transfer at http://sticky.local/ | Passed |
| Gyro auto-rotate | Passed |
| Note: DIO, 40 MHz. Do not flash QIO 80 MHz. |
