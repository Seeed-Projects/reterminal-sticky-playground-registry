# Sticky Lab

**A workbench for your reTerminal Sticky: find out what it can do – and where e-ink and the small battery set limits.**

Eleven stations, each one measures something and logs it to `/lab/messungen.csv` on the microSD:

1. **Refresh** – partial, partial-awake, full; timings and a ghosting probe
2. **Fonts** – the built-in fonts side by side, 4× magnifier
3. **Dither** – six methods on a gray ramp and a lit sphere
4. **Braun kit** – buttons, slider, knob, switch and tabs drawn on the panel, in idle, pressed and active state
5. **Motion** – bar, hand and counter animations, frames per second
6. **Touch** – aim at 9 targets, feedback latency, scribble with spiral and maze templates
7. **Gauges** – climate with dew point, spirit level, clock, battery
8. **Energy** – current while idle, per partial/full refresh and in light sleep, with an estimate per charge
9. **Castle duel** – a turn-based artillery game for two players or against the Sticky (hidden since 0.9 – it has no tile; find it on the overview)
10. **Keyboard** – QWERTY/QWERTZ on e-ink, typing speed and errors
11. **Pins & restarts** – live level, edges per second and interrupt/wake setup of every button and sensor pin; reset reason of the last starts, how long the previous start ran, restarts in a row and an interrupt-storm detector (> 50 interrupts/ms). After three crashes in a row the Lab opens this station by itself, so a crash loop is visible without a USB log

English and German (▲ on the overview). The **Sticky Console**
(<https://markussstaub-dot.github.io/sticky-lab/console/>) mirrors the screen at actual size – you can even draw
on it –, remote-controls the device, collects measurements, shows a live tech inventory and flashes firmware.

Some findings so far: a partial refresh takes ~0.85–0.95 s almost regardless of area; the Sticky's SSD1677/SSD2677
have no fast refresh; animation tops out at ~1.2 frames/s; light sleep draws ~6 mA; the power latch needs GPIO45
**and** GPIO46 high.

## Controls
AI short = overview · AI hold = screenshot to microSD · ▲/▼ = page or choose · tap = action · tap the status line (or ▲+▼) = help for the station.

## Setup
1. Back up the factory firmware (device-specific data): `esptool read-flash 0 0x2000000 backup.bin`.
2. Flash this firmware. A microSD card is optional (for measurements and screenshots).

## Hardware test record
- Hardware: two reTerminal Sticky units (SSD1677 each), ESP32-S3 32 MB flash, 8 MB PSRAM, microSD 32 GB.
- Build: Arduino core esp32 3.3.12, Seeed_GFX2, firmware 0.9.1 (earlier: 0.7.6).
- Tested: first boot and overview, all ten stations (by touch, keys and over USB), language switch kept after
  reboot, battery operation (unplugging, keys, stations), light sleep with wake (30 cycles without reset, on USB),
  USB reconnection and repeated installation with arduino-cli, measurements written to microSD.
- 0.9.x: installed on one unit (app0, as the Playground does): boot, overview, station 11 with reset history
  and restart counter, the hidden castle duel, repeated installation over USB. 0.9.1 only changes the label of the
  slot-switch button (neutral when not running in its own slot) and drops debug logging against 0.9.0.
- Result: works; see the lab log in the project repository.

Status: beta. Hobby project, MIT license, feedback via
[GitHub issues](https://github.com/markussstaub-dot/sticky-lab/issues) without guarantee of an answer.
