# Toybox

An e-paper playground for the reTerminal Sticky: an EPUB reader, notes you write
from your phone and pin to the screen, six games, and the everyday tools.

## What it does

- **Reader.** EPUBs off the card, with the book's own contents list, adjustable
  type, three line spacings and three screen rotations. Illustrations draw
  straight out of a book that has been through CrossInk's optimizer, with no
  conversion step. Reading positions are written in CrossPoint's format as well
  as its own, so a card carries its places between the two firmwares.
- **Fonts.** Typefaces are read from the card in CrossInk's `.cpfont` format --
  the device's own text and one face each for books, notes, flashcards and
  recipes, so a novel can be set in a serif without the menus following it. A
  book set in a card family is sized in that family's own cuts. Anything a font
  does not carry falls back to the built-in face, so choosing one can never
  empty a language.
- **Comics.** A `.tbk` page format prepared on a PC by
  [Toybox Slicer](https://github.com/z4b333/Toybox-slicer), which re-cuts a
  webtoon strip at blank gutters so a page never breaks through a face.
- **Notes.** The device serves a small editor to a phone over its own access
  point, or reads a Markdown file off the card. A pinned note stays on the panel
  with the power off -- as can a picture, a book's cover, or the page you were
  reading with a picture laid over it.
- **Games.** Wordle, Sudoku, Nonogram, 2048, Ships and XO. Boards and streaks
  are saved as you play.
- **Tools.** Coin, dice, timer and stopwatch, random number, card draw, a picker
  that chooses from a list, flashcards, and recipes read from `/recipes`.
- **Languages.** English, Thai, Chinese, Japanese, Korean and Vietnamese, with
  optional font packs for full CJK coverage installed separately from the
  project's own page.

## Controls

Touch throughout. Two side buttons page lists, turn book pages and step through
recipe steps; the OK button opens the options panel in the readers and the
recipe screens. Holding either side button at power-on reaches the service
screen.

## Package origin

Built from [sticky-toybox](https://github.com/z4b333/sticky-toybox) at
`0712bb6b4afd51224204ba2a5ae0f4fa749b5d3c`, tagged `v1.4.4`, dated 01 Sep 2026. The four binaries here are
the same files the project's own web installer serves at
<https://z4b333.github.io/sticky-toybox/> -- produced by `tools/make_image.sh`
and packaged for this registry by `tools/make_registry.py`, so the two installers
write identical bytes.

Application: 3983536 bytes, SHA-256 `62927c8c90548e32951b1db248f4f7c9ae845bb599b0e532f23472b15df39109`.

Built with PlatformIO and the Arduino ESP32 framework rather than ESP-IDF, which
is why this is a firmware-only contribution rather than a source build.

## Storage

A FAT32 microSD card holds books (`/books`), pictures (`/wallpapers`), recipes
(`/recipes`), and optionally notes (`/notes`, as `.md` or `.txt`) and flashcard
decks (`/decks`, as `.tsv`, `.csv` or `.txt`) to import. Notes, settings, saved
games and reading positions live in the device's own flash and need no card.

## Physical device test

Preview image: a photograph of the device.

Tested on reTerminal Sticky production hardware:

- [x] Power-on and first boot
- [x] Service screen: buses reported, orientation correct
- [x] Hub, drawers and every app opens
- [x] Touch and both side buttons
- [x] Reader: opens a book, turns pages, keeps its place across a reboot
- [x] Notes: written from a phone, pinned, still on the panel with power off
- [x] Reboot with saved state intact
