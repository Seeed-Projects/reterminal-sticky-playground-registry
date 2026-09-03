# Example Community Firmware

Copy this directory to `firmwares/<firmware-id>/`, then provide:

1. Complete metadata in `firmware.json`.
2. A licensed, reproducible ESP-IDF project under `source/`. GitHub Actions
   builds and packages the firmware for this contribution mode.
3. For a firmware-only contribution, an upstream source URL and license plus
   the packaged files under `firmware/<version>/`.
4. A real Sticky preview for a community entry, or official logo artwork for a partner entry.
5. Community author credit or coordinated partner identity links in `firmware.json`.
6. The package origin, firmware behavior, and physical-device test result in
   this README.

Run `npm test` and `npm run validate` from the repository root before opening a
pull request.

- [English guide: contributing firmware](../../docs/contributing-firmware.md)
- [中文指南：贡献固件](../../docs/contributing-firmware.zh-CN.md)
