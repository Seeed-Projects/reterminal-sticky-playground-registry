# Contributing a 3D printable design

This guide covers cases, stands, mounts, and accessories for the **3D Printables**
page of the Sticky Playground. It is written for makers who publish models on
Printables, MakerWorld, Thingiverse, GrabCAD, or GitHub. You do not need to
know anything about firmware.

For the Chinese guide, see [contributing-printables.zh-CN.md](contributing-printables.zh-CN.md).
Shared review and release steps live in [CONTRIBUTING.md](../CONTRIBUTING.md).

## What you submit

You submit one card, not the model files:

- a small `printable.json` file with the text shown on the card;
- one photo of the printed design fitted on a real reTerminal Sticky;
- a short README with print settings and assembly notes.

The model files stay on your own download page. The Sticky website shows your
card and sends visitors to that page with a **View on …** button.

```text
printables/
  my-case/
    printable.json
    README.md
    assets/
      preview.jpg
```

## What visitors see

1. The visitor opens Sticky Playground and selects **3D Printables**.
2. The visitor filters by category and opens your card.
3. The visitor selects **View on Printables** (or the platform you named).
4. The visitor downloads the files from your page.

## Step by step

### 1. Publish the model

Upload the files to Printables, MakerWorld, Thingiverse, GrabCAD, GitHub, or
your own site. Note the public page URL; it must start with `https://`.

### 2. Copy the template

From the repository root:

```bash
cp -R printables/_template printables/my-case
```

Use a short lowercase name joined by hyphens, for example `sticky-desk-stand`
or `wallet-case`. The directory name and the `id` inside `printable.json` must
be identical.

### 3. Fill in printable.json

```json
{
  "schemaVersion": 1,
  "id": "my-case",
  "name": "My Case",
  "category": "case",
  "summary": "A slim snap-on case that keeps the USB-C port open.",
  "description": "My Case wraps the edges of reTerminal Sticky and leaves the screen, side button, and USB-C port uncovered. Print it in PETG for a firm fit or TPU for a softer bumper.",
  "author": {
    "name": "Your name",
    "url": "https://www.printables.com/@your-profile"
  },
  "download": {
    "platform": "Printables",
    "url": "https://www.printables.com/model/123456-my-case",
    "license": "CC BY-SA 4.0"
  },
  "preview": {
    "image": "assets/preview.jpg",
    "alt": "My Case printed in black PETG fitted on reTerminal Sticky"
  },
  "tags": ["snap-on", "usb-c"]
}
```

| Field | What to write |
|---|---|
| `id` | Same as the directory name |
| `name` | Title shown on the card |
| `category` | One of the five categories below |
| `summary` | One sentence shown under the title (up to 140 characters) |
| `description` | Two or three sentences with fit, material, or usage notes (up to 800 characters) |
| `author.name` | Your name or team; shown on the card |
| `author.url` | Optional. Your profile page, opened when someone selects your name |
| `download.platform` | Site that hosts the files, for example `Printables`, `MakerWorld`, `Thingiverse`, `GrabCAD`, `GitHub` |
| `download.url` | Public page where the files are downloaded |
| `download.license` | Optional. License shown on that page, for example `CC BY-SA 4.0` |
| `preview.image` | Path to your photo inside `assets/`, for example `assets/preview.jpg` |
| `preview.alt` | One sentence describing the photo for screen readers |
| `tags` | Optional. Up to 6 short labels shown beside the category |

### 4. Pick the category

| Category | Use it for | Example |
|---|---|---|
| `case` | Shells and bumpers that wrap the device | wallet case, snap-on bumper |
| `stand` | Anything that holds Sticky upright or at an angle on a surface | desk stand, charging stand |
| `mount` | Parts that attach Sticky to something else | fridge magnet mount, wall bracket, bike clamp |
| `accessory` | Add-ons that are none of the above | stylus holder, cable clip, lanyard loop |
| `reference` | Models of Sticky itself, used to design other parts | enclosure CAD model, dimension template |

The category decides which filter shows your card on the website.

### 5. Add the photo

Save a real photo of the printed design fitted on reTerminal Sticky as
`assets/preview.jpg` (PNG and WebP also work). Landscape framing at roughly 4:3
looks best on the card. Keep the file under 5 MB.

### 6. Write the README

Replace the template README with:

- what the design is and which part of Sticky it protects, holds, or mounts;
- print settings: material, layer height, supports, orientation;
- assembly steps and how to remove the print again;
- a link to the same download page.

### 7. Check locally

Install Node.js 20 or newer, then run from the repository root:

```bash
npm test
npm run validate
```

`npm run validate` confirms that the directory name and `id` match, every
required field is present, the download URL uses HTTPS, the category is one of
the five values, and the photo exists in the right format. It prints
`Registry validation passed` when everything is in place.

### 8. Open the pull request

Select **3D printable design** in the pull request template and complete its
short checklist. A maintainer confirms the author credit, the download page,
and the photo, then merges the PR. The card appears on the Sticky website with
the next site release; see [CONTRIBUTING.md](../CONTRIBUTING.md#after-your-pull-request-is-merged).

## Updating a design

Edit the files in your directory and open a new pull request. Common updates:

- a new download URL after moving to another platform;
- a better photo;
- a changed category or description.

## Checklist

- [ ] The directory name and `printable.json` `id` are identical.
- [ ] `category` is one of `case`, `stand`, `mount`, `accessory`, `reference`.
- [ ] `download.url` is the public HTTPS page that hosts the files.
- [ ] `author.name` credits the designer; `download.license` matches the download page when a license is shown.
- [ ] `assets/preview.jpg` is a real photo of the print on reTerminal Sticky.
- [ ] The README lists print settings and assembly notes.
- [ ] `npm test` and `npm run validate` pass.
