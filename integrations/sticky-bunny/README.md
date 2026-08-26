# Sticky Bunny

Sticky Bunny turns reTerminal Sticky into a living virtual pet and a collection
of focused offline ePaper apps.

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

## Package origin

Version `0.1.1` was built from
[reTerminal Sticky Bunny](https://github.com/limengdu/reTerminal_Sticky_Bunny)
commit `55b3e67925603b266ba77673cdbb0e6d882925cb` with the `sticky-release`
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

## Physical-device test

The `sticky-release` profile has been exercised on reTerminal Sticky production
hardware throughout development, including:

- [x] battery and USB power-on;
- [x] first-boot tutorial, egg hatching, naming, and pet persistence;
- [x] touch, all three side buttons, swipe gestures, rotation, and shaking;
- [x] virtual pet, Pomodoro, Status Board, and Book of Answers;
- [x] battery display, RTC time, scheduled wake, and deep sleep;
- [x] reboot with the pet save restored.

The exact files in `firmware/0.1.1/` are ready for the Registry-manifest flash
and repeated-install acceptance check recorded in the contribution pull request.

## Links

- [Project source and documentation](https://github.com/limengdu/reTerminal_Sticky_Bunny)
- [Sticky official website](https://www.seeedstudio.com/sticky/)
- [reTerminal Sticky product page](https://www.seeedstudio.com/reTerminal-Sticky-p-6861.html)
- [Project support](https://github.com/limengdu/reTerminal_Sticky_Bunny/issues)
