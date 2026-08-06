# Sticky 2048

Sticky 2048 is a community 2048 game for Seeed Studio reTerminal Sticky. It
uses native ESP-IDF and the device's E-Ink display, touch controller, AI button,
battery gauge, and non-volatile storage.

The game renders directly to a 1-bit E-paper framebuffer. It does not depend on
LVGL, networking, cloud services, or a filesystem.

## Features

- Native ESP-IDF implementation
- Portrait 480 × 800 interface on the SSD1677 E-Ink display
- 2048 game logic implemented in C++
- GT911 touch swipe control with an independent polling task
- Partial refresh during gameplay with periodic full-refresh cleanup
- AI button support
  - Short press starts a new game
  - Long press saves the current game state and powers off
- BQ27220 battery percentage display
- External-power detection
- NVS persistence for the best score and current game state
- Optional LEDC buzzer interface

## Controls

- Swipe up, down, left, or right to move the tiles.
- Tap **NEW GAME** to restart the game.
- Short-press the AI button to start a new game.
- Long-press the AI button to save the current game and power off.

## Hardware requirements

- Seeed Studio reTerminal Sticky
- ESP32-S3 with PSRAM
- 3.97-inch 800 × 480 E-Ink display
- USB data cable for building and flashing

## Firmware package

- Registry firmware version: `1.0.1`
- Source: <https://github.com/Lukilyy/reterminal-sticky-2048-eink-game>
- License: MIT
- Build system: ESP-IDF `v5.4`
- Build target: `esp32s3`
- Package origin: GitHub Actions builds the submitted `source/` directory and
  generates the flash manifest and firmware binaries for this Registry entry.

## Build and flash locally

Install ESP-IDF v5.4, open a terminal in `source/`, and run:

```bash
idf.py set-target esp32s3
idf.py -D PROJECT_VER=1.0.1 build
idf.py -p PORT flash monitor
```

Replace `PORT` with the serial port for the reTerminal Sticky. Project defaults
are provided by `source/sdkconfig.defaults`; generated build files and
`sdkconfig` are intentionally excluded from version control.

## Project structure

```text
source/
├── main/
│   ├── game/          # 2048 game logic and NVS save state
│   ├── hardware/      # Display, touch, battery, button, and buzzer adapters
│   ├── ui/            # E-Ink framebuffer rendering and bitmap font
│   ├── main.cpp       # Application entry and refresh management
│   └── pin_config.h
├── components/
│   ├── bq27220/       # Battery fuel-gauge driver
│   ├── button/        # Button driver
│   ├── debug_logging/ # Logging dependency used by hardware drivers
│   ├── gt911/         # Touch-controller driver
│   └── seeed_epaper/  # E-Ink display driver
├── CMakeLists.txt
├── sdkconfig.defaults
└── LICENSE
```

## Hardware mapping

| Function | GPIO |
|---|---:|
| AI / power button | 4 |
| External-power detect | 9 |
| E-Paper SPI | 13–18 |
| GT911 I2C | 2, 3 |
| GT911 enable / interrupt / reset | 42, 21, 41 |
| BQ27220 I2C | 0, 1 |
| Charger enable | 39 |
| Buzzer PWM | 48 |
| Power hold / lock | 45, 46 |

## Physical-device test record

Firmware version `1.0.1` was built with ESP-IDF `v5.4` and tested on physical
reTerminal Sticky production hardware. All required tests passed.

| Item | Result |
|---|---|
| Hardware | reTerminal Sticky production hardware |
| Firmware version | `1.0.1` |
| ESP-IDF version | `v5.4` |
| Power-on and first boot | Passed |
| Game workflow and touch swipes | Passed |
| AI button short/long press | Passed |
| Reboot and saved state | Passed |
| USB reconnection and repeated installation | Passed |

## Scope

This project focuses on the core 2048 gameplay experience and E-Ink interaction
on reTerminal Sticky. It does not include Wi-Fi, BLE, OTA, MQTT, cloud services,
LVGL, or unrelated application features.

## License

The submitted source is available under the MIT License. See
[`source/LICENSE`](source/LICENSE).
