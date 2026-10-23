# EDC Book

EDC Book turns reTerminal Sticky into an offline e-reader with configurable
typography and touch controls. It reads DRM-free TXT, EPUB, MOBI, AZW3, and FB2
books, supports Chinese vertical typesetting, and keeps bookmarks and reading
progress. Books are stored on a microSD card. Wi-Fi file management and
configuration are optional; reading does not require a cloud service.

- Author: [mengxiyou](https://github.com/mengxiyou)
- Project, documentation, and support: [edcbook.cn](https://edcbook.cn/)
- Package: firmware-only, version **3.1.1**
- License: **LicenseRef-EDCBook-Firmware**, described in [LICENSE.txt](LICENSE.txt)

## Source availability and distribution

The author does not publish the source code. The firmware binary is public and
may be used and redistributed freely. Third-party components remain covered by
their respective licenses.

The `source.url` metadata field points to the official project website as the
project provenance link. It is **not a public source repository**. This partner
entry distributes the author's firmware binary under the terms in LICENSE.txt;
it makes no claim of an open-source license.

## Official logo and device demonstration

The Partner Firmware card uses the official EDC Book logo.

![EDC Book official logo](assets/logo.png)

[Watch or download the real-device demonstration](assets/demo.mp4).
The approximately 149-second video is a compressed copy of the author's
recording, with its full duration retained. The logo and recording were
provided by the author for this submission.

## Installation and first use

1. Back up device settings before installing. This is a complete flash image;
   installation replaces the internal firmware, fonts, and filesystem, including
   stored settings. Books on the microSD card are separate from the flash image.
2. Connect reTerminal Sticky with a USB data cable. Use the Sticky Playground
   browser flasher in desktop Chrome or Edge when this entry is published.
   The device connection and bootloader instructions are available in the
   [Sticky hardware guide](https://www.seeedstudio.com/sticky/docs/zh/device-guide/hardware-overview/).
3. Write `firmware/3.1.1/edcbook_full.bin` at **0x0**. It is a merged image, so
   do not write it at the application-only offset `0x10000`.
4. Put DRM-free books on a microSD card with a card reader, insert the card,
   and restart the reader. Select a book in the file browser and use touch
   controls to read, turn pages, and adjust typography. Hardware-button actions
   can be configured in the reader settings.
5. Use the built-in guide or the [project guide](https://edcbook.cn/#guide)
   for reading controls and optional Wi-Fi file management.

On Sticky, USB is used for serial communication and firmware installation;
this firmware does not provide USB mass-storage file transfer. The package
uses the first 16 MiB of flash and includes its fonts and internal filesystem.
DRM-protected books are not supported.

## Package provenance and flash layout

This is the author's release artifact `edcbook_20261017.bin`, copied byte for
byte to `edcbook_full.bin` for this Registry package. It was not rebuilt or
modified for submission. Its image structure and checksum were checked locally.

| Item | Value |
| --- | --- |
| Product version | 3.1.1 |
| Build date | 2026-10-17 |
| Framework | Arduino + ESP-IDF, ESP-IDF 5.5.2 |
| Chip family | ESP32-S3 |
| Image and configured flash size | 16 MiB (16,777,216 bytes) |
| File | `firmware/3.1.1/edcbook_full.bin` |
| Flash offset | `0x0` |
| SHA-256 | `588539e79aadf60543c3c80eccaf1086b6fe7fcb17950edaade69c4e43417958` |
| Manifest | [firmware/3.1.1/manifest.json](firmware/3.1.1/manifest.json) |

## Validation and physical-device test record

The 3.1.1 artifact described above passed its release build, host regression,
image-structure, and checksum validation. The author's 2026-10-17 release
validation did not include flashing this exact image to a physical device for
a new hardware acceptance pass. No new 3.1.1 physical-device result is claimed
in this submission.

The firmware author confirmed the following earlier results on **2026-10-12**
for the 3.1.0 artifact `edcbook_20261011.bin` (SHA-256
`a64d2c158277b98452900703ff102e97278b8b4acea1f99577713d1161d08242`). These
author-reported tests apply to 3.1.0, not to the new 3.1.1 package. Local
package validation checks the submitted files, not hardware behavior.

- Device: physical Seeed reTerminal Sticky, ESP32-S3.
- Hardware revision: not reported by the tester.
- Installation: complete firmware image over USB, including a repeated full
  installation after reconnecting USB. The specific flashing tool was not reported.
- Tested product version: 3.1.0; ESP-IDF 5.5.2.

| Check | Author-confirmed 3.1.0 result |
| --- | --- |
| Power-on and first boot | Passed |
| Open a book and turn pages | Passed |
| Touch and the hardware buttons used by the firmware | Passed |
| Reboot and restore reading progress and settings | Passed |
| Reconnect USB and repeat the complete firmware installation | Passed |

The 3.1.0 test confirmation covers these workflows; it is not a claim that
every supported file format, every setting, or every hardware revision was
tested. Equivalent results for 3.1.1 require a new physical-device test.
