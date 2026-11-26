# HeySticky

> [!WARNING]
> **Installed HeySticky before 1.3.1? Update once over USB.** Earlier versions
> use a flash layout that can't take a Wi-Fi update as big as 1.3.1, so
> Software Update on the Sticky can't install it. Install 1.3.1 once from this
> page; after that, updates come over Wi-Fi again. Settings stay; to-dos, notes
> and books are on the microSD card.

This community entry publishes HeySticky `1.3.1` for browser-based installation
on reTerminal Sticky.

HeySticky is a personal assistant firmware. The e-paper screen is a dashboard:
a big clock, weather from Open-Meteo, indoor temperature and humidity, and
today's to-dos. You add things by voice (hold the top button and talk; an AI
turns it into to-dos with deadlines or a note) or from a page on your phone.
It also reads EPUB, TXT and Markdown books from the microSD card.
English and Vietnamese.

Project, full documentation and support:
[TaDuy2005/heysticky](https://github.com/TaDuy2005/heysticky).

## Package origin

The four files are unchanged from the upstream
[v1.3.1 GitHub Release](https://github.com/TaDuy2005/heysticky/releases/tag/v1.3.1),
built with the ESP32 Arduino core 3.3.12 and distributed under the upstream MIT
license. The Reader's layout engine comes from
[CrossPoint Reader](https://github.com/crosspoint-reader/crosspoint-reader) (MIT);
its bundled licences are listed in the upstream `third_party/crosspoint/licenses/`.

| Part | Offset | Upstream file | SHA-256 |
|---|---|---|---|
| `bootloader.bin` | `0x0` | `bootloader.bin` | `6ca3000286a37da6936168f9484b54ed51b24a8c2ea8b3ffb3a77e9bb8dba645` |
| `partition-table.bin` | `0x8000` | `partition-table.bin` | `8b6f55e8a9bb2413792a7383f5c0b93aa197c0af6c2ec3f6d2d4edba1e02a23e` |
| `boot_app0.bin` | `0xe000` | `boot_app0.bin` | `f94c5d786a7a8fab06ac5d10e33bf37711a6697636dc037559ea19cc410a17f0` |
| `heysticky.bin` | `0x10000` | `heysticky.bin` | `ce53bc8a84a1845a630eb8cfcd84ef0351a5c07648844a1eed0a09eab24e093c` |

The partition table uses the 32 MB flash with two 8 MB OTA slots, both below
16 MB, so later versions install over Wi-Fi from **Settings › Software Update**
(GitHub Releases of the upstream project, with rollback if a new version fails
to start). Stickies installed before 1.3.1 need this install once: their
second OTA slot ends past 16 MB, too far for an image as big as 1.3.1.

The 1.2.0 package keeps its application file and now uses the same partition
table, matching the re-packed upstream
[v1.2.0 release](https://github.com/TaDuy2005/heysticky/releases/tag/v1.2.0).

## Features

- **Home:** greeting, clock (built-in RTC, 12-hour AM / PM), weather, indoor
  temperature and humidity, today's to-do count.
- **To-do:** tap to tick, long-tap to open; deadlines with date and time.
  Stored as plain Markdown in `/todo.md` on the microSD card.
- **Notes:** one Markdown file per note in `/notes/`; read and delete on the Sticky.
- **Reader:** EPUB (with pictures), TXT and Markdown books from `/books/`. Opens
  where you left off; Contents, text size S / M, and the built-in Noto Sans or a
  CrossPoint `.cpfont` font uploaded from the phone.
- **AI voice:** hold the top button and talk (up to 60 s) to add to-dos or a note.
- **Phone pages:** name, city, Wi-Fi, fonts, AI keys, To-do / Notes editing, and
  Reader (upload or delete books and `.cpfont` fonts). Served on the Sticky's own
  hotspot (`192.168.4.1`), or on your home Wi-Fi after a warning.
- **Report a bug:** say what went wrong, or scan a QR code for a pre-filled
  GitHub issue or email.
- **Sleep** with a live clock, shutdown with UP + DOWN, English / Tiếng Việt.

## Controls on reTerminal Sticky

| Action | How |
|---|---|
| Home | Press the top button, or swipe up from the bottom edge |
| Back | DOWN, or swipe left-to-right |
| Next / previous page | Swipe up / down |
| Turn a book page | Tap the right / left third, swipe up / down, or DOWN / UP |
| Reader menu | Tap the middle of the page |
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
5. For books, upload them on the phone page under **Reader › Books**, or copy
   them into `/books/` with the card in a computer.

Wi-Fi passwords and API keys are kept only in the Sticky's flash. They are
never part of the firmware files. Voice recordings are sent over HTTPS to the
speech-to-text service you choose; nothing else leaves the Sticky.

## Hardware test

| | |
|---|---|
| Device | reTerminal Sticky (ESP32-S3R8, 8 MB PSRAM, 32 MB flash, 3.97" e-paper, GT911 touch) |
| Firmware | HeySticky 1.3.1 |
| Installation | esptool, the four files above at their offsets |
| Result | See the pull request |

Upstream device tests for v1.0.0 – v1.3.1 (first boot, Home, To-do, Notes,
Reader, languages, sleep and power, over-the-air update, AI voice, phone pages,
Report a bug) passed; the record is in the upstream repository history.
