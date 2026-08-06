# Example Community Firmware

Copy this directory to `integrations/<integration-id>/`, then provide:

1. Complete metadata in `integration.json`.
2. A licensed, reproducible ESP-IDF project under `source/`. GitHub Actions
   builds and packages the firmware for this contribution mode.
3. For a firmware-only contribution, an upstream source URL and license plus
   the packaged files under `firmware/<version>/`.
4. A real Sticky preview under `assets/`, plus an optional project logo when needed.
5. The package origin, firmware behavior, and physical-device test result in
   this README.

Run `npm test` and `npm run validate` from the repository root before opening a
pull request.

- [English contribution guide](../../CONTRIBUTING.md)
- [中文贡献指南](../../CONTRIBUTING.zh-CN.md)
