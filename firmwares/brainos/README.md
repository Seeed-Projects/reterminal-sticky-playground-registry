# BrainOS

A voice notebook for Seeed Studio reTerminal Sticky. Hold OK, say the idea, let go:
the board keeps the recording and turns it into text through
[OpenRouter](https://openrouter.ai/) when it has Wi-Fi.

- **Notes by voice**: hold OK for a note, hold Up for a chat note (the question, then
  the assistant's answer), hold Down to add to the open note.
- **Lock screen**: the weekday, the date and up to three pinned notes.
- **brain.local**: a page on your network, behind its own password, to read, edit,
  pin, tidy, archive and download the notes, add Wi-Fi networks and change settings.
- **QR export**: the notes as a loop of QR codes, read on a phone by the
  [reader page](https://carcaralivre.github.io/brainos/reader/).
- **Updates over Wi-Fi** from the project's GitHub releases, with rollback.

## What it needs

- A 2.4 GHz Wi-Fi network.
- Your own OpenRouter API key with a little credit. You pay OpenRouter directly; a
  short voice note usually costs a fraction of a US cent, and each note shows its cost.

Recordings and the text sent to the assistant go to OpenRouter. There is no BrainOS
server or account; the notes live on the board.

## First steps

1. On the board: Settings → Wi-Fi → Add a network.
2. On the board: Settings → About → Create page password. Within two minutes, open
   http://brain.local on a phone or computer on the same network and make a password.
3. Paste the OpenRouter key on the page and tap Save key.
4. Hold OK, speak, let go.

Full guide: https://github.com/carcaralivre/brainos#first-steps

## Package

- Origin: https://github.com/carcaralivre/brainos/releases/tag/v1.0.0
- Version: 1.0.0
- File: `brainos-full.bin`, merged image written at 0x0, 1794960 bytes,
  SHA-256 `125cb0c5e86afd3f32e05399978f7ade2b3c73c74517e7546d58054b3aa7fa3e`
- Tested on: reTerminal Sticky (ESP32-S3, 32 MB flash)
- License: MIT. Built on [Sticky OS](https://github.com/Sticky-OS/sticky-os) (MIT).
