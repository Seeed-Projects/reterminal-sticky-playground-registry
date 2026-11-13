# CrossInk

This partner entry provides CrossInk `2.0.0-rc2` for browser-based installation
on reTerminal Sticky. It is a release candidate, listed in the beta channel.

The contributor supplied the complete four-file package. Its Sticky application
is byte-identical to `firmware-sticky-v2.0.0-959e3d1-RC.bin` from the official
[v2.0.0 RC-2 release](https://github.com/uxjulia/CrossInk/releases/tag/v2.0.0-rc2).
The bootloader, partition table and OTA selector match the existing 1.5.1 package.
The upstream license is MIT.

The package is stored under `firmware/2.0.0-rc2/` and writes the bootloader at
`0x0`, partition table at `0x8000`, OTA selector at `0xe000`, and application at
`0x10000`. File sizes and SHA-256 values are recorded in the manifest.
The Seeed contributor confirmed successful physical-device testing on reTerminal
Sticky. Versions `1.5.1` and `1.5.0-beta` remain available unchanged.

English is now the only built-in interface language. Other interface languages
and non-English EPUB hyphenation use SD card packs; see the release instructions.

CrossInk provides an e-reader experience with configurable typography,
bookmarks, reading statistics, reading modes, and touch controls. A compatible
microSD card stores books, settings, reading progress, and generated data.

Project source and support are available from the official
[CrossInk repository](https://github.com/uxjulia/CrossInk). User documentation
is available from the official [CrossInk website](https://www.crossink.dev).
