# CrossLink Pixel

CrossLink Pixel is a reading-focused firmware for reTerminal Sticky with a
pixel-style interface and Chinese text layout.

- Project and documentation: <https://crosslink-reader.top/>
- Version: `4.3.0`
- License: MIT, as confirmed by the CrossLink author.
- Hardware: reTerminal Sticky with 32 MB flash.

This update replaces the 4.3.0 package with the contributor-supplied
`pixel-sticky-full-1113.bin`, adding SSD2677 driver support. The original
filename and bytes are preserved, with authorization to share on the Playground.
Version `4.2.1` remains available unchanged.

Back up device data, then install through the Playground browser flasher.
The complete image is written at `0x0` using ESP32-S3 / DIO / 80 MHz / 32 MB
settings. It covers the first 16 MB; storage occupies the second 16 MB.
File size and SHA-256 are recorded in `firmware/4.3.0/manifest.json`.

The contributor flashed this 4.3.0 package to a physical reTerminal Sticky
and confirmed that it booted and ran normally.
