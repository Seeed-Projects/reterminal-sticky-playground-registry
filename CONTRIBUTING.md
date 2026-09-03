# Contributing to reTerminal Sticky Playground

This repository is the public contribution and review layer for the reTerminal
Sticky Playground. It accepts two kinds of contributions, each with its own
directory, metadata file, and guide.

For the Chinese version, see [CONTRIBUTING.zh-CN.md](CONTRIBUTING.zh-CN.md).

## Choose your guide

| I want to share… | Directory | Metadata file | Guide |
|---|---|---|---|
| Firmware that users flash from the Sticky website | `firmwares/<firmware-id>/` | `firmware.json` | [Contributing firmware](docs/contributing-firmware.md) |
| A 3D printable case, stand, mount, or accessory | `printables/<design-id>/` | `printable.json` | [Contributing a 3D printable design](docs/contributing-printables.md) |

Firmware contributions include buildable source or a verified firmware package,
and are tested on a physical device before review. Printable contributions are
a card with a preview photo; the model files stay on the author's download page.

## Repository layout

```text
firmwares/
  _template/                 copy this for a new firmware
  <firmware-id>/
    firmware.json
    README.md
    assets/
    source/                  source contributions
    firmware/<version>/      firmware-only contributions
printables/
  _template/                 copy this for a new printable design
  <design-id>/
    printable.json
    README.md
    assets/preview.jpg
schemas/
  firmware.schema.json
  printable.schema.json
scripts/
  validate-registry.mjs      checks both directories
```

## Shared rules

- One directory per contribution. The directory name and the `id` inside the
  metadata file are the same lowercase kebab-case identifier.
- All links use HTTPS.
- Images live under `assets/` and use PNG, JPG, WebP, or (firmware only) static SVG.
- Submitted files contain no personal Wi-Fi credentials, API keys, tokens, or passwords.
- Card text is written in English because the Sticky website serves a global audience.

## Local checks

Install Node.js 20 or newer, then run from the repository root:

```bash
npm test
npm run validate
```

The validator checks every firmware and printable directory and prints
`Registry validation passed (N firmware(s), M printable(s)).` when the
repository is consistent. GitHub Actions runs the same commands on every pull
request.

## Pull request review

1. Open a pull request and select the contribution type in the template.
2. GitHub Actions validates the Registry and, for source-built firmware,
   compiles the project and attaches the firmware as a PR artifact.
3. A maintainer reviews the metadata, assets, links, license, and (for
   firmware) the physical-device test record.
4. The pull request is merged once the checks pass and the review is complete.

## After your pull request is merged

Merging a Registry pull request does not publish to the production website by
itself. The Sticky website pins one reviewed Registry commit. Maintainers
follow this sequence:

1. Merge the Registry pull request.
2. For source-built firmware, wait for the Registry `main` workflow to publish
   the firmware Release.
3. Update the pinned Registry commit in the Sticky website repository.
4. Build the website locally and confirm the new card (and flashing page, for
   firmware).
5. For firmware, flash it from the local page to a physical device.
6. Merge into the Sticky website `main` and deploy.

New cards appear on <https://www.seeedstudio.com/sticky/playground/> after step 6.

## Updating an existing contribution

Edit the files in your directory and open a new pull request. Firmware updates
add the new version first in `flash.versions`; see the firmware guide.
Printable updates change the card text, photo, category, or download page.
