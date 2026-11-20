# Bus ETA

Bus ETA turns reTerminal Sticky into a live bus arrival display. It supports Singapore and Hong Kong bus data, on-device station setup, WiFi setup, and automatic low-power sleep.

## Setup

After flashing, open the WiFi page to connect the device to a network. In Setup, select the city/provider and enter a station name or station ID, plus a required refresh interval (whole number, 1–60 minutes). A fresh flash shows no preset stop name; the Bus page prompts you to set a station until one is saved, then shows the station name, local time and available arrivals.

The firmware uses WiFi to retrieve public bus arrival data and NTP time. It writes synchronized time to the onboard RTC when available. After one hour without a hardware button press, it displays a simple sleep screen and enters low-power sleep. Press the refresh/AI button to wake and refresh the Bus page.

## Firmware package

- Version: `1.1.0`
- Source project: [Clair2929/reterminal-sticky-bus-eta](https://github.com/Clair2929/reterminal-sticky-bus-eta)
- Build toolchain: ESP-IDF v5.4, target ESP32-S3
- Package: the bootloader, partition table and application image are listed with their flash offsets and SHA-256 hashes in `firmware/1.1.0/manifest.json`.

### What changed in 1.1.0

- Fix Singapore stop-name lookup ("Station ID lookup failed") by downloading the stop directory from stable CDN mirrors (jsDelivr → GitHub raw → data.busrouter.sg) over TLS 1.2, with retry/backoff and full-content-length verification.
- Auto-detect the e-paper controller (SSD1677 / SSD2677), so the same firmware runs on both the original and SSD2677-equipped devices.
- Remove the hard-coded default stop name on fresh flashes.
- Gate the WiFi AP scan to the WiFi Config page so it no longer interrupts downloads.
- Make the refresh interval required (1–60 minutes) in Setup.

## Hardware test

Tested on reTerminal Sticky devices with ESP32-S3, including both the original SSD1677 e-paper panel and the SSD2677 variant. The device booted, connected to WiFi, synchronized NTP time, read the RTC and battery gauge, and displayed live bus arrivals. The preview photo shows the firmware running on the device.
