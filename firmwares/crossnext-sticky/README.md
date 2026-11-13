# CrossNext for Sticky

CrossNext adapted for Seeed reTerminal Sticky, with English, Spanish and
Vietnamese menus. This is a firmware-only community contribution using the
v1.4.11 release with automatic display controller detection for both screen revisions.
The source remains in the maintained upstream repository.

## Features

- Read EPUB, TXT, Markdown and XTC/XTCH books with bookmarks and saved passages.
- Review flashcards and vocabulary, use productivity tools, and play games.
- Community Plugin Hub integration and audio voice note features.
- Display saved book highlights on the sleep screen with customizable font and alignment.
  The optional rotation interval is 1–240 minutes; 0 disables scheduled changes.
- Automatic runtime detection and support for both screen controller revisions.
- Use basic reading offline. Wi-Fi and external accounts are needed only for
  online integrations that the user chooses to configure.

## Setup and controls

Use a reTerminal Sticky, a USB data cable and a FAT32 microSD card for books and
application data. Back up settings and card contents before changing firmware.
Install the merged image at **0x0** using the Registry manifest. It includes the
bootloader, partition table and application; do not write this factory image
at the application-only offset 0x10000.

If the device stays in USB charging standby after reset, hold the physical
Power button to open the interface. Use touch to select shelf items and the
physical page keys to navigate. Configure the language in Settings > System >
Language. See the [upstream installation guide](https://github.com/loitq07/CrossNext-for-Sticky#readme)
for device setup and recovery.

For the highlight screen, save passages while reading and choose Settings >
Display > Sleep screen > Book highlights. It reads `/My Clippings.txt` on the
card. Timed rotation is configurable; physical timer wake behavior, refresh
quality and sleep current still need verification.

## Package origin

- Release: [v1.4.11](https://github.com/loitq07/CrossNext-for-Sticky/releases/tag/v1.4.11).
- Source reference: `d5bb439c24bb403cb32025e5f864283a4e28007d`.
- Build environment: PlatformIO `gh_release_sticky`.
- Binary: `CrossNext-Sticky-v1.4.11-factory.bin`.
- Size: **8,301,568 bytes**.
- SHA-256: `8a34aa0c9b18affe13d5f4afe34fc1e987ea4148be6ac3a9a8cb007873abbcc3`.
- Flash offset: **0x0**; chip family: **ESP32-S3**.

The source license is MIT. The included LICENSE preserves the upstream notice.
Credit goes to [manuelx900/CrossNext](https://github.com/manuelx900/CrossNext),
[CrossPlay](https://github.com/ma-r-s/crossplay),
[CrossPoint Reader](https://github.com/crosspoint-reader/crosspoint-reader),
and Seeed Studio's hardware and FreeInk SDK. Loi Tran maintains the Sticky fork.

## Physical device test

The maintainer supplied `assets/preview.jpg`, an updated real photo of CrossNext running on
reTerminal Sticky showing the reading shelf with the Vietnamese title "DÁM BỊ GHÉT".
The maintainer confirmed the device is running the release and that reading, touch/buttons,
reboot with the reading position preserved, and USB reconnection work.

- Hardware: reTerminal Sticky (ESP32-S3).
- Tested firmware version: v1.4.11.
- [x] Main reading workflow.
- [x] Touch and physical buttons.
- [x] Reboot with reading position preserved.
- [x] USB reconnection.
- [x] Display initialization and detection on screen hardware.
- [ ] Repeated clean installation of the exact submitted factory image.
- [ ] Scheduled highlight wake, refresh quality and sleep-current measurement.

The factory release package hash is verified above. The entry is marked experimental
while extended acceptance checks remain open.

## Support

Report issues at <https://github.com/loitq07/CrossNext-for-Sticky/issues>.
