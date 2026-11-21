# PocketPlay

## What it is

Games only firmware for the Seeed reTerminal Sticky

## Source

- Project: https://github.com/jsrijith/pocketplay
- License: GPL-2.0-or-later for the firmware image. PocketPlay's own code is MIT, but the image links wolfSSL, which is GPL-2.0-or-later, so the image as a whole is under the GPL. Each release's tag is its source.

## Package

- Version: 0.7.0 (latest), from the [v0.7.0 release](https://github.com/jsrijith/pocketplay/releases/tag/v0.7.0)
- `pocketplay-full.bin` at 0x0 (2898960 bytes)
- SHA-256: `d5529ee4fe470f6e88abce8bf945667026c52cb03b83f4435a716b94033352ec`
- 0.4.0 stays available: `pocketplay-full.bin` at 0x0 (2830592 bytes)

Submitted by Srijith.

## What's new since 0.4.0

- **PocketBGG (0.7.0)**, a new app on the APPS page: your BoardGameGeek collection on the device. Put your BGG username and your own API token in `/pocketbgg/config.txt` on the card, and `SYNC` downloads your collection, each game's last-played date and each game's cover, dithered to black and white. It all stays on the card, so the list works offline. Filter by status, played or not and player count, sort by name or last played, and step through games' details with Up and Down.
- **Dice Roller (0.6.0):** a count of your rolls under the total, and a `RESET` button that clears the dice and starts the count again from 0.
- **Crown & Crest (0.6.0):** a count of the rolls under the dice, and a `RESET` button that starts a new game. Tap it twice to confirm.
- **D&Diagrams (0.5.0):** the chosen dungeon's name shows at the top of the map, and Up and Down step through the dungeons.

Saves from 0.4.0 are kept when updating to 0.7.0.

## Physical-device test record: 0.7.0

Reported by the project author, Srijith. These are the author's results, not an independent maintainer retest.

- Hardware: Seeed Studio reTerminal Sticky.
- Firmware: PocketPlay 0.7.0, `pocketplay-full.bin` from the v0.7.0 release, installed at `0x0`.
- SHA-256: `d5529ee4fe470f6e88abce8bf945667026c52cb03b83f4435a716b94033352ec`.
- Toolchain: ESP-IDF 5.5.5 (Arduino core 3.3.11, pioarduino platform 55.03.311), built by GitHub Actions from the tag.

| Check | Author-reported result |
| --- | --- |
| Power-on: boots to the splash screen (`VERSION 0.7.0`), then the home page | PASS |
| Main workflow: open a game from the home page and play it | PASS |
| Touch and buttons: taps, Up/Down to page, OK to sleep and wake | PASS |
| Reboot and saved state: sleep, wake, the same game and position come back | PASS |
| USB reconnection and repeat install over the same image | PASS |

## Physical-device test record: 0.4.0

Reported by the project author, Srijith, in [PR #137](https://github.com/Seeed-Projects/reterminal-sticky-playground-registry/pull/137#issuecomment-6952527861). These are the author's results, not an independent maintainer retest.

- Hardware: Seeed Studio reTerminal Sticky.
- Firmware: PocketPlay 0.4.0, `pocketplay-full.bin` installed at `0x0`.
- SHA-256: `aac0b1710ba118223709b29fb57605459a77dd7ddc1ae52177273598c3d13625`.
- Toolchain: ESP-IDF 5.5.5 (Arduino core 3.3.11, pioarduino platform 55.03.311).

| Check | Author-reported result |
| --- | --- |
| Power-on: boots to the splash screen, then the home page | PASS |
| Main workflow: open a game from the home page and play it | PASS |
| Touch and buttons: taps, Up/Down to page, OK to sleep and wake | PASS |
| Reboot and saved state: sleep, wake, the same game and position come back | PASS |
| USB reconnection and repeat install over the same image | PASS |

The preview is the author's [on-device photo](https://github.com/user-attachments/assets/572403e7-b4da-47b9-a513-772b42f36474), showing the 0.4.0 splash screen and games home page.
