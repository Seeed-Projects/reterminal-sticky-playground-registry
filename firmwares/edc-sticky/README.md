# EDC Sticky

**Useful on its own – the phone app makes it as smart as you like.**

Custom firmware and an Android companion app that turn the reTerminal Sticky into an everyday-carry companion.

## Without the phone
Clock and status line · alarm (rings even while dozing) · world time · timer · calculator and unit converter (Braun
ET66-style keys, computed on the Sticky) · spirit level · ruler in mm or inch · order slips and QR cards (guest Wi-Fi,
links, loyalty cards) · scribble pad · single-stroke handwriting · voice notes buffered on microSD · trip and climate
logger · settings: 12/24 h, metric/US, set the clock.

## With the app
Offline speech-to-text (whisper.cpp), weather, logbook pages, new widgets, sync of notes and logs, "find each other".

## Setup
1. Back up the factory firmware (device-specific data): `esptool read-flash 0 0x2000000 backup.bin`.
2. Flash this firmware, insert a microSD card.
3. Install the Android app from the [releases](https://github.com/markussstaub-dot/reterminal-sticky/releases) and
   connect once.

Keys: AI short = Home (on Home: menu), AI hold = dictate, ▲/▼ = pages, ▲+▼ = help.

Status: beta, used daily. Hobby project, MIT license, feedback via
[GitHub issues](https://github.com/markussstaub-dot/reterminal-sticky/issues) without guarantee of an answer.
