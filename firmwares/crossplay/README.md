# CrossPlay

CrossPlay is a fork of [CrossPoint](https://crosspointreader.com/) that keeps the
whole e-reader and adds the other things a screen that holds still is good at.
The same build runs on reTerminal Sticky and the Xteink X4 Pro.

- Project: https://github.com/ma-r-s/crossplay
- Site: https://crossplay.ma-r-s.com
- Issues: https://github.com/ma-r-s/crossplay/issues
- Upstream: https://github.com/crosspoint-reader/crosspoint-reader
- License: MIT

## What it adds

25 games you think about rather than react to: Chess, Go, Hex, Checkers,
Connect Four, Yahtzee, Knucklebones, Battleship, Hearts, Solitaire, Sudoku,
Minesweeper, Picross, Wordle, Connections, Murdle, D&Diagrams, Jaipur, Sea
Salt, Toy Battle, Underhand, Insider, Forehead, Trivia and Wavelength.

11 apps: Study (Anki decks reviewed offline), Wikipedia (fifty thousand
articles on the card, in English or German), Weather, Notes, RSS, Reading
Stats, Hacker News, the xkcd archive, Instapaper, Wallpapers and a Calculator.

Eleven of the games (Chess, Checkers, Connect Four, Yahtzee, Knucklebones,
Battleship, Jaipur, Sea Salt, Toy Battle, Go and Hex) can be played between two
devices side by side over PLAY NEARBY. They find each other by themselves:
nothing to pair, no account and no network.

Reading is unchanged. The EPUB engine, sync and file browser are CrossPoint's.

## Controls

Touch throughout. The two side keys (GPIO5, GPIO6) page the shelf and the
reader; the shared Confirm/Power key (GPIO4) confirms on a click and sleeps on a
hold. Back is a left-to-right swipe starting from the left edge of the panel,
not a button. The games are driven by touch and do not use the side keys.

## Setup

1. Flash from the Playground card.
2. When the flasher says it is done, press the reset button with a pin: the
   small hole on the bottom, first on the left. The Sticky then starts CrossPlay.
3. Insert a microSD card before first boot. Saves, player identity, books and
   flashcard decks all live on the card, and the Sticky does not ship with one.
   Without a card the firmware stops at a full-screen "SD card error" and goes
   no further.
4. Open the shelf and pick an app.

## Package origin

The `firmware/1.16.1/` package is the published release artifact for
[v1.16.1](https://github.com/ma-r-s/crossplay/releases/tag/v1.16.1), the image
the project's own installer writes. It is a single merged image written at
offset 0, not a four-part package.

- File: `crossplay-v1.16.1-sticky-full.bin`
- Origin: https://github.com/ma-r-s/crossplay/releases/download/v1.16.1/crossplay-v1.16.1-sticky-full.bin
- Size: 7,444,976 bytes
- SHA-256: `6c1626e40dbb1fb56e982835db2c6fa2b8c5a8a9fc17b9594cedad34315c676e`
- MD5: `16a939604c9fbc23bd37d3123b6c0744`

`firmware/1.12.11/` stays as it was, the earlier release:

- File: `crossplay-v1.12.11-sticky-full.bin`
- Origin: https://github.com/ma-r-s/crossplay/releases/download/v1.12.11/crossplay-v1.12.11-sticky-full.bin
- Size: 6,643,296 bytes
- SHA-256: `c07efd799ee3d520ff5ff7e407b3401f5e2dfd271fc226d3e248c93f848b21cc`
- MD5: `d7a2f89dcff2fb41fe52836a96ca3d60`

The merged image contains the bootloader at 0x0 (ESP32-S3, chip id 0x09), the
partition table at 0x8000, erased OTA-selection data at 0xe000 and the
application at 0x10000. The partition table spans exactly 16MB, with two 7.94MB
OTA slots, which is where the manifest's `flashSize`, `flashMode` (dio) and
`flashFreq` (80m) come from: all four were read back out of the image header
rather than copied from another entry.

## Hardware test record

**Version 1.12.11, the version packaged here, has been flashed to a reTerminal
Sticky by the project maintainer, run, driven with the device's physical
buttons, and restarted.**

Not separately re-checked at 1.12.11: saved state across that restart, USB
reconnection, and a repeated installation over an existing one. He did not
record a screen-by-screen matrix for the run either, so this entry does not
claim one at 1.12.11. What follows is the detailed record from an earlier
version on the same hardware (the production sample Seeed supplied), version
**1.4.0** on 2026-08-26, driven over a serial bridge with screenshots read back:

- boot to Home, and shelf paging with the two side keys;
- a full game of Chess against the on-device engine;
- Solitaire in landscape, with the orientation-mapped Back swipe;
- Study's empty state, and Settings;
- PLAY NEARBY radio init and the LOOKING screen;
- PLAY NEARBY between the Sticky and an Xteink X4 Pro: discovery, pairing, and a
  chess opening exchanged correctly in both directions;
- microSD: a blank card formatted to FAT32, mounted at 40MHz on the shared
  display bus; a game save and the player identity both survived a reboot;
- heap approximately 243KB free idle, 216KB with the radio up.

Every release, 1.12.11 included, is built for the Sticky by CI and covered by
the project's host test suite and its stack-budget check, which runs against the
`sticky` build specifically.

Not verified:

- **PLAY NEARBY between two Stickys.** Only one Sticky exists here, and
  two-device play needs two. Sticky-to-X4-Pro is the verified path.
