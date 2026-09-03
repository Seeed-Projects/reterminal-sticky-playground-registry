# Sticky Arcade

Sticky Arcade 2.0.0 is a native offline activity launcher for reTerminal
E1005 ("reTerminal Sticky"). It includes 17 touch games plus a read-only
microSD browser and EPUB reader, for 18 activities in total.

The games include Falling Blocks, Connect Four, Klondike, Mahjong Solitaire,
2048, Mini Minesweeper, Sudoku, Reversi, Word Search, Crossword, Sokoban,
Dots and Boxes, Peg Solitaire, Lights Out, Nonogram, Pipe Connect, and
Slitherlink. Current games resume across selector visits and deep sleep.

## Setup and controls

On first boot, choose English, Spanish, French, German, or Simplified Chinese.
The language screen can be reopened with the selector's settings cog. Interface
fonts are included in firmware, and the games work without Wi-Fi or a microSD
card.

- Tap a tile to open an activity. Touch the board, swipe, or use each game's
  on-screen actions as shown in its help pane.
- On the selector, **UP** and **DOWN** move between pages. In most games,
  **UP** saves and returns to the selector.
- Release **OK** in under two seconds for Back. Hold it for at least two
  seconds and release to save and enter deep sleep; press **OK** to resume.
- Representative controls include swiping the 2048 board, holding a covered
  Minesweeper tile to flag it, and tapping a Connect Four column to play.

## EPUB reader

Insert a microSD card and open EPUB Reader to browse folders and supported
DRM-free, reflowable EPUB 2/3 books. The browser is read-only: it does not
write, rename, delete, or format files. Side **UP** and **DOWN** buttons change
browser or reading pages. The current folder, book, chapter, and page are kept
across deep sleep while the same card and book remain available.

Embedded JPEG and PNG covers are supported. Optional CJK font files from the
upstream release can be extracted to the SD-card root for Chinese, Japanese,
and Korean book text. DRM and fixed-layout books are not supported.

## Firmware package

- Registry firmware version: `2.0.0`
- Release: <https://github.com/danpodeanu/seeed-reterminal-E100X/releases/tag/v2.0.0>
- Release source commit: `f47874d0d6e9e62ce87b094e4aaba6faeba256f1`
- Upstream asset: `firmware-sticky-arcade-reterminal_e1005-full.bin`
- Target: ESP32-S3 with 32 MB flash
- Package: one merged bootloader, partition table, OTA selector, and
  application image written at offset `0x0`
- Size: `2,994,592` bytes
- SHA-256: `05ac03faa733061e58dc67428f7c2bbe63d5798f3ec45718f5aaf1a5e9be2ed9`
- MD5: `7eb7a24321528d4c54cb325ee01ad428`

The committed binary is byte-for-byte identical to the full E1005 image from
the upstream v2.0.0 GitHub Release. The release workflow produced it with
ESP32-S3 DIO flash mode, an 80 MHz flash frequency, and a 32 MB flash size.
The submitted preview is the project-owned E1005 selector capture from the
same tagged source.

## Installation

Open Sticky Arcade in reTerminal Sticky Playground, connect the device with a
USB data cable, select version 2.0.0, and start the browser installation. Erase
the device when moving from another firmware so old settings do not affect the
first-boot language flow. The manifest writes the complete merged image from
flash offset `0x0`.

The equivalent direct-flash layout is:

```bash
esptool.py --chip esp32s3 --baud 460800 write_flash \
  --flash_mode dio --flash_freq 80m --flash_size 32MB \
  0x0 firmware-sticky-arcade-reterminal_e1005-full.bin
```

The full image is for USB or browser installation. Do not copy it to an SD
card as `/update.bin`; the upstream `-ota.bin` release asset is the separate
package intended for SD firmware updates.

## Physical-device test record

This exact v2.0.0 release was built, flashed as the merged full image, and used
successfully on the connected reTerminal E1005 in the originating project
session. The device booted Sticky Arcade and its launcher and controls were
used successfully. This record does not claim separate repeated-install,
power-loss, or every-game qualification beyond that performed use.

| Item | Result |
|---|---|
| Hardware | Connected reTerminal E1005 / reTerminal Sticky |
| Firmware | Upstream v2.0.0 full merged release image |
| Flash layout | 32 MB image at `0x0` |
| Build, flash, boot, and interactive use | Passed |

Source and full operating documentation are in the
[upstream repository](https://github.com/danpodeanu/seeed-reterminal-E100X/tree/v2.0.0/sticky-arcade).
Support is provided through the project's
[issue tracker](https://github.com/danpodeanu/seeed-reterminal-E100X/issues).
Sticky Arcade is licensed under
[GPL-2.0](https://github.com/danpodeanu/seeed-reterminal-E100X/blob/v2.0.0/LICENSE).
