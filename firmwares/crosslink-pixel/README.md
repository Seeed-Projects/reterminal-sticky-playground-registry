# CrossLink Pixel

CrossLink Pixel is a reading-focused firmware for reTerminal Sticky with a
pixel-style interface. The project describes smoother e-ink page turns,
Chinese text layout, responsive font rendering, and quick startup.

- Project and installation site: <https://crosslink-reader.top/>
- Package: firmware-only, version `4.3.0`
- License: MIT
- Target: reTerminal Sticky with **32 MB flash**

## Package provenance

The contributor supplied `pixel-sticky-full-v4.3.0T.bin` for this update.
The Registry package retains the original filename and bytes; it
has not been rebuilt. A public release URL or upstream SHA-256 for this exact
file has not been provided. The contributor confirms that the CrossLink author
authorized sharing this firmware on the reTerminal Sticky Playground.

The contributor reports that the CrossLink author identified this package as
MIT-licensed. The `source.url` field points to the official project website
for provenance; source for this exact Pixel build is not publicly linked here.

Thank you to the CrossLink team for supporting reTerminal Sticky and allowing
this firmware to be shared through the Playground.

## Flash layout

| Item | Value |
| --- | --- |
| File | `firmware/4.3.0/pixel-sticky-full-v4.3.0T.bin` |
| Flash offset | `0x0` |
| File size | 16,777,216 bytes (first 16 MB of flash) |
| SHA-256 | `5878f0b91392c78379526ee8db683e30750648d224182cde47c57cb01d40aeb1` |
| Chip | ESP32-S3 |
| Flash configuration | 32 MB, DIO, 80 MHz |
| Bootloader | Valid ESP image checksum and validation hash; ESP-IDF 5.4.2 |
| Application | Offset `0x20000`; valid ESP image checksum and validation hash |

The embedded application metadata reports project `eego` and build version
`df8ad769-dirty`. The contributor specifies product version `4.3.0` for the
supplied `4.3.0T` file; the embedded metadata does not contain that product version.

The embedded partition table has `ota_0` at `0x20000` (4 MB), `ota_1` at
`0x420000` (512 KB), `assets` at `0x4a0000` (11,648 KB), and `storage` at
`0x1000000` (16 MB). The last partition extends beyond this 16 MB image and
requires a physical 32 MB flash chip. Do not use this package on a 16 MB unit.

Version `4.2.1` remains available unchanged under `firmware/4.2.1/`.

## Installation and test record

Back up device data, then write the complete image at offset `0x0` using the
Playground browser flasher on a 32 MB reTerminal Sticky. This image replaces
the bootloader, partition table, application, and assets in the first 16 MB.
The `storage` partition occupies the second 16 MB and is not included in the
supplied file.

The contributor flashed the supplied `pixel-sticky-full-v4.3.0T.bin` to a physical
reTerminal Sticky and confirmed that the firmware booted and ran normally.
The flashing mode and individual reading workflows were not reported.
