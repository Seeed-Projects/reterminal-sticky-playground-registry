# Sticky Bunny

Sticky Bunny turns reTerminal Sticky into a living virtual pet and a collection
of focused offline ePaper apps.

## Latest Registry package: 0.1.3

- Automatically detect SSD1677/SSD2677 and use the matching full, partial, grayscale, temperature, and sleep paths.
- Read panel identification through bidirectional GPIO14 at 2 MHz.
- Reset partial-refresh state after standby resume, fixing repeated tutorial-page refreshes on SSD1677.
- Keep the release profile, pet storage, onboarding, and existing app controls.

### Package provenance

- Source: [32db65d681aabb407a62b6dde632f041171a4fa1](https://github.com/limengdu/reTerminal_Sticky_Bunny/commit/32db65d681aabb407a62b6dde632f041171a4fa1).
- Build profile: `sticky-release`, PlatformIO `espressif32@6.11.0`, ESP-IDF `5.4.1`.
- Target: ESP32-S3, 32 MB flash, DIO, 80 MHz; USB serial flashing at 460800 baud.
- Origin: the maintainer's local release build used for the physical checks below. All three files were copied byte-for-byte. Where available, CI independently verifies the source; its artifacts are not the source of these package bytes.
- License: MIT.
- All historical version directories, manifests, and binaries remain unchanged.

Registry package `0.1.3` retains the exact hardware-tested bytes. The runtime application version is `0.1.3`; its ESP-IDF image descriptor still says `0.1.2`.

This is a manifest-driven USB installation package. The Registry lists the newest package first in `flash.versions`; the manifest provides the flash offsets and file hashes. Embedded application labels are disclosed here and are not changed in the binary.

| File | Offset | Bytes | SHA-256 |
| --- | --- | --- | --- |
| `bootloader.bin` | `0x0` | 22464 | `aa2b0c40438ca25fc9924c4c63e7ee22cc487afd6d694c18495b23becce664d9` |
| `partitions.bin` | `0x8000` | 3072 | `61058ba0d15dbbc7b8dd23f0247026a2be7614b99ec4143243458d00e454e18f` |
| `firmware.bin` | `0x10000` | 2139600 | `db46e5094556e59e6ad805ddd8803774dde26d4681fff4a734943bfe57a550d3` |

### Hardware and automated verification (2026-11-11)

| Check | Evidence / scope |
| --- | --- |
| SSD1677, exact packaged release | Tutorial pages 1 through 6 and repeated entry/exit passed; maintainer confirmed the result |
| SSD2677 | Hardware passed the preceding SPI-fix commit `2b2dccc1b0524409059b61577b98b7f369f86d54`; the final `32db65d` package was not re-tested on this revision |
| Source CI, exact commit | [Build and test run 42973854684](https://github.com/limengdu/reTerminal_Sticky_Bunny/actions/runs/42973854684) passed |
| Host coverage | 31 host tests and the real panel-driver regression suite, including six consecutive SSD1677 standby/partial cycles |
| Package integrity | Every copied file matches the release output; application and bootloader ESP32-S3 checksums and validation hashes are valid |

The earlier hardware checklist below describes version 0.1.2. It is not a claim that every pet/app, sleep, rotation, persistence, USB, or repeated-install workflow was re-run for 0.1.3. Final-package SSD2677 acceptance remains a review follow-up.

### Upgrade notes

Select `0.1.3` for this update. Save the pet state before upgrading. Keep the existing flash contents when updating the same project if you need its saved data; a full erase deletes that data. When switching from a different project, use a clean installation as described below. No partition layout is changed by this update.

## What it does

- **Virtual pet.** Hatch, name, feed, pet, talk to, and play with a rabbit that
  grows through five stages, develops one of three personalities, remembers
  daily care, sleeps, and occasionally goes outside.
- **Pomodoro timer.** Start a 15, 25, or 60 minute focus session, enter a custom
  duration, pause or end the countdown, and receive a gentle repeating alarm.
- **Status Board.** Display `BUSY`, `MEETING`, `ON CALL`, `OPEN TO TALK`, `REST`,
  or a custom message in landscape orientation.
- **Book of Answers.** Hold a question in mind and shake for three seconds to
  reveal a message, `YES`, `NO`, or `UNCLEAR`.
- **Hardware interaction.** Use touch, side buttons, swipe gestures, rotation,
  continuous shaking, RTC wake scheduling, battery display, and deep sleep.

## Controls

- Tap the AI key or swipe up from the bottom of the screen to open the app
  launcher.
- Tap an app card to open it, or swipe down from the upper screen to close the
  launcher.
- Rotate from landscape to portrait while the launcher is open to enter
  Pomodoro.
- Rotate from portrait to landscape while the launcher is open to enter the
  Status Board.
- Shake while the launcher is open to enter the Book of Answers.
- Double-tap the AI key from any page to return to the pet.
- Hold both non-AI side keys to enter deep sleep.

## Historical 0.1.2 package origin

Version `0.1.2` was built from
[reTerminal Sticky Bunny](https://github.com/limengdu/reTerminal_Sticky_Bunny)
commit `cf63d3fd4978a5f18fb5b779c197a17a91c59fe1` with the `sticky-release`
PlatformIO environment. The build uses `espressif32@6.11.0`, ESP-IDF 5.4.1,
the `sticky_esp32s3` board definition, and the project's 32 MB partition table.

The Registry package contains the exact release build's bootloader, partition
table, and application image at offsets `0x0000`, `0x8000`, and `0x10000`.

## Installation and first boot

Connect reTerminal Sticky with a USB-C data cable and install the package from
the Playground using desktop Chrome or Edge. A clean installation shows the
six-page illustrated guide, followed by the egg-hatching and naming flow.

Uploading without an erase keeps the existing NVS pet save. Accepting the erase
prompt creates a fresh save and shows onboarding again.

## Historical 0.1.2 physical-device test

The `sticky-release` profile has been exercised on reTerminal Sticky production
hardware throughout development, including:

- [x] battery and USB power-on;
- [x] first-boot tutorial, egg hatching, naming, and pet persistence;
- [x] touch, all three side buttons, swipe gestures, rotation, and shaking;
- [x] virtual pet, Pomodoro, Status Board, and Book of Answers;
- [x] battery display, RTC time, scheduled wake, and deep sleep;
- [x] reboot with the pet save restored.
- [x] GT911 startup at `480x800`, repeated touch detection, and tutorial page
  navigation with the exact release files.

The exact files in `firmware/0.1.2/` are ready for the Registry-manifest flash
and repeated-install acceptance check recorded in the contribution pull request.

## Links

- [Project source and documentation](https://github.com/limengdu/reTerminal_Sticky_Bunny)
- [Sticky official website](https://www.seeedstudio.com/sticky/)
- [reTerminal Sticky product page](https://www.seeedstudio.com/reTerminal-Sticky-p-6861.html)
- [Project support](https://github.com/limengdu/reTerminal_Sticky_Bunny/issues)
