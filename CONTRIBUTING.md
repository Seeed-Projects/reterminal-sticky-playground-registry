# Contributing Firmware to reTerminal Sticky Playground

This repository is the public contribution and review layer for the reTerminal
Sticky firmware catalog. A completed community contribution becomes a card on
the Sticky Playground website and opens a Seeed-hosted browser flashing page.

Contributors submit the complete buildable source, firmware package, metadata,
and visual assets in one pull request. The private Sticky website reads only a
reviewed Registry commit and generates the public card and flashing page.

For the Chinese guide, see [CONTRIBUTING.zh-CN.md](CONTRIBUTING.zh-CN.md).

## Contribution result

A published community contribution provides this user flow:

1. The user opens Sticky Playground.
2. The user selects the community firmware card.
3. The user reviews the firmware version and installation notes on the Sticky website.
4. The user connects reTerminal Sticky by USB.
5. The user selects **Flash Now** to install the firmware in the browser.

The Registry stores the source and installable package. The Sticky website owns
the card layout, browser serial connection, flashing interface, domain, and
production deployment.

## Required pull request contents

Create one directory under `integrations/` and include all of these items:

```text
integrations/
  my-firmware/
    integration.json
    README.md
    assets/
      logo.svg
      preview.jpg
    source/
      CMakeLists.txt
      sdkconfig.defaults
      main/
      components/
      LICENSE
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
| `assets/logo.*` | Project identity shown by the catalog |
| `assets/preview.*` | A real screenshot or photo of the firmware running on Sticky |
| `source/` | Complete source needed to reproduce the submitted firmware package |
| `source/LICENSE` | License covering the contributed source |
| `firmware/<version>/manifest.json` | Flash layout, sizes, and SHA-256 values |
| `firmware/<version>/*.bin` | Ready-to-install firmware files built from `source/` |

The directory name and `integration.json.id` use the same lowercase kebab-case
identifier, such as `weather-dashboard` or `sticky-2048`.

## Create a contribution

From the repository root:

```bash
cp -R integrations/_template integrations/my-firmware
```

Then complete the files in this order:

1. Rename the directory and update `integration.json.id`.
2. Add the complete project under `source/`.
3. Add the source license and project README.
4. Add a logo and an actual device preview under `assets/`.
5. Build the project from the submitted `source/` directory.
6. Package the build output under `firmware/<version>/`.
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
    "logo": "assets/logo.svg",
    "preview": "assets/preview.jpg",
    "previewAlt": "My Firmware running on reTerminal Sticky"
  },
  "tags": ["dashboard", "offline"],
  "build": {
    "system": "esp-idf",
    "version": "5.4.2",
    "target": "esp32s3",
    "projectPath": "source"
  },
  "flash": {
    "versions": [
      {
        "version": "1.0.0",
        "channel": "experimental",
        "manifestPath": "firmware/1.0.0/manifest.json"
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
| `source.path` | Local source directory, normally `source` |
| `build.system` | Currently `esp-idf` |
| `build.version` | The exact ESP-IDF release, including patch version, such as `5.4.2` |
| `build.target` | `esp32s3` for reTerminal Sticky |
| `build.projectPath` | The ESP-IDF project directory, matching `source.path` |
| `flash.versions[].manifestPath` | Local manifest under the same integration directory |

`official` and `platform` catalog sections are maintained through coordinated
Seeed or partner work. Normal community pull requests target the `community`
section. Maintainers may temporarily use `draft` for migrated entries that are
still waiting for a complete firmware package; draft entries are not published
to Sticky Playground.

## Source requirements

The `source/` directory must build independently from the submitted files. It
includes project build files, application code, local components, dependency
locks or manifests, default configuration, and the applicable license.

Use placeholders or runtime setup for user-specific Wi-Fi credentials, API
keys, tokens, and passwords. The contributed source and firmware package must
represent the same release.

The first automated build path supports ESP-IDF projects. A typical source tree
contains:

```text
source/
  CMakeLists.txt
  sdkconfig.defaults  # includes CONFIG_APP_REPRODUCIBLE_BUILD=y
  main/
    CMakeLists.txt
    main.cpp
  components/
  LICENSE
```

Projects that need another build system should open an issue first so the
maintainers can add a reproducible CI build adapter before the firmware PR.

ESP-IDF community projects must enable `CONFIG_APP_REPRODUCIBLE_BUILD=y` in
`source/sdkconfig.defaults`. This removes build-time and local-path differences,
allowing CI to confirm that the submitted source produces the packaged firmware.

## Build and package ESP-IDF firmware

Install the ESP-IDF version declared in `integration.json`, then run from the
integration source directory:

```bash
idf.py set-target esp32s3
idf.py build
```

Return to the Registry root and package the exact ESP-IDF flash map:

```bash
npm run package:esp-idf -- my-firmware 1.0.0
```

The packaging command reads `source/build/flasher_args.json`, copies every
required `.bin` file, and creates `firmware/1.0.0/manifest.json` with the flash
offset, byte size, and SHA-256 for each file.

Commit the generated manifest and `.bin` files together with the source. This
gives reviewers one reproducible source release and one ready-to-flash package.

## Local validation

Install Node.js 20 or newer, then run:

```bash
npm test
npm run validate
```

The validator checks:

- directory and ID consistency;
- required metadata and HTTPS links;
- local source and ESP-IDF project files;
- the integration README and source license;
- image file signatures and static SVG safety;
- local manifest structure;
- firmware file existence, byte size, and SHA-256;
- every file and offset from the ESP-IDF flash map;
- non-overlapping flash address ranges;
- the direct-flash requirements for community catalog entries.

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

The pull request must contain the source, package, metadata, assets, and hardware
test result. GitHub Actions then performs three checks:

1. Validate the Registry structure and every local firmware package.
2. Rebuild each submitted ESP-IDF community project in a clean environment.
3. Compare the clean build output with every committed `.bin` and SHA-256.

The pull request remains under review until these checks pass and the maintainer
can verify the project purpose, license, compatibility, and hardware test.

## What happens after merge

Merging the Registry pull request does not immediately publish production. The
maintainer uses the reviewed release flow:

1. Merge the complete Registry pull request.
2. Update the pinned Registry commit on the Sticky preview branch.
3. Build Sticky locally and confirm the new card and flashing page.
4. Flash the firmware from the local Sticky page to a physical device.
5. Merge the tested Sticky preview branch into Sticky `main`.
6. Deploy the Sticky website through the company server and Kubernetes.

This separates contributor review, real-device acceptance, and production
release while keeping the private website repository closed.

## Updating an existing firmware

For a new version:

1. Update the source under `source/`.
2. Add the new version to `flash.versions` with the newest version first.
3. Build and run `npm run package:esp-idf -- <id> <version>`.
4. Keep older firmware directories referenced by `integration.json`.
5. Run validation and repeat the physical device test.
6. Describe user-visible changes and upgrade behavior in the pull request.

## Pull request checklist

- [ ] One integration directory contains the complete contribution.
- [ ] `integration.json` uses `community` + `community` + `flash`.
- [ ] `source/` contains a complete licensed buildable project.
- [ ] `firmware/<version>/` contains the manifest and every required `.bin`.
- [ ] The package was generated from the submitted source.
- [ ] `npm test` and `npm run validate` pass.
- [ ] The packaged firmware was tested on a physical reTerminal Sticky.
- [ ] The PR describes the hardware, build version, firmware version, and result.
