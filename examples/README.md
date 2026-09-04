# Firmware starter projects

Minimal projects for the build systems the Registry compiles in CI. Copy one into
your firmware entry as `source/`, then point `build` in `firmware.json` at it.

| Directory | Build system | `firmware.json` build block |
| --- | --- | --- |
| `platformio-starter/` | PlatformIO | `{ "system": "platformio", "environment": "sticky", "projectPath": "source" }` |
| `arduino-starter/` | Arduino CLI | `{ "system": "arduino", "profile": "sticky", "projectPath": "source" }` |

For ESP-IDF projects, start from `firmwares/_template/source/`.

CI compiles both starters on every change to the build pipeline, so they double as a
health check for the PlatformIO and Arduino channels.

## Pinning toolchain versions

Both starters pin the toolchain version that builds them: `platform = espressif32@6.13.0`
in `platformio.ini`, and `platform: esp32:esp32 (3.3.11)` in `sketch.yaml`. Keep a pin in
your own project so a rebuild months from now produces the same binary. Choose whichever
version your code needs.

## Board settings

The starters use stock ESP32-S3 board settings so they stay easy to compile. reTerminal
Sticky ships 32 MB of flash and octal PSRAM, so a real firmware usually adds:

- PlatformIO: `board_upload.flash_size` and `board_build.arduino.memory_type` in `platformio.ini`
- Arduino: the matching `PSRAM` and `FlashSize` options in the `fqbn` string, for example
  `esp32:esp32:esp32s3:PSRAM=opi,FlashSize=32M`

See [docs/contributing-firmware.md](../docs/contributing-firmware.md) for the full
contribution flow.
