# CrossMux

This partner entry provides CrossMux firmware for browser-based installation
from the reTerminal Sticky Playground.

Version `2.0.0` is stored under `firmware/2.0.0/` as a complete four-part
ESP32-S3 package. The contributor supplied the BINs downloaded from the official
website; all four match the Sticky package in the official
[2.0.0 release](https://github.com/0x1abin/crossmux/releases/tag/2.0.0).
The upstream license is MIT.

The package writes the bootloader at `0x0`, partition table at `0x8000`, OTA
selector at `0xe000`, and application at `0x10000`, using DIO / 80 MHz / 16 MB
settings. File sizes and SHA-256 values are recorded in the manifest.

The contributor flashed this package to a physical reTerminal Sticky and
confirmed successful operation. Version `1.6.0` remains available unchanged.
