# Sticky Lotus

> A lightweight Magic: The Gathering life counter built for the Seeed Studio reTerminalSticky.

<p align="center">
  <img src="assets/images/sticky-lotus.jpeg" alt="Sticky Lotus" width="500">
</p>

<p align="center">
  A fast, touch-first life counter for Magic: The Gathering — designed specifically
  for the E-Ink display of the Seeed Studio Sticky.
</p>

---

## About Sticky Lotus

**Sticky Lotus** turns the Seeed Studio Sticky into a dedicated tabletop companion
for Magic: The Gathering.

The project focuses on the things you actually need during a game:

- Life tracking for 2 or 4 players
- Commander Damage
- Poison Counters
- Touch gestures
- Long-press ±10 life changes
- Configurable starting life
- Battery status
- Persistent game state
- Deep Sleep support
- E-Ink optimized partial refreshes

The goal is a device that feels less like an embedded development board
and more like a purpose-built Magic accessory.

---

## Features

### Life Counter

Sticky Lotus supports both:

**2 Player** and  **4 Player / Commander**

Each player receives an independent life counter with touch controls.

Tap the `+` or `−` area to change life by one.

Hold the area to change life by ten.

### Commander Damage

**Swipe left or right** from a player area to open the Commander Damage interface.

Commander damage can be edited independently for each attacking player
and is applied when leaving the Commander Damage screen.

### Poison Counters

Poison counters are available directly through a **swipe up** gesture from
the corresponding player area and **swipe down** to close.

### Touch optimized

The interface is designed around the physical orientation of players
around a table.

Player areas facing the opposite side are rotated automatically so that
every player can read and operate their own controls.

### E-Ink optimized rendering

Instead of refreshing the entire screen after every input, Sticky Lotus
updates only the affected regions whenever possible.

This significantly improves perceived responsiveness and reduces
unnecessary E-Ink refreshes.

---

## Deep Sleep

Pressing the hardware power button puts Sticky Lotus into Deep Sleep.

Before sleeping:

1. The current game state is stored in NVS.
2. The Deep Sleep artwork is rendered.
3. The E-Ink display performs its final refresh.
4. The ESP32 enters Deep Sleep.

The E-Ink panel retains the sleep artwork without requiring continuous power.

Press the power button again to wake the device and restore the previous game.


# Firmware 

- Registry firmware version: `1.0`
- Source: https://github.com/inkOne/sticky-lotus
- License: MIT
