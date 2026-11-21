# Sticky Assistant

## What it is

A push-to-talk voice assistant for the reTerminal Sticky. Hold the AI button,
ask, and the reply appears on the e-paper screen. The assistant can read and
change your calendar and to-do list and search the web. Swipe or use Up/Down
for pages: home (time, next event, weather), the assistant, calendar, to-dos,
weather and settings, plus plugins from the
[sticky-assistant plugin hub](https://github.com/biggy-spoon/sticky-assistant-plugin-hub).

It needs the free **sticky-assistant backend** running on a computer on the
same network (Docker or Python). The backend does the speech recognition
(Whisper, on that computer) and talks to the AI you choose: Claude, Gemini,
OpenAI, or a local OpenAI-compatible server such as Ollama.

## Setup

1. Start the backend: `docker compose up -d` in the project repository (see its
   README).
2. Install this firmware, erasing first if another firmware is on the device.
3. The screen shows how to finish: join the `StickyAssistant-Setup` WiFi
   network, then enter your WiFi and the backend's address, such as
   `http://192.168.1.50:8000/converse`.
4. The rest (AI provider, API key, calendar, units, timezone, home page) is on
   the settings page the device serves at its own address.

## Controls

- **AI button:** hold to talk; a short press goes back, or sleeps on the home
  page; a double press redraws the screen.
- **Up / Down:** previous and next page. Held together for two seconds at
  start-up, they reopen WiFi setup.
- **Touch:** tap tiles, rows and pickers; swipe left and right between pages,
  up and down to scroll.

The device sleeps after five minutes without input (adjustable) and any
button wakes it.

## Source

- Project: https://github.com/biggy-spoon/sticky-assistant
- License: MIT (the e-paper driver follows Sticky OS's MIT SSD1677 driver; the
  fonts are under the SIL Open Font License)

## Package

- Version: 1.0.0, built by the project's release workflow from tag `v1.0.0`
  with ESP-IDF 5.3 (`firmware/1.0.0/`, from the release's zip)
- `bootloader.bin` at 0x0, `partition-table.bin` at 0x8000,
  `sticky-assistant.bin` at 0x10000; sizes and SHA-256 in `manifest.json`

## Hardware test

| | |
|---|---|
| Hardware | reTerminal Sticky (ESP32-S3 rev v0.2, 32MB flash, 8MB PSRAM) |
| ESP-IDF | 5.3 |
| Firmware | 1.0.0 |
| Installed with | the release files at the offsets in `manifest.json` (esptool) |

- [x] Power-on and first boot with empty settings: the setup screen appears and
      the `StickyAssistant-Setup` network comes up
- [x] Setup: WiFi and backend entered; the device restarts onto its home page
- [x] Main workflow: hold the AI button, ask a question, the reply appears
- [x] Touch (tiles, swipes, pickers) and all three buttons
- [x] Reboot and sleep/wake: settings and the current page are kept
- [x] USB reconnection, and a second installation

Result: **pass**. Every check above passed on version 1.0.0, installed from the published release files.

Submitted by [biggy-spoon](https://github.com/biggy-spoon).
