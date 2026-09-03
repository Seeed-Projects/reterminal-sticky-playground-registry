# Contributing a 3D printable design

This guide covers cases, stands, mounts, and accessories for the
[**3D Printables**](https://www.seeedstudio.com/sticky/playground/3d-printables/)
page of the Sticky Playground. It is written for makers who publish models on
Printables, MakerWorld, Thingiverse, GrabCAD, or GitHub. You do not need to
know anything about firmware or ESP-IDF.

For the Chinese guide, see [contributing-printables.zh-CN.md](contributing-printables.zh-CN.md).
Rules shared with firmware contributions, the three ways to open a pull
request, and the release flow are in [CONTRIBUTING.md](../CONTRIBUTING.md).

## Contents

1. [What you submit](#1-what-you-submit)
2. [What visitors see](#2-what-visitors-see)
3. [Files in your directory](#3-files-in-your-directory)
4. [printable.json field reference](#4-printablejson-field-reference)
5. [Categories](#5-categories)
6. [The preview photo](#6-the-preview-photo)
7. [The README](#7-the-readme)
8. [Step by step](#8-step-by-step)
9. [Validation errors and how to fix them](#9-validation-errors-and-how-to-fix-them)
10. [Updating a design](#10-updating-a-design)
11. [Worked example](#11-worked-example)
12. [Checklist](#12-checklist)

## 1. What you submit

You submit one **card**, not the model files:

| File | Purpose | Required |
|---|---|---|
| `printables/<design-id>/printable.json` | The text and links shown on the card | Yes |
| `printables/<design-id>/assets/preview.jpg` | Photo of the printed design fitted on reTerminal Sticky | Yes |
| `printables/<design-id>/README.md` | Print settings, assembly notes, and the download link in prose | Yes |

The STL / 3MF / STEP files stay on your own download page. The Sticky website
shows your card and sends visitors to that page with a **View on …** button.
This keeps the files under your control: you can update them, add remixes, or
change the license without a new pull request here.

## 2. What visitors see

On the 3D Printables page every design is one card in a grid, with a category
filter on the left. A card shows, from top to bottom:

1. your `preview.image` photo (cropped to about 4:3);
2. the category pill (for example **Cases**) and any `tags`;
3. `name` as the title;
4. `summary` as one or two lines under the title;
5. an **Author** row showing `author.name`, linked to `author.url` if you gave one;
6. a full-width **View on `<download.platform>`** button that opens `download.url` in a new tab.

`description` is stored for the card's detail view and for search; it is not
shown in the grid today, so put the essential message in `summary`.

## 3. Files in your directory

```text
printables/
  my-case/                 <- directory name = id
    printable.json         <- card metadata (this guide, section 4)
    README.md              <- print settings and assembly notes (section 7)
    assets/
      preview.jpg          <- photo referenced by preview.image (section 6)
```

Rules for the directory name (`<design-id>`):

- lowercase letters, digits, and single hyphens only; 2–64 characters;
- must match the regular expression `^[a-z0-9]+(?:-[a-z0-9]+)*$`;
- describes the design, not you: `wallet-case`, `sticky-desk-stand`,
  `fridge-magnet-mount`;
- must be identical to the `id` field inside `printable.json`.

Anything else in the directory (extra photos, a `LICENSE` file) is allowed but
ignored by the website. Do not add model files; pull requests that commit
STL/3MF/STEP files will be asked to remove them.

## 4. printable.json field reference

`printable.json` is a strict JSON object. Every key below is either required or
optional; **any other key makes validation fail**. The formal definition is
[`schemas/printable.schema.json`](../schemas/printable.schema.json).

### Top level

| Field | Type | Required | Limits | What to write |
|---|---|---|---|---|
| `schemaVersion` | integer | Yes | must be `1` | Always `1` |
| `id` | string | Yes | 2–64 chars, `^[a-z0-9]+(?:-[a-z0-9]+)*$` | Same as the directory name |
| `name` | string | Yes | 1–80 chars | Card title, for example `Sticky Wallet Case` |
| `category` | string | Yes | one of `case`, `stand`, `mount`, `accessory`, `reference` | Which filter shows the card; see [Section 5](#5-categories) |
| `summary` | string | Yes | 1–140 chars | One sentence under the title. Say what it is and the one thing that makes it useful |
| `description` | string | Yes | 1–800 chars | Two or three sentences: how it fits Sticky, recommended material, anything to know before printing |
| `author` | object | Yes | see below | Who designed it |
| `download` | object | Yes | see below | Where the files live |
| `preview` | object | Yes | see below | The photo on the card |
| `tags` | array of strings | No | up to 6 items, each 1–32 chars, no duplicates | Short labels shown beside the category, for example `snap-on`, `usb-c`, `tpu` |

### `author`

| Field | Type | Required | Limits | What to write |
|---|---|---|---|---|
| `author.name` | string | Yes | 1–80 chars | Person or team name shown on the card |
| `author.url` | string | No | HTTPS URL, up to 2048 chars | Profile page opened when the name is selected: your Printables/MakerWorld profile, GitHub, or personal site |

### `download`

| Field | Type | Required | Limits | What to write |
|---|---|---|---|---|
| `download.platform` | string | Yes | 1–40 chars | Site that hosts the files. Becomes the button text **View on `<platform>`**. Use the site's own spelling: `Printables`, `MakerWorld`, `Thingiverse`, `GrabCAD`, `GitHub`, `Cults3D`, or your site name |
| `download.url` | string | Yes | HTTPS URL, up to 2048 chars | The public page (or direct file link) where visitors download the model |
| `download.license` | string | No | 1–80 chars | License shown on your download page, written the way the platform shows it: `CC BY 4.0`, `CC BY-SA 4.0`, `CC BY-NC 4.0`, `MIT`, `GPL-3.0` |

### `preview`

| Field | Type | Required | Limits | What to write |
|---|---|---|---|---|
| `preview.image` | string | Yes | must match `^assets/[A-Za-z0-9._-]+\.(png\|jpg\|jpeg\|webp)$`; file ≤ 5 MB | Path to the photo inside your directory, normally `assets/preview.jpg` |
| `preview.alt` | string | Yes | 1–180 chars | One sentence describing the photo for screen readers and image search, for example `Black PETG wallet case fitted on reTerminal Sticky, front view` |

### Complete example

```json
{
  "schemaVersion": 1,
  "id": "my-case",
  "name": "My Case",
  "category": "case",
  "summary": "A slim snap-on case that keeps the USB-C port and side button open.",
  "description": "My Case wraps the edges of reTerminal Sticky and leaves the screen, side button, and USB-C port uncovered. Print it in PETG for a firm fit or TPU for a softer bumper. The lip is 1.2 mm thick and clears the e-paper bezel.",
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
  "tags": ["snap-on", "usb-c", "petg"]
}
```

Writing tips:

- `name`: title case, no trailing punctuation, no "for reTerminal Sticky" (every card is for Sticky).
- `summary`: complete sentence with a period, no marketing adjectives.
- `description`: mention fit, material, and any hardware the print needs (magnets, screws, felt pads).
- Keep all text in English; the website serves a global audience.

## 5. Categories

`category` decides which filter on the left of the page shows your card. Pick
the one that describes the primary purpose.

| Value | Filter label | Use it for | Examples |
|---|---|---|---|
| `case` | Cases | Shells and bumpers that wrap the device itself | wallet case, snap-on bumper, full enclosure with lid |
| `stand` | Stands | Anything that holds Sticky upright or at an angle on a surface | desk stand, charging stand, easel |
| `mount` | Mounts | Parts that attach Sticky to something else | fridge magnet mount, wall bracket, monitor clip, bike clamp |
| `accessory` | Accessories | Add-ons that are none of the above | stylus holder, cable clip, lanyard loop, travel box |
| `reference` | Reference Models | Models of Sticky itself, used to design other parts | enclosure CAD model, dimension template, test-fit jig |

If a design does two things (a case that is also a stand), choose the one a
visitor would search for first and mention the other in `tags`.

## 6. The preview photo

The photo is the first thing visitors see, and the only way they can judge fit
before opening your page.

| Requirement | Details |
|---|---|
| Real photo | The printed part fitted on a real reTerminal Sticky. Renders, slicer screenshots, and photos without the device are not accepted. |
| Framing | Landscape, roughly 4:3. The card crops to that ratio, so keep the subject centred. |
| Format | JPG, PNG, or WebP. JPG is usually smallest. |
| Size | Up to 5 MB. 1200–1600 px on the long edge is plenty. |
| Path | Inside `assets/`, referenced exactly in `preview.image`. |
| Content | No watermarks, no text overlays, no other products. |

Good practice: natural light, plain background, the e-paper screen switched on
and readable, one angle that shows how the print meets the device.

## 7. The README

The README is for people who already decided to print your design. Replace the
template with these sections:

```markdown
# My Case

One paragraph: what it is, which part of Sticky it protects, holds, or mounts,
and who it is for.

## Print settings

- Material: PETG (firm) or TPU 95A (soft)
- Layer height: 0.2 mm
- Walls: 3
- Infill: 15 %
- Supports: none
- Orientation: back face on the bed

## Assembly

1. Remove any brim.
2. Slide the case on from the USB-C side first.
3. Press the top edge until the lip clicks over the bezel.
4. To remove, push the bottom corner out with a thumb.

## Hardware

None. (Or: 2 × 6 mm neodymium magnets, glued into the recesses.)

## Files

<https://www.printables.com/model/123456-my-case> — same page as `download.url`.

## License

CC BY-SA 4.0
```

Keep it short; anything longer belongs on your download page.

## 8. Step by step

### Step 1 — Publish the model

Upload the files to Printables, MakerWorld, Thingiverse, GrabCAD, GitHub, or
your own site. Copy the public page URL. It must start with `https://`, and
opening it in a private browser window must show the files without logging in.

### Step 2 — Take the photo

Print the design, fit it on your Sticky, and take the photo described in
[Section 6](#6-the-preview-photo). Save it as `preview.jpg`.

### Step 3 — Get a copy of the repository

Choose one of the three methods in
[CONTRIBUTING.md → Three ways to submit](../CONTRIBUTING.md#5-three-ways-to-submit):

- **Method A** (git): fork, clone, create a branch.
- **Method B** (web): fork, then create files directly in the GitHub interface.

The rest of this section assumes Method A; the file contents are identical for
Method B.

### Step 4 — Copy the template

From the repository root:

```bash
cp -R printables/_template printables/my-case
```

You now have `printables/my-case/printable.json`, `README.md`, and
`assets/README.md`.

### Step 5 — Fill in printable.json

Open `printables/my-case/printable.json` and replace every value, following
[Section 4](#4-printablejson-field-reference). Change `id` to `my-case`.

### Step 6 — Add the photo

Copy your photo to `printables/my-case/assets/preview.jpg` and delete
`assets/README.md` (it only explained what to put there). Make sure
`preview.image` in `printable.json` is `assets/preview.jpg`.

### Step 7 — Write the README

Replace `printables/my-case/README.md` with the outline from
[Section 7](#7-the-readme).

### Step 8 — Check locally

Install Node.js 20 or newer, then run from the repository root:

```bash
npm test
npm run validate
```

Expected last line:

```text
Registry validation passed (12 firmware(s), 6 printable(s)).
```

(The numbers count every directory in the repository, so the printable count
goes up by one with your design.) If it fails, see
[Section 9](#9-validation-errors-and-how-to-fix-them).

### Step 9 — Commit and push

```bash
git add printables/my-case
git commit -m "feat: add My Case printable"
git push -u origin add-my-case
```

### Step 10 — Open the pull request

On GitHub, open the pull request from your branch to
`Seeed-Projects/reterminal-sticky-playground-registry` / `main`. In the
template:

- under **Contribution type**, tick **3D printable design: new design or update**;
- complete **Common verification**;
- complete the **3D printable design** section: design name, directory,
  download page, platform, category, and the five checkboxes;
- leave the **Firmware** section untouched.

Title suggestion: `Add <Design Name> printable`.

### Step 11 — Review

GitHub Actions runs `npm test` and `npm run validate` within a couple of
minutes. A maintainer then checks the author credit, the download page, and the
photo. Reply to comments on the PR or push fixes to the same branch. After
merge, the card appears with the next Sticky website release; see
[CONTRIBUTING.md → After your pull request is merged](../CONTRIBUTING.md#9-after-your-pull-request-is-merged).

## 9. Validation errors and how to fix them

Every error names the file, then the field path, then the problem. Examples
for `printables/my-case/printable.json`:

| Error text | Cause | Fix |
|---|---|---|
| `printables/my-case: is missing printable.json` | File not created, or named differently | Create `printables/my-case/printable.json` |
| `...printable.json: contains invalid JSON (...)` | Missing comma, trailing comma, or unquoted key | Paste the file into a JSON checker; fix the reported line |
| `...printable.json: missing required field "download"` | A required top-level object is absent | Add the object from [Section 4](#4-printablejson-field-reference) |
| `...printable.json: contains unsupported field "mode"` | A key that is not in the field reference | Remove it (firmware-only fields such as `mode`, `catalogSection`, `source`, `external` do not belong here) |
| `...printable.json.id: must match the directory name "my-case"` | `id` differs from the directory | Make them identical |
| `...printable.json.category: must be one of: case, stand, mount, accessory, reference` | Typo or a firmware category | Use one of the five values |
| `...printable.json.summary: must contain between 1 and 140 characters` | Too long | Shorten to one sentence; move detail to `description` |
| `...printable.json.download.url: must use HTTPS` | URL starts with `http://` | Use the `https://` version of the page |
| `...printable.json.download.url: must be a valid absolute URL` | Missing scheme or spaces | Copy the URL from the browser address bar |
| `...printable.json.preview.image: must be inside the assets/ directory` | Path does not start with `assets/` | Move the file and write `assets/preview.jpg` |
| `...printable.json.preview.image: references a missing file: assets/preview.jpg` | File name or extension differs, or the file was not added to git | Check spelling and case; run `git add printables/my-case/assets` |
| `...printable.json.preview.image: must use one of these file extensions: .png, .jpg, .jpeg, .webp` | HEIC, SVG, GIF, or other format | Export as JPG or PNG |
| `...printable.json.preview.image: does not contain a valid JPEG file signature` | A PNG (or other file) renamed to `.jpg` | Re-export in the right format, or rename to the real extension and update `preview.image` |
| `...printable.json.preview.image: must not exceed 5 MB` | Photo too large | Resize to about 1600 px on the long edge |
| `...printable.json.tags: must be an array containing no more than 6 tags` | Too many tags | Keep the six most useful |
| `...printable.json.tags[2]: duplicates tag "case"` | Same tag twice | Remove the duplicate |
| `...printable.json.README.md: references a missing file: README.md` | README deleted or not committed | Add `printables/my-case/README.md` |

## 10. Updating a design

Edit the files in your directory and open a new pull request (Method A, B, or
C in CONTRIBUTING.md). Tick **3D printable design: new design or update** in
the template and say what changed. Typical updates:

| Change | What to edit |
|---|---|
| Moved to another platform | `download.platform`, `download.url`, README link |
| Better photo | Replace `assets/preview.jpg`; update `preview.alt` if the view changed |
| New revision of the model | `description` (mention the revision), README print settings |
| Different license | `download.license`, README |
| Re-categorise | `category` |

The directory name and `id` stay the same so the card keeps its identity.

## 11. Worked example

The Sticky Wallet Case by Dropthetenors is published on Thingiverse. Its card
in this repository is:

`printables/sticky-wallet-case/printable.json`

```json
{
  "schemaVersion": 1,
  "id": "sticky-wallet-case",
  "name": "Sticky Wallet Case",
  "category": "case",
  "summary": "A slim printed wallet-style case designed around the Sticky card form factor.",
  "description": "A slim wallet-style case that wraps around reTerminal Sticky so it can be carried like a card. Files are published on Thingiverse.",
  "author": {
    "name": "Dropthetenors",
    "url": "https://www.thingiverse.com/thing:7396154"
  },
  "download": {
    "platform": "Thingiverse",
    "url": "https://www.thingiverse.com/thing:7396154/files"
  },
  "preview": {
    "image": "assets/preview.jpg",
    "alt": "Sticky Wallet Case printed and fitted on reTerminal Sticky"
  },
  "tags": [
    "case"
  ]
}
```

On the website this renders as a card under **Cases** with the title
"Sticky Wallet Case", the author "Dropthetenors" linked to the Thingiverse page,
and a **View on Thingiverse** button that opens the files tab. Browse the other
directories under `printables/` for more real examples.

## 12. Checklist

- [ ] The directory name and `printable.json` `id` are identical and use only lowercase letters, digits, and hyphens.
- [ ] `category` is one of `case`, `stand`, `mount`, `accessory`, `reference`.
- [ ] `summary` is one sentence (≤ 140 characters); `description` is two or three sentences (≤ 800).
- [ ] `author.name` credits the designer; `author.url` (if given) is HTTPS.
- [ ] `download.platform` is the hosting site's name; `download.url` is the public HTTPS page; `download.license` matches the page when a license is shown.
- [ ] `assets/preview.jpg` is a real photo of the print on reTerminal Sticky, ≤ 5 MB, referenced exactly in `preview.image`, with a descriptive `preview.alt`.
- [ ] `README.md` lists print settings, assembly steps, hardware, files link, and license.
- [ ] No model files are committed.
- [ ] `npm test` and `npm run validate` pass.
- [ ] The PR template has **3D printable design** ticked and its section completed.
