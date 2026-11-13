# CrossPoint Reader

This partner entry provides CrossPoint Reader firmware packages for browser-based
installation from the reTerminal Sticky Playground.

Version `2.0.0` is the official stable release, stored under `firmware/2.0.0/`.
The contributor supplied the complete four-file package. The application is
byte-identical to `crosspoint-2.0.0-sticky.bin` from the official
[2.0.0 release](https://github.com/crosspoint-reader/crosspoint-reader/releases/tag/2.0.0).
The bootloader, partition table and OTA selector match the existing 1.6.0 package.
The upstream license is MIT.

The package writes the bootloader at `0x0`, partition table at `0x8000`, OTA
selector at `0xe000`, and application at `0x10000`, using ESP32-S3 / DIO /
80 MHz / 16 MB settings. File sizes and SHA-256 values are recorded in the manifest.

The Seeed contributor confirmed that this 2.0.0 package was flashed and tested
successfully on physical reTerminal Sticky hardware.

Versions `1.6.0` and `1.5` remain available unchanged.
