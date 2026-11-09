# CrossNext for Sticky

CrossNext adapted for Seeed reTerminal Sticky, with English, Spanish and
Vietnamese menus. This is a firmware-only community contribution using the
public v1.4.8 release. The source remains in the maintained upstream repository.

## Features

- Read EPUB, TXT, Markdown and XTC/XTCH books with bookmarks and saved passages.
- Review flashcards and vocabulary, use productivity tools, and play games.
- Display saved book highlights on the sleep screen. The optional rotation
  interval is 1–240 minutes; 0 disables scheduled changes.
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

- Release: [v1.4.8](https://github.com/loitq07/CrossNext-for-Sticky/releases/tag/v1.4.8).
- Source reference: `182239b3042216f435a03583bf51e25a1d7d1f1e`.
- Build environment: PlatformIO `gh_release_sticky`.
- Binary: `CrossNext-Sticky-v1.4.8-factory.bin`, downloaded from the public
  GitHub Release, rather than copied from a local build with different bytes.
- Size: **7,839,168 bytes**.
- SHA-256: `93cf26bf9ea15ab3aa2d3e5618b34fc0c4afa1d671281de1b9b777c04543ae94`.
- Flash offset: **0x0**; chip family: **ESP32-S3**.
- The downloaded file's SHA-256 matches the GitHub Release asset digest.

The source license is MIT. The included LICENSE preserves the upstream notice.
Credit goes to [manuelx900/CrossNext](https://github.com/manuelx900/CrossNext),
[CrossPlay](https://github.com/ma-r-s/crossplay),
[CrossPoint Reader](https://github.com/crosspoint-reader/crosspoint-reader),
and Seeed Studio's hardware and FreeInk SDK. Loi Tran maintains the Sticky fork.

## Physical device test

On 2026-11-09, the fork maintainer supplied `assets/preview.jpg`, a real photo of
CrossNext running on reTerminal Sticky. The maintainer confirmed the device is
running v1.4.8 and that reading, touch/buttons, reboot with the reading position
preserved, and USB reconnection work. The photograph shows the reading shelf.

- Hardware: reTerminal Sticky; PCB revision not recorded.
- Tested firmware version: v1.4.8.
- Installation method used for the photographed unit: not recorded.
- [x] Main reading workflow.
- [x] Touch and physical buttons.
- [x] Reboot with reading position preserved.
- [x] USB reconnection.
- [ ] A repeated clean installation of the exact submitted factory image.
- [ ] Scheduled highlight wake, refresh quality and sleep-current measurement.

The public Release binary's hash is verified above. The bytes installed on the
photographed device were not independently read back, so the maintainer's
version-level test report does not establish byte-for-byte identity. A repeated
clean installation of the submitted package remains a review follow-up. The
entry is marked experimental while these checks remain open.

## Support

Report issues at <https://github.com/loitq07/CrossNext-for-Sticky/issues>.
