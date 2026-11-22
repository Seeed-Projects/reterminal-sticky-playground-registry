# HeySticky

This community entry publishes HeySticky `1.2.0` for browser-based installation
on reTerminal Sticky.

HeySticky is a personal assistant firmware. The e-paper screen is a dashboard:
a big clock, weather from Open-Meteo, indoor temperature and humidity, and
today's to-dos. You add things by voice (hold the top button and talk; an AI
turns it into to-dos with deadlines or a note) or from a page on your phone.
English and Vietnamese.

Project, full documentation and support:
[TaDuy2005/heysticky](https://github.com/TaDuy2005/heysticky).

## Package origin

The four files are unchanged from the upstream
[v1.2.0 GitHub Release](https://github.com/TaDuy2005/heysticky/releases/tag/v1.2.0)
(tag `v1.2.0`, commit `eebe90bb3cb6cdb7f7a27e99d724fd43ba5d3512`), built with
the ESP32 Arduino core 3.3.12 and distributed under the upstream MIT license.

| Part | Offset | Upstream file | SHA-256 |
|---|---|---|---|
| `bootloader.bin` | `0x0` | `bootloader.bin` | `6ca3000286a37da6936168f9484b54ed51b24a8c2ea8b3ffb3a77e9bb8dba645` |
| `partition-table.bin` | `0x8000` | `partition-table.bin` | `82c24e8dc044135a61780f55a284d5fae3f7ea1370c2d36f3943330d91d057ae` |
| `boot_app0.bin` | `0xe000` | `boot_app0.bin` | `f94c5d786a7a8fab06ac5d10e33bf37711a6697636dc037559ea19cc410a17f0` |
| `heysticky.bin` | `0x10000` | `heysticky.bin` | `dd4f98328408307ac9eab79bd10dd099f140a35c74dab78fa23b3d6b0997154a` |

The partition table uses the 32 MB flash with two 12.5 MB OTA slots, so later
versions install over Wi-Fi from **Settings › Software Update** (GitHub
Releases of the upstream project, with rollback if a new version fails to start).

## Features

- **Home:** greeting, clock (built-in RTC, 12-hour AM / PM), weather, indoor
  temperature and humidity, today's to-do count.
- **To-do:** tap to tick, long-tap to open; deadlines with date and time.
  Stored as plain Markdown in `/todo.md` on the microSD card.
- **Notes:** one Markdown file per note in `/notes/`; read and delete on the Sticky.
- **AI voice:** hold the top button and talk (up to 60 s) to add to-dos or a note.
- **Phone pages** on the Sticky's own hotspot (`192.168.4.1`): name, city, Wi-Fi,
  fonts, AI keys, and To-do / Notes editing.
- **Sleep** with a live clock, shutdown with UP + DOWN, English / Tiếng Việt.

## Controls on reTerminal Sticky

| Action | How |
|---|---|
| Home | Press the top button, or swipe up from the bottom edge |
| Back | DOWN, or swipe left-to-right |
| Next / previous page | Swipe up / down |
| Tick / open a to-do | Tap / long-tap it |
| Talk to AI | Hold the top button while you speak, then let go |
| Turn off | Hold UP + DOWN together for 3 s |
| Turn on | Hold the top button for 2 s |

## First setup

1. Insert a FAT32 microSD card and install the firmware.
2. With no saved Wi-Fi, the Sticky opens **Web setup**: scan the QR code with
   your phone, then open `192.168.4.1`.
3. Set your name, city and Wi-Fi.
4. For voice, add an OpenAI-compatible API key under **Integrations › AI**
   (OpenRouter by default; about US$1 a month at 20 short memos a day).
   See [AI setup](https://github.com/TaDuy2005/heysticky#ai-setup).

Wi-Fi passwords and API keys are kept only in the Sticky's flash. They are
never part of the firmware files. Voice recordings are sent over HTTPS to the
speech-to-text service you choose; nothing else leaves the Sticky.

## Hardware test

| | |
|---|---|
| Device | reTerminal Sticky (ESP32-S3R8, 8 MB PSRAM, 32 MB flash, 3.97" e-paper, GT911 touch) |
| Firmware | HeySticky 1.2.0 |
| Installation | esptool, the four files above at their offsets |
| Result | See the pull request |

Upstream device tests for v1.0.0 – v1.2.0 (first boot, Home, To-do, Notes,
languages, sleep and power, over-the-air update, AI voice, phone pages) passed;
the record is in the upstream repository history.
