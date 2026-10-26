# Pregnancy-Journal

## What it is

Pregnancy-Journal is offline pregnancy-tracking firmware for Seeed Studio
reTerminal Sticky. It provides a Baby Week dashboard, weight records, fetal
movement counting, and settings in English and Simplified Chinese. It requires
no account, Wi-Fi, cloud service, or subscription. It is for personal records
and does not replace professional medical advice.

## Changes in 2.0.1

- Open the existing dataset-creation confirmation directly from the shared
  no-dataset page in Baby Week, Weight, and Kicks. The shortcut appears only
  for a mounted, writable card that has no dataset. It never creates data
  without confirmation, and changing cards invalidates the pending action.
- Improve Chinese reading text, headings, buttons, spacing, and punctuation
  wrapping throughout the four apps, Launcher, and shared pages.
- Fix clipped storage messages, pregnancy-detail text crossing columns, and
  missing late-pregnancy stage translations.
- Keep the data format, partition layout, and explicit card-write confirmation
  behavior unchanged.

## Requirements and first-time setup

Use a reTerminal Sticky with ESP32-S3, 32 MB flash, and 8 MB PSRAM, plus a
MicroSD card formatted as FAT32. exFAT is not supported. User records live in
PREGJNL on the card; there is no internal-flash fallback.

1. Insert the FAT32 card and power on.
2. On a writable card with no dataset, select **SET UP MEMORY CARD** from Baby
   Week, Weight, or Kicks. Review the explanation and confirm creation. You can
   also use **Settings → MicroSD**. The firmware does not format the card or
   delete unrelated files.
3. In Baby Week, enter the due date or first day of the last menstrual period.
   Set the device clock if requested; it can also be edited in Settings.
4. Open Weight or Kicks from Launcher to record data.

## Controls

- Touch the dashboard illustration for Baby details and the week number for
  Mom details; use the menu button to open Launcher.
- Swipe up from the bottom edge to open Launcher.
- The top button navigates toward Baby Week home and Launcher; while counting
  kicks, a single press records one movement.
- Double-click the top button for the shortcut selected in Settings.
- Side buttons switch apps when the current page allows leaving; hold both
  for two seconds to request sleep. The top button wakes the device.
- Portrait and landscape have dedicated layouts.

Weight supports 366 records (one per date); Kicks supports 400 records. Back up
PREGJNL regularly. Long-term storage compaction is not implemented; exhausted
storage/index capacity rejects new writes rather than silently discarding data.

## Installation and upgrade behavior

The 2.0.1 package is a complete merged image written at **0x0**, with DIO,
80 MHz flash frequency, 32 MB flash size, and 460800 baud. The manifest includes
the exact file size and SHA-256. Use desktop Chrome or Edge with a USB data
cable for the Playground flasher.

Before installing, back up PREGJNL on the MicroSD card and export any legacy
records that still exist only in internal storage. The merged image includes
blank padding over the internal NVS area (0x9000–0xEFFF), so that area is
replaced even without a full-chip erase. Do not rely on internal settings or
legacy internal records surviving this package. The image does not contain
MicroSD data and does not itself write or format the card.

For an existing Pregnancy-Journal dataset, retain the card and check its records,
time, and language after boot. Dataset creation is only for cards without one.
Use the flasher erase option when replacing a different firmware. Version 2.0.0
and its original package remain available; backup is still recommended before
any downgrade.

## Source and package provenance

- Project and support: https://github.com/sky18Dragon/Pregnancy-Journal
- Source base: https://github.com/sky18Dragon/Pregnancy-Journal/tree/4c8f5fa15871c76441eb062a773dd9f236f1a270
- Version-only source delta: `firmware/2.0.1/source-version.patch`, changing
  CMake PROJECT_VER and STICKY_FIRMWARE_VERSION from 2.0.0 to 2.0.1.
- License: MIT; see LICENSE and NOTICE.md for retained attribution.
- Build: PlatformIO 6.1.19, espressif32 6.11.0, ESP-IDF 5.4.1,
  environment sticky-release. This is a local source build, not a flash dump;
  it contains no copied NVS, MicroSD records, or user configuration.
- Layout: bootloader at 0x0, partition table at 0x8000, application at 0x10000;
  merged with esptool using the build's flash arguments.
- Catalog preview: the existing real-device photograph from the 2.0.0
  contribution is retained. It shows the Baby Week dashboard, not a new 2.0.1
  acceptance run.

| Version | File | Offset | Size | SHA-256 |
| --- | --- | --- | --- | --- |
| 2.0.1 | `pregnancy-journal-2.0.1-merged.bin` | `0x0` | 5370928 | `8abae1dc16e284aef5bc78e07c1a62d498c24d921a394163892c0628a5c63d2e` |
| 2.0.0 (retained) | `pregnancy-journal-2.0.0-merged.bin` | `0x0` | 5368576 | `87dafb2da15ba03b6659a6a50a63525d3c7ed017bb9d326ed9cca90f02de8773` |

## Verification record

### Functional baseline tested on 2026-10-26

The behavior included in source base 4c8f5fa was flashed as sticky-release while
its embedded version still read 2.0.0. Flash hash verification succeeded.
The startup log confirmed MicroSD mount, saved-record loading, application
startup, and a completed full display refresh. The owner then reported that
testing was complete with no problems. The earlier first-use workflow was also
confirmed after replacing an exFAT card with a FAT32-formatted card.

Hardware was a physical Seeed reTerminal Sticky (ESP32-S3, 32 MB flash,
8 MB PSRAM), connected to macOS by USB. The board revision was not recorded
for this run. The earlier 2.0.0 acceptance record is available in
https://github.com/Seeed-Projects/reterminal-sticky-playground-registry/pull/92#issuecomment-6293048716
and applies to the original 2.0.0 package only.

### Exact 2.0.1 package

On 2026-10-27, the exact merged image listed above was installed **twice** on
physical reTerminal Sticky hardware. The SHA-256 was verified before each
installation, and esptool reported "Hash of data verified" both times.

- Hardware: ESP32-S3 revision v0.2, 32 MB flash, 8 MB PSRAM, SSD1677 display
  and GT911 touch; board revision not recorded.
- Method: esptool 4.9.0 over USB from macOS; complete image at 0x0,
  DIO / 80 MHz / 32 MB / 460800 baud; automatic RTS reset after each write.
- No full-chip erase or MicroSD format was performed. The merged image did
  overwrite internal NVS padding, as documented above.
- Both 25-second startup captures reported application 2.0.1 and ESP-IDF 5.4.1,
  successful MicroSD mounting, four-app initialization, boot ready, and full
  display refresh completion with ESP_OK. No panic was observed in these windows.
- First boot recovered 35 journal transactions and 7 live records. After manual
  use and the repeated installation, recovery reported 45 transactions,
  last transaction 45, 7 live records and 7 objects.
- The owner confirmed Baby Week, Weight, Kicks, Settings, portrait/landscape
  display, touch/hardware buttons, and existing records were normal on this
  2.0.1 package. They then physically unplugged/replugged USB, confirmed normal
  operation, and authorized the second installation.
- The second installation and reboot completed successfully; serial recovery
  loaded the existing card dataset. This is the repeated-install and saved-state
  boot evidence, rather than another claim of a full manual pass after the second write.

The existing `manifest=degraded slot=A commit=1` warning was present. On the
second boot, the first touch geometry read failed and the built-in retry
succeeded; touch then initialized without fallback. Browser
flashing, battery-only endurance, sleep/wake and abnormal-card recovery were not
part of this acceptance pass. The old 2.0.0 record and the earlier functional
baseline remain historical context only.

Static checks cover all manifest hashes, ESP32-S3 image headers and embedded
2.0.1 application version, partition capacity, merged-image contents at every
flash offset, and blank padding in place of user data.

Baseline validation: 78/78 host tests, all three firmware profiles, and 572
English/Chinese portrait/landscape previews passed before packaging. The 2.0.1 preparation also passed 78/78 host tests, all three firmware
builds, Registry tests (73/73), and Registry validation (33 firmware entries,
12 printable entries).

Known observations from the baseline run: the first touch-controller geometry
read required a successful retry; `manifest=degraded slot=A commit=1` remained
in the storage log. This update does not change either subsystem. Long-term
battery operation, sleep/wake, abnormal-card recovery, and endurance are not
covered by the short baseline observation.

Submitted by 孕期手帐.
