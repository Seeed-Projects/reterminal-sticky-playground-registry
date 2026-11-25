# MicroSlate

MicroSlate turns reTerminal Sticky into a dedicated writing machine with a
Bluetooth LE keyboard, microSD auto-save, and Wi-Fi note sharing and cloud sync.

- Website: <https://typeslate.com/>
- Documentation: <https://typeslate.com/tools/microslate/>
- Source: <https://github.com/Josh-writes/microslate-firmware>
- Author: [Josh-writes](https://github.com/Josh-writes)
- Version: `3.0.2` (stable). License: MIT; see `LICENSE`.

The contributor supplied the four-file Sticky package. Its `firmware.bin` is
byte-identical by SHA-256 to the official `microslate-sticky-v3.0.2.bin`
[Release asset](https://github.com/Josh-writes/microslate-firmware/releases/tag/v3.0.2).
Original filenames and bytes are preserved; offsets, sizes, and SHA-256 values
are recorded in `firmware/3.0.2/manifest.json` (ESP32-S3 / DIO / 80 MHz / 16 MB).

Back up device data, then install through the Playground browser flasher.
Insert a microSD card and pair a Bluetooth LE keyboard. Wi-Fi note transfer and
cloud sync are optional; cloud sync requires a OneDrive, Google Drive, or Dropbox account.

The Seeed contributor flashed this package to a physical reTerminal Sticky
and confirmed successful operation.

## Version 3.0.5

Adds improved Bluetooth keyboard pairing, update checks, and support for both
Sticky screen controllers. The application matches the official
[v3.0.5 release](https://github.com/Josh-writes/microslate-firmware/releases/tag/v3.0.5)
by SHA-256. Bootloader, partitions, and OTA initialization files are retained
from 3.0.2; the upstream partition layout is unchanged. Version 3.0.2 remains
available. The Seeed contributor flashed the 3.0.5 package to a physical
reTerminal Sticky and confirmed successful operation.
