# Contributing to reTerminal Sticky Playground

This repository is the public contribution and review layer for the
[reTerminal Sticky Playground](https://www.seeedstudio.com/sticky/playground/).
Everything a visitor sees on the Playground **Firmware** and **3D Printables**
pages comes from the files in this repository. You add or change a directory
here, open a pull request, a maintainer reviews it, and the Sticky website picks
up the change in its next release.

For the Chinese version, see [CONTRIBUTING.zh-CN.md](CONTRIBUTING.zh-CN.md).

## Contents

1. [Choose your guide](#1-choose-your-guide)
2. [What you need](#2-what-you-need)
3. [Repository layout](#3-repository-layout)
4. [How a contribution reaches the website](#4-how-a-contribution-reaches-the-website)
5. [Three ways to submit](#5-three-ways-to-submit)
6. [Shared rules for every contribution](#6-shared-rules-for-every-contribution)
7. [Local checks](#7-local-checks)
8. [Pull request review](#8-pull-request-review)
9. [After your pull request is merged](#9-after-your-pull-request-is-merged)
10. [Updating or removing a contribution](#10-updating-or-removing-a-contribution)
11. [FAQ](#11-faq)

## 1. Choose your guide

There are two kinds of contributions. Each has its own directory, metadata
file, template, and detailed guide.

| I want to share… | Directory | Metadata file | Template to copy | Detailed guide |
|---|---|---|---|---|
| Firmware that users flash from the Sticky website | `firmwares/<firmware-id>/` | `firmware.json` | `firmwares/_template/` | [Contributing firmware](docs/contributing-firmware.md) |
| A 3D printable case, stand, mount, or accessory | `printables/<design-id>/` | `printable.json` | `printables/_template/` | [Contributing a 3D printable design](docs/contributing-printables.md) |

- A **firmware** contribution ships either buildable ESP-IDF, PlatformIO, or
  Arduino source that GitHub Actions compiles, or a verified `.bin` package with
  a manifest. It needs a physical-device test before review. Expect to spend an
  hour or two on the first submission.
- A **printable** contribution is a card: one JSON file, one photo, one short
  README. The model files stay on your Printables / MakerWorld / Thingiverse /
  GrabCAD / GitHub page. Expect 15–30 minutes.

**Printables have a shortcut.** The **Submit with the form** button on the
[3D Printables page](https://www.seeedstudio.com/sticky/playground/3d-printables/)
asks for a short version of the same information, takes your photo as an upload, and
opens the pull request here on your behalf. It needs no GitHub account, no git,
and no JSON, and it takes about five minutes. The form links you to the pull
request it created so you can follow the review. Use the manual route below when
you prefer working in git, or when you update a design that is already listed.

**Firmware-only packages have the same shortcut.** The **Submit with the form**
button on the [Firmware page](https://www.seeedstudio.com/sticky/playground/firmware/)
accepts a compiled `.bin` (one merged image, or separate files with offsets)
plus the card details. Source contributions that GitHub Actions compiles still
use the manual route in [contributing-firmware.md](docs/contributing-firmware.md).

This page covers everything the two kinds have in common. Read it once, then
follow the detailed guide for your content type.

## 2. What you need

| Requirement | Firmware | Printable | Notes |
|---|---|---|---|
| A GitHub account | Only for the manual route | Only for the manual route | Free account is enough; the website form needs none |
| Git on your computer | Recommended | Optional | Printables can be submitted entirely in the GitHub web interface (see [Method B](#method-b-github-web-interface-no-git)) |
| Node.js 20 or newer | Recommended | Optional | Runs the same checks locally that GitHub Actions runs on your PR |
| A reTerminal Sticky | Yes | Yes | Firmware must be flashed and tested on a real device; printables need a photo of the print on a real device |
| ESP-IDF, PlatformIO, or Arduino CLI | Optional | No | Only if you want to build firmware locally before submitting |

## 3. Repository layout

```text
firmwares/                     one directory per firmware
  _template/                   copy this to start a firmware
  <firmware-id>/
    firmware.json              card text, links, build settings, versions
    README.md                  behavior, controls, setup, device test record
    assets/
      preview.jpg              real Sticky screenshot or photo
      logo.svg                 optional identity image
    source/                    source contributions: the buildable project
    firmware/<version>/        firmware-only contributions: manifest.json + *.bin

printables/                    one directory per printable design
  _template/                   copy this to start a design
  <design-id>/
    printable.json             card text, author, category, download page
    README.md                  print settings and assembly notes
    assets/
      preview.jpg              photo of the print fitted on Sticky

schemas/
  firmware.schema.json         formal definition of firmware.json
  printable.schema.json        formal definition of printable.json

scripts/
  validate-registry.mjs        checks every directory; run with npm run validate
  create-flash-manifest.mjs    writes a firmware manifest; run with npm run create:manifest
  list-build-targets.mjs       finds source-built firmware for GitHub Actions
  package-esp-idf.mjs          turns an ESP-IDF build into manifest.json + .bin
  package-platformio.mjs       same, for a PlatformIO build
  package-arduino.mjs          same, for an Arduino build

docs/
  contributing-firmware.md     detailed firmware guide (+ .zh-CN.md)
  contributing-printables.md   detailed printables guide (+ .zh-CN.md)

.github/
  workflows/validate-registry.yml   runs on every PR and on main
  pull_request_template.md          the checklist you fill in
```

The directory name is the public identifier of your contribution. It appears
in the website URL for firmware (`/sticky/playground/<firmware-id>/`) and in
analytics for both kinds. Choose it carefully; renaming later is a new PR.

## 4. How a contribution reaches the website

```text
1. You open a pull request that adds or changes one directory.
2. GitHub Actions validates the repository and (for source firmware) compiles it.
3. A maintainer reviews the PR and merges it into main.
4. For source firmware, the main workflow publishes a GitHub Release with the .bin files.
5. The Sticky website repository pins the new Registry commit.
6. The website is built, tested locally and on a device, then deployed.
7. Your card appears on https://www.seeedstudio.com/sticky/playground/.
```

Steps 1–2 are yours. Steps 3–7 are done by maintainers; see
[Section 9](#9-after-your-pull-request-is-merged) for what to expect.

## 5. Three ways to submit

Pick the one that matches your comfort with git. All three end with a pull
request against `Seeed-Projects/reterminal-sticky-playground-registry:main`.

### Method A: fork and clone with git (recommended for firmware)

1. Fork the repository on GitHub: open
   <https://github.com/Seeed-Projects/reterminal-sticky-playground-registry>
   and select **Fork**.
2. Clone your fork and create a branch:

   ```bash
   git clone https://github.com/<your-account>/reterminal-sticky-playground-registry.git
   cd reterminal-sticky-playground-registry
   git checkout -b add-my-design
   ```

3. Copy the template for your content type and fill it in (details in the
   guide for your type):

   ```bash
   cp -R printables/_template printables/my-design     # printable
   cp -R firmwares/_template  firmwares/my-firmware    # firmware
   ```

4. Run the local checks:

   ```bash
   npm test
   npm run validate
   ```

5. Commit and push:

   ```bash
   git add printables/my-design
   git commit -m "feat: add My Design printable"
   git push -u origin add-my-design
   ```

6. Open the pull request. GitHub shows a **Compare & pull request** button on
   your fork; select it, make sure the base is
   `Seeed-Projects/reterminal-sticky-playground-registry` / `main`, then fill in
   the template that appears in the description box.

### Method B: GitHub web interface (no git)

Suitable for printables and for small firmware metadata fixes.

1. Fork the repository (same as step A1).
2. In your fork, open the `printables/` directory and select
   **Add file → Create new file**.
3. In the file name box type `my-design/printable.json`. Typing a `/` creates
   the directory. Paste the contents of
   [`printables/_template/printable.json`](printables/_template/printable.json)
   and edit the values.
4. Select **Commit changes…**, choose **Create a new branch**, name it
   `add-my-design`, and commit.
5. Still on that branch, open `printables/my-design/` and use
   **Add file → Upload files** to upload your photo into an `assets/`
   sub-directory: drag the file in, then edit the path shown above the upload
   area so it reads `printables/my-design/assets/preview.jpg`. Commit.
6. Repeat step 5 with **Create new file** for `printables/my-design/README.md`.
7. GitHub now shows a banner offering to open a pull request from your branch.
   Select it, confirm the base is the Seeed repository's `main`, and fill in the
   template.

GitHub Actions runs the same checks as `npm run validate`; if something is
wrong, the PR shows a red cross and the log tells you which field to fix. Edit
the file on your branch and the check re-runs automatically.

### Method C: edit an existing file in the browser (updates only)

To fix a typo, change a link, or add a version to a card that already exists:

1. Open the file on GitHub (for example
   `printables/sticky-wallet-case/printable.json`).
2. Select the pencil icon (**Edit this file**). GitHub forks the repository for
   you if needed.
3. Make the change, select **Commit changes…**, and choose **Propose changes**.
4. GitHub opens the pull request form; fill in the template.

## 6. Shared rules for every contribution

These are enforced by `npm run validate` and by review.

| Rule | Details |
|---|---|
| One directory per contribution | Everything for one firmware or one design lives in its own directory. Do not touch other directories in the same PR. |
| Directory name = `id` | Lowercase letters, digits, and single hyphens only: `^[a-z0-9]+(?:-[a-z0-9]+)*$`, 2–64 characters. Examples: `sticky-2048`, `wallet-case`. The `id` field inside the metadata file must be identical. |
| HTTPS links only | Every URL field must start with `https://`. |
| Images under `assets/` | Referenced as `assets/<file>`. Firmware accepts PNG, JPG, WebP, or static SVG; printables accept PNG, JPG, WebP. Maximum 5 MB for previews, 1 MB for logos. |
| Real images | Previews are real screenshots or photos on a reTerminal Sticky. Renders and stock images are not accepted. |
| English card text | `name`, `summary`, `description`, `alt` texts, and README are in English because the website serves a global audience. |
| No secrets | Source, configuration, and README contain no Wi-Fi passwords, API keys, tokens, or private keys. Use placeholders and describe runtime setup instead. |
| Your own work or clearly credited | `author` names the person or team who made it. When you submit someone else's open design or firmware, credit them in `author` and link the original in `download.url` / `source.url`. |
| Metadata files are strict | Unknown fields cause validation to fail. Copy the template and change values rather than adding new keys. |

## 7. Local checks

Install [Node.js](https://nodejs.org/) 20 or newer, then run from the
repository root:

```bash
npm test          # unit tests for the validator itself
npm run validate  # checks every directory under firmwares/ and printables/
```

Successful output:

```text
Registry validation passed (12 firmware(s), 5 printable(s)).
```

Failure output lists each problem with the exact file and field:

```text
Registry validation failed with 2 error(s):
- printables/my-design/printable.json.download.url: must use HTTPS
- printables/my-design/printable.json.preview.image: references a missing file: assets/preview.jpg
```

Read the part after the file name as a path into your JSON: `download.url`
means the `url` key inside the `download` object. Fix the field, run the
command again, repeat until it passes.

What the validator checks, for both kinds:

- the directory name matches `id`;
- every required field is present and no unknown field exists;
- strings stay within their length limits and enumerated fields use an allowed value;
- URLs are HTTPS;
- referenced images exist, use an allowed extension, contain real image data, and stay under the size limit;
- `README.md` exists.

Firmware adds checks for source trees, build settings, manifests, `.bin` sizes,
SHA-256 values, and flash offsets; see the firmware guide.

## 8. Pull request review

1. Open the pull request and fill in the template. Select exactly one
   **Contribution type**; only the matching section of the template needs to be
   completed.
2. GitHub Actions runs `npm test` and `npm run validate`. For source-built
   firmware it also compiles the project and attaches the firmware as a PR
   artifact. A red cross means something failed; open **Details** to read the
   log.
3. A **review card** comment appears on the pull request within a minute or two.
   It shows the card as reviewers see it, the preview photo, whether the links
   answer, and anything that needs attention such as a missing licence or a
   credential left in the diff. The comment is rewritten on every push, so it
   always describes the current state of your branch.
4. A maintainer reviews metadata, images, links, license, and (for firmware)
   the physical-device test record. Review comments appear on the PR; reply or
   push fixes to the same branch.
5. When the checks are green and the review is complete, the maintainer merges.

Typical turnaround is a few working days. If a PR has been quiet for more than
two weeks, leave a comment on it.

## 9. After your pull request is merged

Merging does not publish immediately. The Sticky website reads one pinned
Registry commit, so maintainers release in batches:

1. Merge the Registry pull request.
2. For source-built firmware, wait for the Registry `main` workflow to publish
   the firmware Release.
3. Update the pinned Registry commit in the Sticky website repository.
4. Build the website locally and confirm the new card (and flashing page, for
   firmware).
5. For firmware, flash it from the local page to a physical device.
6. Merge into the Sticky website `main` and deploy.

Your card appears on <https://www.seeedstudio.com/sticky/playground/> after
step 6. You will see the closed PR referenced in the website release notes when
that happens.

## 10. Updating or removing a contribution

**Update.** Edit the files in your directory and open a new pull request using
any method above. Select **Firmware: update to an existing firmware** or
**3D printable design** in the template. Firmware updates add the new version
first in `flash.versions`; the firmware guide has the details.

**Remove.** Open a pull request that deletes the directory and explain why in
the description. Maintainers may instead keep the directory and mark it as no
longer maintained if users still depend on it.

**Transfer.** If someone else takes over maintenance, open a PR that changes
`author` and links, and mention the previous author in the description so they
can confirm.

## 11. FAQ

**Can I submit more than one design or firmware in one PR?**
Please open one PR per directory. It keeps review focused and lets each card be
released independently.

**My design is on several platforms. Which one goes in `download.url`?**
Pick the page you maintain most actively. Mention the others in the README.

**I found a design by someone else that fits Sticky well. Can I add it?**
Yes, if its license allows redistribution of the link and credit. Put the
original designer in `author`, link their page in `download.url`, and say in
the PR that you are submitting on their behalf.

**Do I have to run `npm run validate` locally?**
No. GitHub Actions runs it on your PR. Running it locally just gives you the
answer in seconds instead of minutes.

**The validator says `contains unsupported field "..."`.**
The metadata files only accept the fields listed in their guide. Remove the
extra key, or check for a typo in a valid key.

**How do I test firmware on my device before submitting?**
Build locally with your project's toolchain and flash it from there, or use the
PR artifact that GitHub Actions produces after you open the PR. The firmware
guide describes both.

**Who do I ask if something is unclear?**
Open an issue in this repository, or ask in the pull request itself.
