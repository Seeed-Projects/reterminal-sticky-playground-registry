# Contributing firmware

This guide covers firmware for the **Firmware** page of the Sticky Playground.
A completed community firmware contribution becomes a card on that page and
opens a Seeed-hosted browser flashing page. To share a 3D printable case or
stand instead, follow [Contributing a 3D printable design](contributing-printables.md).

Contributors may submit either a complete buildable source project, or a
verified firmware-only package with an upstream source link. For source
contributions, GitHub Actions builds and packages the firmware. Both paths
include metadata, visual assets, and a physical-device test record. The private
Sticky website reads only a reviewed Registry commit and generates the public
card and flashing page.

## The five-minute route: the website form

When you already have a compiled `.bin`, the **Submit with the form** button on
the [Firmware page](https://www.seeedstudio.com/sticky/playground/firmware/)
opens a short form: the firmware name, category, summary, version, license,
project page, one photo, and the binary. You can send one merged image written
at `0x0`, or separate files at the offsets already used by packages on
Playground (`0x0`, `0x8000`, `0xE000`, `0x10000`). The submission service writes
`firmware.json`, `README.md`, `assets/preview.<ext>`, and
`firmware/<version>/` for you and opens the pull request. The page then shows
the pull request link. You need no GitHub account, no git, and no JSON editor.

Source contributions that GitHub Actions compiles still use the manual route
below. Read on when you prefer to prepare the files yourself, when you update a
firmware that is already listed, or when you want to understand exactly what
the form produces.

For the Chinese guide, see [contributing-firmware.zh-CN.md](contributing-firmware.zh-CN.md).
Shared review and release steps live in [CONTRIBUTING.md](../CONTRIBUTING.md).

## Contents

- [The five-minute route: the website form](#the-five-minute-route-the-website-form)
- [Contribution result](#contribution-result)
- [Required pull request contents](#required-pull-request-contents)
- [Create a contribution](#create-a-contribution)
- [firmware.json](#firmwarejson) and its [field reference](#field-reference)
- [Contribution paths](#contribution-paths): [source](#source-contribution) or [firmware-only](#firmware-only-package) (with the [manifest.json reference](#manifestjson-for-a-firmware-only-package))
- [Official Sticky firmware updates](#official-sticky-firmware-updates)
- [Source contribution requirements](#source-contribution-requirements)
- [Optional local build](#optional-local-build)
- [Local validation](#local-validation) and [common validation errors](#common-validation-errors)
- [Submitting the pull request](#submitting-the-pull-request)
- [Physical device test](#physical-device-test)
- [Pull request review and automation](#pull-request-review-and-automation)
- [What happens after merge](#what-happens-after-merge)
- [Updating an existing firmware](#updating-an-existing-firmware)
- [Pull request checklist](#pull-request-checklist)

## Contribution result

A published community contribution provides this user flow:

1. The user opens Sticky Playground.
2. The user selects the community firmware card.
3. The user reviews the firmware version and installation notes on the Sticky website.
4. The user connects reTerminal Sticky by USB.
5. The user selects **Flash Now** to install the firmware in the browser.

The Registry stores the installable package and either the buildable source or
its upstream source reference. The Sticky website owns the card layout, browser
serial connection, flashing interface, domain, and production deployment.

## Required pull request contents

Create one directory under `firmwares/`. Choose one of the following package
layouts.

Source contribution:

```text
firmwares/
  my-firmware/
    firmware.json
    README.md
    assets/
      preview.jpg
      logo.svg  # optional
    source/
      CMakeLists.txt
      sdkconfig.defaults
      main/
      components/
      LICENSE
```

Firmware-only contribution:

```text
firmwares/
  my-firmware/
    firmware.json
    README.md
    assets/
      preview.jpg
      logo.svg  # optional
    firmware/
      1.0.0/
        manifest.json
        bootloader.bin
        partition-table.bin
        my-firmware.bin
```

| Item | Purpose |
|---|---|
| `firmware.json` | Card text, author, compatibility, build settings, and firmware versions |
| `README.md` | Firmware behavior, controls, setup, and hardware test record |
| `assets/logo.*` | Official identity asset for partner entries; optional project identity asset for community entries |
| `assets/preview.*` | A real Sticky screenshot or photo for community entries, or the official logo used by a partner entry |
| `source/` | Complete source for a source contribution |
| `source/LICENSE` | License covering the locally submitted source |
| `firmware/<version>/manifest.json` | Flash layout, sizes, and SHA-256 values for firmware-only contributions |
| `firmware/<version>/*.bin` | Ready-to-install files for firmware-only contributions |

The directory name and `firmware.json.id` use the same lowercase kebab-case
identifier, such as `weather-dashboard` or `sticky-2048`.

Images under `firmwares/<firmware-id>/assets/` may use `.png`, `.jpg`,
`.jpeg`, `.webp`, or static `.svg` files. Community entries use a real Sticky
screenshot or photo as the preview. Coordinated partner entries may reference
the same official logo path from both `assets.logo` and `assets.preview`.

## Create a contribution

Start from the current upstream `main` so the contribution uses the directory
layout the Registry reads today:

```bash
# Once per clone
git remote add upstream https://github.com/Seeed-Projects/reterminal-sticky-playground-registry.git
git pull --no-ff upstream main
```

From the repository root:

```bash
cp -R firmwares/_template firmwares/my-firmware
```

Complete the files in this order:

1. Rename the directory and update `firmware.json.id`.
2. Add community author attribution, or the coordinated partner's official project links.
3. Add the project README, upstream source URL, and source license name.
4. Add a real Sticky preview for a community entry, or the official logo for a partner entry.
5. For a source contribution, add `source/`, `build` metadata, and `sourceBuild: true`.
6. For a firmware-only contribution, add the tested package under `firmware/<version>/`.
7. Run the Registry tests and validator.
8. Flash the packaged firmware to a physical reTerminal Sticky.
9. Open a pull request with the test result.

## firmware.json

Normal third-party submissions use `"group": "community"`,
`"catalogSection": "community"`, and `"mode": "flash"`.

```json
{
  "schemaVersion": 1,
  "id": "my-firmware",
  "name": "My Firmware",
  "group": "community",
  "catalogSection": "community",
  "category": "productivity",
  "mode": "flash",
  "status": "experimental",
  "summary": "Turn Sticky into a focused information display.",
  "description": "My Firmware provides a local information display with touch controls and an offline data source.",
  "author": {
    "name": "Project author or team",
    "url": "https://github.com/example"
  },
  "origin": {
    "name": "Project repository",
    "url": "https://github.com/example/my-firmware"
  },
  "source": {
    "url": "https://github.com/example/my-firmware",
    "license": "MIT",
    "path": "source"
  },
  "support": {
    "url": "https://github.com/example/my-firmware/issues"
  },
  "documentationUrl": "https://github.com/example/my-firmware#readme",
  "compatibility": {
    "devices": ["reterminal-sticky"],
    "notes": "Tested on reTerminal Sticky production hardware."
  },
  "assets": {
    "preview": "assets/preview.jpg",
    "previewAlt": "My Firmware running on reTerminal Sticky"
  },
  "tags": ["dashboard", "offline"],
  "build": {
    "system": "esp-idf",
    "version": "v5.4",
    "target": "esp32s3",
    "projectPath": "source"
  },
  "flash": {
    "versions": [
      {
        "version": "1.0.0",
        "channel": "experimental",
        "sourceBuild": true
      }
    ],
    "notes": [
      {
        "title": "Device connection",
        "description": "Connect reTerminal Sticky with a USB data cable and use desktop Chrome or Edge."
      }
    ]
  }
}
```

### Field reference

`firmware.json` is a strict JSON object: every key below is either required or
optional, and **any other key makes validation fail**. The formal definition is
[`schemas/firmware.schema.json`](../schemas/firmware.schema.json). Community
contributions fill in the "Community value" column; partner and official
entries are coordinated with Seeed.

#### Identity and catalog placement

| Field | Type | Required | Limits | Community value / what to write |
|---|---|---|---|---|
| `schemaVersion` | integer | Yes | must be `1` | `1` |
| `id` | string | Yes | 2–64 chars, `^[a-z0-9]+(?:-[a-z0-9]+)*$` | Same as the directory name, for example `sticky-2048` |
| `name` | string | Yes | 1–80 chars | Card title shown on the website |
| `group` | string | Yes | `official`, `partner`, `community` | `community` |
| `catalogSection` | string | Yes | `official`, `platform`, `community`, `draft` | `community`. Cards in `draft` are not published |
| `category` | string | Required for `community` | `ereader`, `productivity`, `personal`, `weather`, `finance`, `tools`, `fun`, `smart-home`, `other` | The filter that shows your card; see the table below |
| `mode` | string | Yes | `flash`, `external`, `template`, `download` | `flash` (browser flashing). The other modes are reserved for coordinated entries |
| `status` | string | Yes | `experimental`, `beta`, `stable` | Project maturity; shown as a badge |
| `summary` | string | Yes | 1–140 chars | One sentence under the title |
| `description` | string | Yes | 1–800 chars | Two or three sentences: what it does, what services it needs, what users should expect |
| `tags` | array of strings | No | up to 6 items, each 1–32 chars, unique | Short labels such as `offline`, `wifi`, `touch` |

Community firmware categories:

| Value | Filter label | Use it for |
|---|---|---|
| `ereader` | eReader | Book, article, and document readers |
| `productivity` | Productivity | Task lists, calendars, notes, timers, dashboards for work |
| `personal` | Personal | Habit trackers, prayer times, journals, personal reminders |
| `weather` | Weather | Weather, air quality, tides, forecasts |
| `finance` | Finance | Prices, portfolios, budgets |
| `tools` | Tools | Utilities, calculators, converters, device diagnostics |
| `fun` | Fun | Games, art, toys, novelty displays |
| `smart-home` | Smart Home | Home Assistant, sensors, controls, presence |
| `other` | Others | Firmware that fits none of the categories above |

#### Attribution and links

| Field | Type | Required | Limits | What to write |
|---|---|---|---|---|
| `author.name` | string | Required for `community` | 1–80 chars | Author or team name shown on the card |
| `author.url` | string | No | HTTPS URL | Profile or project page opened when the name is selected |
| `origin.name` | string | No | 1–80 chars | Display name of where the firmware comes from, for example `Project repository` or a community name |
| `origin.url` | string | No | HTTPS URL | Link for `origin.name` |
| `source.url` | string | Yes | HTTPS URL | Upstream source repository, for both contribution paths |
| `source.license` | string | Required for firmware-only | 1–80 chars | SPDX-style license id such as `MIT`, `GPL-3.0`, `Apache-2.0` |
| `source.path` | string | Required for source contributions | relative path inside the directory, normally `source` | Directory containing the project; Arduino uses the sketch name |
| `support.url` | string | Yes | HTTPS URL | Where users report problems, normally the issue tracker |
| `documentationUrl` | string | No | HTTPS URL | User documentation, normally the upstream README |

#### Compatibility and images

| Field | Type | Required | Limits | What to write |
|---|---|---|---|---|
| `compatibility.devices` | array | Yes | exactly `["reterminal-sticky"]` | Always `["reterminal-sticky"]` |
| `compatibility.notes` | string | No | up to 400 chars | Tested hardware revision, required accessories, known limits |
| `assets.preview` | string | Yes | `assets/<file>.(png\|jpg\|jpeg\|webp\|svg)`, ≤ 5 MB | Real Sticky screenshot or photo of the firmware running |
| `assets.previewAlt` | string | Yes | 1–180 chars | One sentence describing the preview |
| `assets.logo` | string | No | same pattern, ≤ 1 MB | Optional project logo. Partner entries use their official logo here and in `preview` |

#### Build settings (source contributions only)

| Field | Type | Required | Limits | What to write |
|---|---|---|---|---|
| `build.system` | string | Yes | `esp-idf`, `platformio`, or `arduino` | The build system the project already uses |
| `build.version` | string | ESP-IDF only | up to 64 chars, `^[A-Za-z0-9][A-Za-z0-9._-]*$` | ESP-IDF version the project builds with: `v5.4`, `v5.3.2`, `latest` |
| `build.target` | string | ESP-IDF only | up to 40 chars | `esp32s3` (reTerminal Sticky uses an ESP32-S3) |
| `build.environment` | string | PlatformIO only | up to 64 chars, `^[A-Za-z0-9][A-Za-z0-9._-]*$` | `platformio.ini` environment name, without the `env:` prefix |
| `build.profile` | string | Arduino only | up to 64 chars, `^[A-Za-z0-9][A-Za-z0-9._-]*$` | `sketch.yaml` profile name |
| `build.projectPath` | string | Yes | relative path | Same value as `source.path` |

Each build system accepts only its own fields. Write `version` and `target` for
ESP-IDF, `environment` for PlatformIO, and `profile` for Arduino.

Omit the whole `build` object for firmware-only packages.

#### Firmware versions

| Field | Type | Required | Limits | What to write |
|---|---|---|---|---|
| `flash.versions` | array | Yes | at least 1, newest first | One entry per published version |
| `flash.versions[].version` | string | Yes | 1–40 chars, unique | Version label shown to users, for example `1.0.0` |
| `flash.versions[].channel` | string | Yes | `experimental`, `beta`, `stable` | Maturity of that version |
| `flash.versions[].sourceBuild` | boolean | Source contributions: `true` on the newest version | — | GitHub Actions builds this version from `source/` |
| `flash.versions[].manifestPath` | string | Firmware-only: required on every version | `firmware/<version>/manifest.json` | Path to the committed manifest |
| `flash.versions[].manifestUrl`, `manifestSha256`, `releaseUrl` | string | Maintainer use | — | Used for versions that already live in a Registry GitHub Release |
| `flash.notes` | array | No | 1–12 items | Installation notes shown on the flashing page; each item has `title` (≤ 100) and `description` (≤ 500) |

Each version uses exactly one delivery method: `sourceBuild: true`, or
`manifestPath`, or the Release triplet.

`author` and `origin` are website attribution fields. The `source` object keeps
the technical source repository, license, and local build path used during
review and packaging.

`official` and `platform` catalog sections are maintained through coordinated
Seeed or partner work. Partner entries use `"group": "partner"`,
`"catalogSection": "platform"`, `"mode": "flash"`, official project links,
and official identity assets. Author attribution is optional for partner and
official cards because their platform identity is represented by the
firmware name and official links. Normal community pull requests target the
`community` section. Maintainers may
temporarily use `draft` for migrated entries that are still waiting for a
complete firmware package; draft entries are not published to Sticky
Playground.

## Contribution paths

### Source contribution

Include `source/`, set `source.path`, add the matching `build` object, and set
the newest firmware version to `"sourceBuild": true`. GitHub Actions compiles
the source in a clean environment and creates the manifest and `.bin` files.
The generated files appear as a temporary PR artifact for review. They are
published as a versioned GitHub Release after the PR is merged.
Each project version has one immutable Release, so source updates use a new
`flash.versions[].version` value.

### Firmware-only package

Include `source.url` and `source.license`, omit `source.path` and `build`, and
commit the complete local package under `firmware/<version>/`. The manifest must
list every required binary with its flash offset, byte size, and SHA-256. Record
the tested hardware, firmware version, package origin, and physical-device test
result in the integration README and pull request.

```json
"source": {
  "url": "https://github.com/example/my-firmware",
  "license": "MIT"
}
```

#### manifest.json for a firmware-only package

Each version directory contains one `manifest.json` and the `.bin` files it
lists. The website reads this manifest to flash the device in the browser.

```json
{
  "name": "My Firmware",
  "version": "1.0.0",
  "flashSize": "16MB",
  "flashMode": "dio",
  "flashFreq": "80m",
  "baudRate": 460800,
  "new_install_prompt_erase": true,
  "builds": [
    {
      "chipFamily": "ESP32-S3",
      "parts": [
        { "path": "bootloader.bin",      "offset": 0,      "size": 21344,   "sha256": "<sha256>" },
        { "path": "partition-table.bin", "offset": 32768,  "size": 3072,    "sha256": "<sha256>" },
        { "path": "my-firmware.bin",     "offset": 65536,  "size": 1523712, "sha256": "<sha256>" }
      ]
    }
  ]
}
```

| Field | Required | What to write |
|---|---|---|
| `name` | Yes | Same as `firmware.json` `name` |
| `version` | Yes | Same as the `flash.versions[].version` that points at this manifest |
| `flashSize` | No | Flash size of the device, `16MB` for reTerminal Sticky; used to reject parts that overflow |
| `flashMode`, `flashFreq`, `baudRate`, `new_install_prompt_erase` | No | Passed to the browser flasher; the values above work for Sticky |
| `builds[].chipFamily` | Yes | `ESP32-S3` |
| `builds[].parts[].path` | Yes | File name only, in the same directory as the manifest, ending in `.bin` |
| `builds[].parts[].offset` | Yes | Flash address in bytes (decimal), taken from your build's `flasher_args.json` |
| `builds[].parts[].size` | Yes | Exact byte size of the file |
| `builds[].parts[].sha256` | Yes | Lowercase SHA-256 of the file |

Parts must not overlap and each `.bin` may not exceed 32 MB.

**Let the repository write the manifest.** Copy your `.bin` files into
`firmwares/my-firmware/firmware/1.0.0/`, then run one command from the
repository root; it fills in every size and SHA-256 for you and prints the
`flash.versions` entry to paste into `firmware.json`:

```bash
npm run create:manifest -- my-firmware 1.0.0 --part bootloader.bin@0x0 --part partitions.bin@0x8000 --part firmware.bin@0x10000
```

If you built with ESP-IDF, point the command at the build instead of typing
offsets:

```bash
npm run create:manifest -- my-firmware 1.0.0 --flasher-args source/build/flasher_args.json
```

A single merged image needs no offsets at all:

```bash
npm run create:manifest -- my-firmware 1.0.0
```

Running the command again after replacing a `.bin` refreshes the sizes and
hashes and keeps the flasher settings the manifest already had.

Offsets come from ESP-IDF's `build/flasher_args.json` (`flash_files`) or the
upstream project's release notes. A single merged image starts at offset `0`.
To read the numbers by hand instead:

```bash
cd firmwares/my-firmware/firmware/1.0.0
wc -c *.bin                 # size
shasum -a 256 *.bin         # sha256 (macOS/Linux); certutil -hashfile file SHA256 on Windows
```

## Official Sticky firmware updates

Seeed-maintained updates to `sticky-factory` use the firmware-only package path
with `"group": "official"`, `"catalogSection": "official"`, and
`"mode": "flash"`. Add each new official version under
`firmwares/sticky-factory/firmware/<version>/` and set the newest
`flash.versions` entry to the exact local path
`firmware/<version>/manifest.json`.

The version directory contains the complete tested binary package. Its manifest
records the firmware version, chip family, flash settings, part offsets, byte
sizes, SHA-256 values, and optional MD5 values. The integration README and pull
request record the official artifact origin and the same package metadata.

Official versions that already use Registry GitHub Releases keep their existing
`manifestUrl`, `manifestSha256`, and `releaseUrl` records. This allows the newest
version to use repository-backed storage while preserving every historical
download. The committed version directory is the single delivery record for a
new repository-backed official version.

Select **Official Sticky firmware update maintained by Seeed** in the pull
request template and complete its package, provenance, and physical-device
verification fields.

## Source contribution requirements

For the source path, the `source/` directory must build independently
from the submitted files. It includes project build files, application code,
local components, dependency locks or manifests, default configuration, and the
applicable license.

Use placeholders or runtime setup for user-specific Wi-Fi credentials, API
keys, tokens, and passwords.

CI builds ESP-IDF, PlatformIO, and Arduino projects. Pick the one your project
already uses and declare it in `build.system`. Starting points for PlatformIO and
Arduino live in [examples/](../examples/README.md).

### ESP-IDF

```text
source/
  CMakeLists.txt
  sdkconfig.defaults
  main/
    CMakeLists.txt
    main.cpp
  components/
  LICENSE
```

```json
"build": {
  "system": "esp-idf",
  "version": "v5.4",
  "target": "esp32s3",
  "projectPath": "source"
}
```

`build.version` selects the ESP-IDF Docker image tag, for example `v5.0.5`,
`v5.3.2`, or `latest`.

### PlatformIO

```text
source/
  platformio.ini
  src/
    main.cpp
  LICENSE
```

```json
"build": {
  "system": "platformio",
  "environment": "sticky",
  "projectPath": "source"
}
```

`build.environment` names the `[env:...]` section CI builds, without the `env:`
prefix. Pin the platform version in `platformio.ini` so a rebuild produces the
same binary:

```ini
[env:sticky]
platform = espressif32@6.13.0
board = esp32-s3-devkitc-1
framework = arduino
```

### Arduino

Arduino CLI requires the sketch directory and the `.ino` file to share a name, so
name the project directory after the sketch instead of using `source`:

```text
my-sketch/
  my-sketch.ino
  sketch.yaml
  LICENSE
```

```json
"build": {
  "system": "arduino",
  "profile": "sticky",
  "projectPath": "my-sketch"
}
```

`build.profile` names a profile in `sketch.yaml`. A profile pins the board core
and every library, which is what makes the build reproducible:

```yaml
profiles:
  sticky:
    fqbn: esp32:esp32:esp32s3
    platforms:
      - platform: esp32:esp32 (3.3.11)
        platform_index_url: https://espressif.github.io/arduino-esp32/package_esp32_index.json
default_profile: sticky
```

Add the board options your firmware needs to the `fqbn`, for example
`esp32:esp32:esp32s3:PSRAM=opi,FlashSize=32M` for the 32 MB flash and octal PSRAM
that reTerminal Sticky ships.

Projects on another build system should open an issue first so the maintainers can
add a reproducible CI build adapter before the firmware PR.

## Optional local build

Authors may build locally before opening the PR. Run the command for the declared
build system from the project directory:

```bash
# ESP-IDF
idf.py set-target esp32s3
idf.py -D PROJECT_VER=1.0.0 build

# PlatformIO
pio run -e sticky

# Arduino
arduino-cli compile --profile sticky
```

This local build verifies the project before submission. The Registry ignores
local build output, generated configuration, dependency caches, and editor
settings. GitHub Actions performs the official build and packaging after the PR
is opened.

## Local validation

Install Node.js 20 or newer, then run:

```bash
npm test
npm run validate
```

The validator checks:

- directory and ID consistency;
- required metadata and HTTPS links;
- the integration README and declared source license;
- local source and the build system's project files when `source.path` is present;
- image file signatures and static SVG safety;
- local manifest structure;
- firmware file existence, byte size, and SHA-256;
- non-overlapping flash address ranges;
- the direct-flash requirements for community catalog entries.
- the repository-backed package path for the newest Sticky official firmware.

For source contributions, manifest and firmware-file checks run after GitHub
Actions builds the project. For firmware-only contributions, they run directly
against the files committed in the PR.

### Common validation errors

| Error text | Cause | Fix |
|---|---|---|
| `integrations: entries belong in firmwares/ with their metadata in firmware.json` | The branch started from an earlier directory layout | Sync with upstream `main`, then move the entry to `firmwares/<id>/` and rename its metadata to `firmware.json` |
| `firmwares/my-firmware: is missing firmware.json` | File not created or misnamed | Create `firmwares/my-firmware/firmware.json` |
| `...firmware.json: contains unsupported field "..."` | A key that is not in the field reference | Remove it or fix the spelling |
| `...firmware.json.id: must match the directory name "..."` | `id` differs from the directory | Make them identical |
| `...firmware.json.category: is required for community firmware entries` | Missing `category` | Add one of the eight values |
| `...firmware.json.author: is required for community firmware` | Missing `author` | Add `author.name` |
| `...firmware.json.source.license: is required for firmware-only packages` | Firmware-only without a license id | Add `source.license` |
| `...firmware.json.build: is required when source.path is provided` | Source contribution without `build` | Add the `build` object |
| `...firmware.json.build.projectPath: must contain CMakeLists.txt for the esp-idf build system` | `projectPath` is empty or points at the wrong directory | Commit the project root there. PlatformIO needs `platformio.ini`, Arduino needs `sketch.yaml` |
| `...firmware.json.build: contains unsupported field "target"` | Fields from another build system | Keep only the fields your `build.system` accepts |
| `...flash.versions[0].sourceBuild: must be true for a source contribution` | Newest version lacks `sourceBuild` | Set `"sourceBuild": true` on the first entry |
| `...flash.versions[0]: must use exactly one firmware delivery method` | `sourceBuild` and `manifestPath` both set, or neither | Keep one |
| `...manifestPath: must be inside the firmware/ directory` | Manifest stored elsewhere | Move to `firmware/<version>/manifest.json` |
| `...manifestPath: version must match firmware version "1.0.0"` | Manifest `version` differs from `flash.versions[].version` | Make them identical |
| `...parts[0].path: must be one .bin filename beside the manifest` | Path contains a directory or wrong extension | Put the file next to the manifest and use its bare name |
| `...parts[0].size: expected 1523712 bytes but found 1523700` | Wrong `size` | Re-run `wc -c` and update |
| `...parts[0].sha256: does not match the firmware file` | Wrong hash or file changed after hashing | Re-run `shasum -a 256` and update |
| `...parts[1]: overlaps the previous firmware part` | `offset + size` of one part runs into the next | Check offsets against `flasher_args.json` |
| `...assets.preview: does not contain a valid PNG file signature` | A JPG renamed to `.png` (or similar) | Use the real extension |
| `...source.LICENSE: references a missing file: source/LICENSE` | License file missing from the source tree | Add `source/LICENSE` |

## Submitting the pull request

Any of the three methods in
[CONTRIBUTING.md → Three ways to submit](../CONTRIBUTING.md#5-three-ways-to-submit)
works; firmware is easiest with git because of the number of files.

```bash
git checkout -b add-my-firmware
git add firmwares/my-firmware
git commit -m "feat: add My Firmware 1.0.0"
git push -u origin add-my-firmware
```

Then open the pull request against
`Seeed-Projects/reterminal-sticky-playground-registry` / `main` and complete
the template:

1. Under **Contribution type**, tick one **Firmware:** line.
2. Complete **Common verification**.
3. In the **Firmware** section, fill in the name, directory, version, upstream
   project, license, and (for firmware-only) the artifact origin and SHA-256.
4. Tick the **Package type** and complete only the matching sub-list (source
   contribution or firmware-only).
5. Complete **Physical-device test** with the device revision, install method,
   version, workflow tested, and the reboot/USB results.

Title suggestion: `Add <Firmware Name> <version>`.

Source contributions: after the PR is opened, GitHub Actions compiles the
project. When the **Build <id>** job finishes, its page has an artifact named
`firmware-<id>-<version>` containing `manifest.json` and the `.bin` files. You
can download it and flash it with `esptool` or the browser flasher to complete
the device test on the exact bytes reviewers will publish.

## Physical device test

Install the packaged files on a reTerminal Sticky using the same offsets in
`manifest.json`. Test at least:

- power-on and first boot;
- the main user workflow;
- touch and hardware buttons used by the project;
- reboot and saved state when the firmware stores data;
- USB reconnection and one repeated installation.

Record the tested hardware, ESP-IDF version, firmware version, and result in the
pull request. A photo or short video of the main workflow helps reviewers verify
the submitted preview and behavior.

## Pull request review and automation

The pull request contains the selected contribution path, metadata, assets, and
hardware test result. GitHub Actions performs these checks:

1. Validate the Registry structure and every firmware-only package.
2. Build source contributions with the toolchain version the project declares.
3. Generate the flash manifest and firmware package from the toolchain's flash map.
4. Upload the generated package as a temporary PR artifact for maintainer review.

The review card also lists any change to Official Sticky firmware or Partner
Firmwares. Those catalog entries are maintained by Seeed or the partner
platform; community submissions use Community Firmwares.

The pull request remains under review until these checks pass and the maintainer
can verify the project purpose, license, compatibility, and hardware test.

## What happens after merge

Merging the Registry pull request does not immediately publish production. The
maintainer uses the reviewed release flow:

1. Merge the complete Registry pull request.
2. Wait for the Registry `main` workflow to publish the source-built firmware Release.
3. Update the pinned Registry commit on the Sticky preview branch.
4. Build Sticky locally and confirm the new card and flashing page.
5. Flash the firmware from the local Sticky page to a physical device.
6. Merge the tested Sticky preview branch into Sticky `main`.
7. Deploy the Sticky website through the company server and Kubernetes.

This separates contributor review, real-device acceptance, and production
release while keeping the private website repository closed.

## Updating an existing firmware

For a new version:

1. Add the new version to `flash.versions` with the newest version first.
2. For a source contribution, update `source/` and set the new version to `sourceBuild: true`.
3. For a firmware-only contribution, add the tested package under `firmware/<version>/`.
4. Keep older firmware directories referenced by `firmware.json`.
5. Run validation and repeat the physical device test.
6. Describe user-visible changes and upgrade behavior in the pull request.

## Pull request checklist

- [ ] One firmware directory contains the complete contribution.
- [ ] `firmware.json` uses the group, catalog section, and mode documented for the selected contribution type.
- [ ] Community firmware entries include a `category` from the documented list.
- [ ] The contribution uses source plus build metadata, or firmware-only plus the provenance required for its contribution type.
- [ ] Official firmware updates use the repository-backed version directory and record the official artifact origin.
- [ ] The source path uses `sourceBuild: true`, or the firmware-only path includes the manifest and every required `.bin`.
- [ ] The README and PR identify the tested package origin and firmware version.
- [ ] `npm test` and `npm run validate` pass.
- [ ] The packaged firmware was tested on a physical reTerminal Sticky.
- [ ] The PR describes the hardware, firmware version, result, and build version when applicable.
