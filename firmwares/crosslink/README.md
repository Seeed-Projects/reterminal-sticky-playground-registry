# CrossLink

CrossLink provides two e-reader firmware variants for reTerminal Sticky:
CrossLink Pixel, with a pixel-style interface and Chinese text layout, and
CrossLink 2.0.

- Project and documentation: <https://crosslink-reader.top/>
- License: MIT, as confirmed by the CrossLink author.
- Hardware: reTerminal Sticky with 32 MB flash.
- CrossLink Pixel: `4.3.0` (default variant); `4.2.1` remains available unchanged.
- CrossLink 2.0: `1.3.0`, packaged from the contributor-supplied
  `flash-sticky-1.3.0.bin` with its original filename and bytes preserved.

Back up device data, then select a variant and version in the Playground browser
flasher. Both variants use complete images written at `0x0` with ESP32-S3 /
DIO / 80 MHz / 32 MB settings. Each manifest records the file size and SHA-256.

The contributor flashed CrossLink Pixel `4.3.0` and CrossLink 2.0 `1.3.0` to
physical reTerminal Sticky hardware and confirmed normal operation.
