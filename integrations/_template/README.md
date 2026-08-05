# Example Community Firmware

Copy this directory to `integrations/<integration-id>/`, then provide:

1. Complete metadata in `integration.json`.
2. A licensed, reproducible ESP-IDF project under `source/`.
3. The packaged firmware and manifest under `firmware/<version>/`.
4. A project logo and a real Sticky preview under `assets/`.
5. The firmware behavior and physical-device test result in this README.

Run `npm test` and `npm run validate` from the repository root before opening a
pull request.

- [English contribution guide](../../CONTRIBUTING.md)
- [中文贡献指南](../../CONTRIBUTING.zh-CN.md)
