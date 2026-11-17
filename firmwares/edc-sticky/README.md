# EDC Sticky

**Useful on its own – the phone app makes it as smart as you like.**

Custom firmware and an Android companion app that turn the reTerminal Sticky into an everyday-carry companion.

## Without the phone
A menu with groups and tabs: **Time** (clock, alarm that rings even while dozing, world time with day/night map,
timer) · **Measure** (spirit level, ruler in mm or inch) · **Capture** (voice recorder on microSD, on-screen keyboard
and number pad, scribble pad, single-stroke handwriting) · **Calculate** (calculator and unit converter with Braun
ET66-style keys, computed on the Sticky) · castle-duel game against the Sticky or for two · order slips and QR cards
(guest Wi-Fi, links, loyalty cards) · trip and climate logger · settings: 12/24 h, metric/US, German/English, set the
clock.

## With the app
Offline speech-to-text (whisper.cpp), weather, manuals and cheat sheets, new widgets, sync of notes, recordings and
logs, "find each other", two Stickys at one phone.

## Emergency mode (0.43)
An emergency page with name, ICE contacts and vital medical info that stays visible with a dead battery (e-paper needs
no power to keep the image), SOS in Morse via the buzzer, a power-save mode, PIN-protected documents (AES-256-GCM, key
wrapped with your PIN inside the device), offline maps of your own places (OpenStreetMap via OpenFreeMap) and a sun
compass. Fill it in without Python in the [Sticky Console](https://markussstaub-dot.github.io/sticky-lab/console/)
(tab "Notfall") or with the tools in the repo – guide: `docs/NOTFALL.md`. Not a replacement for a first-aid course.

## Setup
1. Back up the factory firmware (device-specific data): `esptool read-flash 0 0x2000000 backup.bin`.
2. Flash this firmware, insert a microSD card.
3. Install the Android app from the [releases](https://github.com/markussstaub-dot/reterminal-sticky/releases) and
   connect once – or, without the app, copy the `soll` folder of the microSD base kit from the same release to the card.
4. Optional: put the [Sticky Lab](https://github.com/markussstaub-dot/sticky-lab) (0.8.1 or later) into the second
   slot with `esptool write-flash 0x410000 sticky-lab.bin`; switch with the menu tile "Sticky Lab" or ▲ held at power-on.

Keys: AI short = Home (on Home: menu), AI hold = dictate, ▲/▼ = pages, ▲+▼ = help.

Status: beta, used daily. Hobby project, MIT license, feedback via
[GitHub issues](https://github.com/markussstaub-dot/reterminal-sticky/issues) without guarantee of an answer.
