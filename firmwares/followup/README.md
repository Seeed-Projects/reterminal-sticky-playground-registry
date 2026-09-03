# Followup

This community entry provides Followup 0.1.0 for browser-based installation on
reTerminal Sticky.

Followup is a voice-first thought-capture companion for ideas, to-dos, and
notes. It records a thought at the moment it occurs, uses Gemini to transcribe
and summarize the recording, saves the resulting data on a microSD card, and
keeps selected follow-ups visible on the E-Ink display.

## Requirements

- reTerminal Sticky
- USB data cable for installation
- microSD card for recordings, transcripts, and summaries
- Wi-Fi connection for Gemini features and time synchronization
- Gemini API key for transcription and summarization

## Firmware package

- Registry firmware version: `0.1.0`
- Source: <https://github.com/alxv2016/folloup-sticky>
- Author: [ALXV Labs](https://www.facebook.com/profile.php?id=61575196307817)
- License: GPL-3.0
- Target: ESP32-S3 with 32 MB flash
- Package: one merged binary written at offset `0x0`

The submitted binary is byte-for-byte identical to `merged-binary.bin` in the
upstream `releases/v0.1.0_sticky.zip` archive added at commit
`be7f0d99c36f9f31f88b527aa9279d15a484ddd7`. The embedded bootloader reports
ESP-IDF `v5.5.4-dirty`, DIO flash mode, and an 80 MHz flash frequency.

## Installation

Open Followup from the reTerminal Sticky Playground, connect the device with a
USB data cable, select version 0.1.0, and start the browser installation. The
firmware manifest writes the complete merged image from flash offset `0x0`.

The tested command-line equivalent is:

```bash
esptool.py \
  --chip esp32s3 \
  --baud 460800 \
  write_flash \
  --flash_size 32MB \
  --verify \
  0x0 releases/merged-binary.bin
```

## Physical-device test record

Followup 0.1.0 was installed on reTerminal Sticky production hardware using the
merged binary and command shown above. The verified write completed
successfully, and the running notes interface is shown in the submitted
real-device preview photo.

| Item | Result |
|---|---|
| Hardware | reTerminal Sticky production hardware |
| Firmware version | `0.1.0` |
| Flash offset | `0x0` |
| Flash size | 32 MB |
| Verified write | Passed |
| Followup interface on device | Passed |

Project source and documentation are available from the
[Followup repository](https://github.com/alxv2016/folloup-sticky). Support is
provided through the repository's
[issue tracker](https://github.com/alxv2016/folloup-sticky/issues).
