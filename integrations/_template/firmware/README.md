# Firmware packages

This directory is used by firmware-only contributions. Create one directory per
version and include `manifest.json` plus every `.bin` file referenced by it.

For source contributions, set `sourceBuild: true` in `integration.json` and
place the project under `source/`. GitHub Actions produces the firmware package.
