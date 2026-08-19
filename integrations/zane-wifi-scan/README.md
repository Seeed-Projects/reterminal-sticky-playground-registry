# ZANE Wi-Fi Tools for Seeed reTerminal Sticky

ZANE Wi-Fi Tools is a comprehensive, open-source Wi-Fi analysis and packet monitoring firmware built for the Seeed reTerminal Sticky (ESP32-S3 with a 3.97" SSD1677 ePaper display). 

It turns the device into a standalone, low-power handheld Wi-Fi diagnostic utility featuring 7 dedicated analysis modes, hardware-level battery fuel gauge integration, power management with deep sleep support, and optimized ePaper rendering.

---

## 📸 Key Features

1. 📡 Wi-Fi Scanner  
   Scans local 802.11 networks with real-time RSSI signal strength bars, BSSID, channel, encryption standard (WPA2/WPA3/WEP/OPEN), hidden SSID detection, OUI-based vendor lookup, and first/last seen timestamps. Supports sorting by RSSI or channel.

2. 📊 Channel Analyzer  
   Generates a landscape-oriented 2.4 GHz spectrum chart showing AP density per channel (1–13) to quickly identify network congestion.

3. 🔍 AP Inspector  
   Performs deep inspection of 802.11 Beacon frames for a selected Access Point. Extracts capability flags, beacon intervals, country codes, DS channel, QoS/WMM support, security protocols (RSN/WPA), and PHY modes (802.11b/g/n/ac/ax).

4. 📦 Packet Monitor  
   Sets the ESP32-S3 Wi-Fi radio to promiscuous mode. Displays real-time frame type statistics (Management, Control, Data, Beacons, Deauths, Probe Requests/Responses) alongside a rolling ring-buffer log of recent frames. Supports automatic channel hopping or fixed channel lock.

5. 📱 Client Discovery  
   Sniffs traffic on a specific AP's channel to discover connected client devices by filtering non-AP MAC addresses. Tracks frame counts, RSSI, vendor OUI, and activity timestamps.

6. 🕵️ Probe Analyzer  
   Monitors background 802.11 Probe Requests from nearby client devices across all channels. Differentiates between broadcast probes and directed probes (revealing SSIDs requested by searching devices) and measures overall probe rates (probes/sec).

7. 📈 RSSI Meter  
   High-speed signal strength meter for a target AP. Features an analog-style RSSI bar, live stats (Min, Max, Avg), a historical trend graph (10s / 1min window), and optimized flicker-free ePaper partial refresh for fast visual feedback.

---

## 🛠️ Hardware Requirements & Pinout

Designed specifically for the Seeed reTerminal Sticky:

- MCU: ESP32-S3 (Dual-Core, OPI PSRAM enabled)
- ePaper Display: SSD1677 (3.97", 800x480) via SPI (SCK: 13, MOSI: 14, MISO: 12, CS: 15, DC: 16, RST: 17, BUSY: 18, PWR_EN: 47)
- Fuel Gauge: BQ27220 via I2C (SDA: 1, SCL: 0) at address 0x55
- Buttons: UP (GPIO 5), DOWN (GPIO 6), OK (GPIO 4) — Active-Low with internal pull-ups
- Buzzer: GPIO 48 — LEDC PWM driven (2.5 kHz tone)
- Power Lock: PWR_HOLD: 45, PWR_LOCK: 46, CHG_EN: 39 — System power hold & deep sleep RTC hold

---

## 🕹️ Controls & Navigation

The interface is navigated entirely using the 3 physical buttons:

- UP / DOWN: Navigate menu items, scroll pages, change target AP, or adjust channel.
- OK: Select item, initiate scan/measure, or trigger frame capture.
- Hold OK (Long Press > 0.9s): Return to main menu (or back to picker view from detail screens).
- UP + DOWN (Chord Press): Toggle sorting mode (Scanner), toggle Channel Hopping (Monitor), or switch history window (RSSI Meter).

---

## 🔋 Power Management

- Power Hold: Uses PWR_HOLD and PWR_LOCK pins to maintain system power after soft switch activation.
- Auto-Sleep: Automatically enters Deep Sleep after 60 seconds of inactivity to conserve battery.
- Wakeup: Pressing any button (UP, DOWN, or OK) wakes the device from Deep Sleep via EXT1 RTC wake-up source.
- Fuel Gauge: Reads real-time battery voltage and State-of-Charge (%) directly from the BQ27220 IC over I2C, with automatic voltage-curve fallback estimation.

---

## ⚙️ Building & Flashing

### Prerequisites
- Arduino IDE (or VS Code + ESP-IDF / PlatformIO with Arduino component)
- Arduino ESP32 Board Package: Version 3.x or newer (required for ledcAttach API)
- Required Libraries:
  - Adafruit_GFX
  - SPI
  - Wire
  - WiFi

### Recommended Board Settings (Arduino IDE)
- Board: ESP32S3 Dev Module
- USB CDC On Boot: Disabled
- PSRAM: OPI PSRAM
- Flash Size: 8MB or 16MB (depending on module variant)
- Partition Scheme: Default or any scheme with > 1.5MB app space

---

## 📄 License

Developed by ZANE systems Kft. Released under the MIT License. Feel free to modify, contribute, and adapt for your own projects.
