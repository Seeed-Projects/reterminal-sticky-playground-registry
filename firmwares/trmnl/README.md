# TRMNL

This partner entry provides the official TRMNL firmware package prepared for
browser-based installation on reTerminal Sticky.

Version 1.8.11 was prepared from the official TRMNL `devices_simplified`
source branch at base commit `db991c323db02c3613945f87f1f44f32fd699d55`.
The package includes the Sticky SD and SPI bus initialization update used for
this build and is distributed under the upstream GPL-3.0 license.

The firmware directory contains one merged ESP32-S3 binary per version, written
at offset `0x0`. The 1.8.18 package was built locally from TRMNL firmware
commit `4ffa33acbe317e559aa50ee505ffa5fe4ed0272d` using the `seeed_sticky`
PlatformIO environment. It includes the bootloader at `0x0`, partition table
at `0x8000`, OTA initialization data at `0x13000`, and application at
`0x20000`. The merged bootloader header declares 32 MB Flash to match the
reTerminal Sticky hardware and PlatformIO's detected upload size. This build
does not include a filesystem image.

The 1.8.11 package was built locally for reTerminal Sticky. Physical-device
installation should be confirmed during review. The official TRMNL logo is
used across the catalog and installation views.

Project source, documentation, and support are provided by the official
[TRMNL firmware repository](https://github.com/usetrmnl/trmnl-firmware).
