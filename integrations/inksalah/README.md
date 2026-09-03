# InkSalah

InkSalah turns reTerminal Sticky into an offline-first Salah companion with a
persistent ePaper prayer-window display and native touch settings.

## What it does

- **Prayer windows.** Shows the active prayer window, its end, remaining time,
  and the next obligatory prayer.
- **Local calculation.** Calculates Fajr, Sunrise, Dhuhr, Asr, Maghrib, Isha,
  and optional Qiyam on the ESP32-S3 without downloading prayer timetables.
- **Prayer configuration.** Provides recognized calculation profiles, Standard
  and Hanafi Asr rules, per-time minute adjustments, an optional reminder, and
  optional Qiyam display.
- **Native interaction.** Uses the Sticky touchscreen for settings, the IMU for
  portrait and landscape layouts, and RTC scheduling for low-power operation.
- **Private setup.** Stores Wi-Fi credentials and prayer settings locally in
  ESP32 NVS storage.

## Requirements

- reTerminal Sticky
- USB-C data cable for browser installation
- A 2.4 GHz Wi-Fi network for first-time clock synchronization and optional
  approximate location detection
- A phone or computer for the first-time captive Wi-Fi page

Prayer times are calculated locally from the saved coordinates, civil date,
fixed UTC offset, calculation method, and Asr school. Wi-Fi is not used to
download a prayer timetable.

## Firmware package

- Registry firmware version: `0.1.0`
- Source: <https://github.com/limengdu/reTerminal_Sticky_InkSalah>
- Source commit: `e59cec9e63d4cc9a4a6a33f1909b02e0f2a88e16`
- License: MIT
- Build environment: `sticky-salah-release`
- Platform: Espressif32 `6.11.0`
- Framework: ESP-IDF `5.4.1`
- Target: ESP32-S3 with 32 MB flash

The package was built from the clean source commit above with PlatformIO. The
Registry contains the exact release build's bootloader, partition table, and
application image at offsets `0x0000`, `0x8000`, and `0x10000`. GitHub Actions
run `33602137769` also passed the host tests and both firmware build profiles
for the same source commit.

## Installation and first boot

1. Open InkSalah from the reTerminal Sticky Playground in desktop Chrome or
   Edge.
2. Connect the device with a USB-C data cable and select its serial port.
3. Start the installation and accept the erase prompt for a clean first boot.
4. Connect a phone to the `Sticky-Salah-XXXX` setup network using the password
   `sticky-salah`.
5. Select a scanned 2.4 GHz Wi-Fi network or type a hidden SSID, then choose
   **Check and save**.
6. On the Sticky screen, confirm the detected location and UTC offset, select a
   calculation method and Asr school, and choose **Save & Start**.

The network-based location is an estimate. Confirm its coordinates and UTC
offset before saving. For congregational prayer and local religious guidance,
follow the timetable and iqamah schedule published by a trusted local mosque or
authority.

## Daily controls

- Swipe up from the bottom of the prayer screen to open settings.
- Swipe down from the top of settings to validate, save, and return.
- Swipe right from the left edge inside a settings subpage to return one level
  while keeping the draft.
- Rotate the device to switch between portrait and landscape layouts.
- Leave the device untouched for three minutes to enter deep sleep.

## Physical-device test record

The exact three files in `firmware/0.1.0/` were written to reTerminal Sticky
production hardware at the manifest offsets with esptool. Each write completed
with its data hash verified. A cold restart then reported InkSalah `0.1.0`,
connected to Wi-Fi, synchronized the RTC, restored the saved Shenzhen settings,
rendered the prayer page, and started touch and IMU monitoring. The submitted
preview is a real-device photo of the portrait prayer-window interface.

| Item | Result |
| --- | --- |
| Hardware | reTerminal Sticky production hardware |
| Firmware version | `0.1.0` |
| Exact Registry package write and hash verification | Passed |
| Wi-Fi setup and clock synchronization | Passed |
| Local prayer calculation and seven-time timeline | Passed |
| Native touch settings and saved configuration | Passed |
| Portrait and landscape rotation | Passed |
| Three-minute idle sleep and wake paths | Passed |
| Reboot with saved settings restored | Passed |
| USB reconnection and repeated installation | Passed |

## Links

- [Project source and documentation](https://github.com/limengdu/reTerminal_Sticky_InkSalah)
- [Prayer calculation reference](https://github.com/limengdu/reTerminal_Sticky_InkSalah/blob/main/docs/PRAYER_CALCULATION.md)
- [Sticky official website](https://www.seeedstudio.com/sticky/)
- [reTerminal Sticky product page](https://www.seeedstudio.com/reTerminal-Sticky-p-6861.html)
- [Project support](https://github.com/limengdu/reTerminal_Sticky_InkSalah/issues)
