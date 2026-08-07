# TRMNL

This partner entry provides the official TRMNL firmware package prepared for
browser-based installation on reTerminal Sticky.

Version 1.8.10 was prepared from the official TRMNL `devices_simplified`
source branch at base commit `f0cf1479d6c145b829c2cf0e16f1f425161513be`.
The package uses the Sticky device mapping validated for this build and is
distributed under the upstream GPL-3.0 license.

The firmware directory contains one merged ESP32-S3 binary written at offset
`0x0`. It includes the bootloader, partition table, application, and TRMNL
filesystem image produced by the official project build flow.

The 1.8.10 build and USB upload were tested on reTerminal Sticky production
hardware. The included preview records the firmware running on physical
devices after installation.

Project source, documentation, and support are provided by the official
[TRMNL firmware repository](https://github.com/usetrmnl/trmnl-firmware).
