# Contributing Firmware to reTerminal Sticky Playground

This repository is the public contribution and review layer for the reTerminal
Sticky firmware catalog. A completed community contribution becomes a card on
the Sticky Playground website and opens a Seeed-hosted browser flashing page.

Contributors may submit either a complete buildable source project, or a
verified firmware-only package with an upstream source link. For source
contributions, GitHub Actions builds and packages the firmware. Both paths
include metadata, visual assets, and a physical-device test record. The private
Sticky website reads only a reviewed Registry commit and generates the public
card and flashing page.

For the Chinese guide, see [CONTRIBUTING.zh-CN.md](CONTRIBUTING.zh-CN.md).

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

Create one directory under `integrations/`. Choose one of the following package
layouts.

Source contribution:

```text
integrations/
  my-firmware/
    integration.json
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
integrations/
  my-firmware/
    integration.json
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
| `integration.json` | Card text, author, compatibility, build settings, and firmware versions |
| `README.md` | Firmware behavior, controls, setup, and hardware test record |
| `assets/logo.*` | Optional project identity asset |
| `assets/preview.*` | A real screenshot or photo of the firmware running on Sticky |
| `source/` | Complete source for a source contribution |
| `source/LICENSE` | License covering the locally submitted source |
| `firmware/<version>/manifest.json` | Flash layout, sizes, and SHA-256 values for firmware-only contributions |
| `firmware/<version>/*.bin` | Ready-to-install files for firmware-only contributions |

The directory name and `integration.json.id` use the same lowercase kebab-case
identifier, such as `weather-dashboard` or `sticky-2048`.

Images under `integrations/<integration-id>/assets/` may use `.png`, `.jpg`,
`.jpeg`, `.webp`, or static `.svg` files. The preview image is required; the
project logo is optional.

## Create a contribution

From the repository root:

```bash
cp -R integrations/_template integrations/my-firmware
```

Complete the files in this order:

1. Rename the directory and update `integration.json.id`.
2. Add the required author name, an optional author link, and an optional display source.
3. Add the project README, upstream source URL, and source license name.
4. Add an actual device preview under `assets/`, plus a project logo when needed.
5. For a source contribution, add `source/`, `build` metadata, and `sourceBuild: true`.
6. For a firmware-only contribution, add the tested package under `firmware/<version>/`.
7. Run the Registry tests and validator.
8. Flash the packaged firmware to a physical reTerminal Sticky.
9. Open a pull request with the test result.

## integration.json

Normal third-party submissions use `"group": "community"`,
`"catalogSection": "community"`, and `"mode": "flash"`.

```json
{
  "schemaVersion": 1,
  "id": "my-firmware",
  "name": "My Firmware",
  "group": "community",
  "catalogSection": "community",
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

### Catalog fields

| Field | Community contribution value |
|---|---|
| `group` | `community` |
| `catalogSection` | `community` |
| `mode` | `flash` |
| `status` | `experimental`, `beta`, or `stable` according to project maturity |
| `author.name` | Required author or team name shown on the website |
| `author.url` | Optional HTTPS link opened when the author name is selected |
| `origin.name` | Optional display source shown on the website |
| `origin.url` | Optional HTTPS link opened when the display source is selected |
| `source.url` | Upstream source repository for both contribution paths |
| `source.license` | Source license identifier, required for firmware-only contributions |
| `source.path` | Local source directory for the source path, normally `source` |
| `build.*` | Build metadata supplied together with `source.path` |
| `flash.versions[].sourceBuild` | Set to `true` when GitHub Actions builds the submitted source |
| `flash.versions[].manifestPath` | Local manifest used by a firmware-only contribution |

`author` and `origin` are website attribution fields. The `source` object keeps
the technical source repository, license, and local build path used during
review and packaging.

`official` and `platform` catalog sections are maintained through coordinated
Seeed or partner work. Normal community pull requests target the `community`
section. Maintainers may temporarily use `draft` for migrated entries that are
still waiting for a complete firmware package; draft entries are not published
to Sticky Playground.

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

## Source contribution requirements

For the source path, the `source/` directory must build independently
from the submitted files. It includes project build files, application code,
local components, dependency locks or manifests, default configuration, and the
applicable license.

Use placeholders or runtime setup for user-specific Wi-Fi credentials, API
keys, tokens, and passwords.

The first automated build path supports ESP-IDF projects. A typical source tree
contains:

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

Projects that need another build system should open an issue first so the
maintainers can add a reproducible CI build adapter before the firmware PR.

The `build.version` field selects the ESP-IDF version used by GitHub Actions.
Declare the version already used by the project, for example `v5.0.5`, `v5.3.2`,
or `latest`.

## Optional local ESP-IDF build

Authors may build locally before opening the PR. Install the ESP-IDF version
declared in `integration.json`, then run from the integration source directory:

```bash
idf.py set-target esp32s3
idf.py -D PROJECT_VER=1.0.0 build
```

This local build verifies the project before submission. The Registry ignores
the local `build/`, generated `sdkconfig`, dependency cache, and editor settings.
GitHub Actions performs the official build and packaging after the PR is opened.

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
- local source and ESP-IDF project files when `source.path` is present;
- image file signatures and static SVG safety;
- local manifest structure;
- firmware file existence, byte size, and SHA-256;
- non-overlapping flash address ranges;
- the direct-flash requirements for community catalog entries.

For source contributions, manifest and firmware-file checks run after GitHub
Actions builds the project. For firmware-only contributions, they run directly
against the files committed in the PR.

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
2. Build source-contribution ESP-IDF projects with each project-declared IDF version.
3. Generate the flash manifest and firmware package from ESP-IDF's flash map.
4. Upload the generated package as a temporary PR artifact for maintainer review.

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
4. Keep older firmware directories referenced by `integration.json`.
5. Run validation and repeat the physical device test.
6. Describe user-visible changes and upgrade behavior in the pull request.

## Pull request checklist

- [ ] One integration directory contains the complete contribution.
- [ ] `integration.json` uses `community` + `community` + `flash`.
- [ ] The contribution uses either source plus build metadata, or firmware-only plus an upstream source URL and license.
- [ ] The source path uses `sourceBuild: true`, or the firmware-only path includes the manifest and every required `.bin`.
- [ ] The README and PR identify the tested package origin and firmware version.
- [ ] `npm test` and `npm run validate` pass.
- [ ] The packaged firmware was tested on a physical reTerminal Sticky.
- [ ] The PR describes the hardware, firmware version, result, and build version when applicable.
