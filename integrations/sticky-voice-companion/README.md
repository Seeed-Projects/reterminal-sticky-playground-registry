# Voice Companion

> Speak to your Home Assistant, read the full answer as ink — on a Sticky
> that also carries your lists, agenda, lights, timers, and one shared
> household note board.

Voice Companion is an ESPHome firmware that turns reTerminal Sticky into a
battery-aware Home Assistant Assist satellite with an 8-page touch e-ink UI.
Full documentation, source, and per-entity configuration live in the
[source repository](https://github.com/sira-fiinikkusu/reterminal-sticky-voice-companion).

## Behavior

- **Voice**: press OK and talk to your assistant — push-to-talk, no
  on-device wake word. The Sticky has no speaker — the confirmation
  rides the pipeline and the firmware exposes a `show_followup` Home
  Assistant action so automations or an LLM agent can write the complete
  final reply onto the display, unabridged.
- **Pages** (swipe or UP/DOWN buttons): Main dashboard (clock, weather,
  assistant note, inside/outside temps over etched watermark art) · Notes ·
  Shopping list with tap-to-check-off · Today's agenda · 3-day forecast ·
  Lights (six rooms, tap a light to toggle, tap the room name for all) ·
  Timers (live countdown, on-device triple-beep alarm) · Household Board.
- **Household Board**: hold OK on the board page and dictate. Every Voice
  Companion unit in the house beeps once, flips to the board, and shows the
  note until someone taps the ✕.
- **Power**: deep sleep with a 15-minute sensor heartbeat (battery,
  temperature, humidity — awake ~75 seconds per report), a 3-minute grace
  for human interaction, and hard guarantees: never sleeps during a voice
  session or while a timer runs. The e-ink repaints on events and a
  ~30-minute clock cadence, not on every wake.

## Controls

| Input | Action |
|---|---|
| OK button | Start a voice session (hold on Board page: dictate a note) |
| UP / DOWN buttons | Previous / next page |
| Swipe up / down | Scroll long content, change pages |
| Tap (Shopping) | Check an item off |
| Tap (Lights) | Toggle a light; tap the room label for the whole room |
| Tap ✕ (Board) | Clear the household note fleet-wide |

## Setup

1. Flash from the catalog page, then join the `reTerminal-Sticky` hotspot
   (password `ChangeMe123`) or use Improv over USB to enter Wi-Fi.
2. Adopt the device in Home Assistant's ESPHome integration and enable
   **Allow the device to perform Home Assistant actions** in its options.
3. Merge the helper configuration (three small bridge sensors + four
   helpers) from
   [`homeassistant.example.yaml`](https://github.com/sira-fiinikkusu/reterminal-sticky-voice-companion/blob/master/homeassistant.example.yaml)
   and restart Home Assistant once.
4. Pick an Assist pipeline for the satellite entity. Done.

The prebuilt image uses default entity names (`todo.shopping_list`,
`weather.forecast_home`, generic room lights). Compile your own build from
source to rename entities, rooms, the watermark art, or to add more fleet
units — each extra Sticky is a five-line wrapper file.

## Hardware test record

- Devices: 3× reTerminal Sticky (ESP32-S3R8, 32 MB flash), all flashed and
  in continuous household service.
- Verified on hardware: voice sessions via Assist pipeline; ink follow-ups;
  all 8 pages with touch; shopping check-off round-trip; board dictation +
  fleet-wide announce + ✕ clear; timer countdown, alarm, and
  sleep-hold; 15-minute heartbeat cadence observed stable (wake → ~75 s →
  sleep) with battery reporting throughout; OTA updates; first-boot
  captive-portal provisioning.
- Known behavior: the deep-sleep wake button is the physical OK/UP/DOWN
  group (touch alone does not wake a sleeping unit); the e-paper keeps its
  last frame while asleep by design.

## Version history

- **1.0.0** — initial release. ESPHome 2026.7.4, ESP-IDF-based toolchain.
