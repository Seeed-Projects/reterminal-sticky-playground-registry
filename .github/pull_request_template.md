## Contribution

- Platform or project name:
- Integration ID:
- Firmware version:
- Firmware artifact origin:
- Firmware artifact SHA-256:
- Upstream project:
- Source license (community or partner firmware-only contributions):

### Contribution type

- [ ] New community integration
- [ ] New 3D printable design
- [ ] New partner integration coordinated with the platform owner
- [ ] Official Sticky firmware update maintained by Seeed
- [ ] Existing community or partner integration update

### Package type

- [ ] Source contribution built by GitHub Actions
- [ ] Firmware-only package
- [ ] 3D printable design with an external download link

## What this contribution provides

Describe what Sticky users can do with this integration and what they receive at
the end of the flow. For an update, summarize the user-visible changes.

## Common verification

List the commands and manual checks completed for this contribution.

- [ ] `npm test` passes locally.
- [ ] `npm run validate` passes locally.
- [ ] The integration directory and `id` use the same lowercase kebab-case name.
- [ ] The integration README identifies the package origin and firmware version.
- [ ] Community author attribution and all provided official project, support, and documentation links are current.
- [ ] Community entries include a real Sticky preview; partner entries use the official logo as their identity artwork.
- [ ] The submitted files contain no user-specific credentials, API keys, tokens, passwords, or private keys.
- [ ] The selected package type has all required source or firmware files.

## Source contribution verification

Complete this section when **Source contribution built by GitHub Actions** is selected.

- [ ] Not applicable because **Firmware-only package** is selected.
- [ ] `source.path` and `build` are both present and identify the tested source tree, build system, version, and target.
- [ ] The local source includes its license and every dependency needed for a clean build.
- [ ] The newest firmware version sets `sourceBuild: true`.
- [ ] The GitHub Actions source build passes and provides the firmware artifact.

## Firmware-only verification

Complete this section when **Firmware-only package** is selected.

- [ ] Not applicable because **Source contribution built by GitHub Actions** is selected.
- [ ] `source.url` points to the maintained upstream project.
- [ ] Community and partner packages identify their source license; official updates identify the official artifact origin in the dedicated section.
- [ ] The README identifies the exact package origin, firmware version, and tested hardware.
- [ ] Every required `.bin` file is committed under `firmware/<version>/`.
- [ ] The local manifest records every firmware file, flash offset, byte size, and SHA-256.

## New community integration verification

Complete this section when **New community integration** is selected.

- [ ] Not applicable because a new partner, official firmware, or existing integration update is selected.
- [ ] `integration.json` uses `"group": "community"`, `"catalogSection": "community"`, `"mode": "flash"`, and a documented `category`.
- [ ] The project metadata, assets, compatibility, installation notes, and support links are complete.

## New 3D printable design verification

Complete this section when **New 3D printable design** is selected.

- [ ] Not applicable because this is a firmware contribution or an existing integration update.
- [ ] `integration.json` uses `"group": "community"`, `"catalogSection": "printables"`, and `"mode": "external"`.
- [ ] `external.url` points to the author's Printables, MakerWorld, Thingiverse, or GitHub page.
- [ ] The preview is a real photo of the printed design on reTerminal Sticky.
- [ ] Printable files are not committed to this repository.

## New partner integration verification

Complete this section when **New partner integration coordinated with the platform owner** is selected.

- [ ] Not applicable because this is a community, official firmware, or existing integration update.
- [ ] `integration.json` uses `"group": "partner"`, `"catalogSection": "platform"`, `"mode": "flash"`, and a documented `category`.
- [ ] Project, source, documentation, and support links point to official platform resources.
- [ ] The platform metadata, official logo, compatibility, installation notes, and firmware package are complete.

## Official Sticky firmware update verification

Complete this section when **Official Sticky firmware update maintained by Seeed** is selected.

- [ ] Not applicable because this is a community or partner contribution.
- [ ] `sticky-factory` retains `"group": "official"`, `"catalogSection": "official"`, and `"mode": "flash"`.
- [ ] The newest version appears first and uses `firmware/<version>/manifest.json` through `manifestPath`.
- [ ] The version directory contains the complete official binary package used for physical-device testing.
- [ ] The README and pull request record the official artifact origin, byte size, SHA-256, device, chip, and flash offset.
- [ ] Existing Release-backed versions remain available through their current `manifestUrl`, `manifestSha256`, and `releaseUrl` records.
- [ ] The committed version directory is the delivery record for the new official version.

## Existing community or partner integration update verification

Complete this section when **Existing community or partner integration update** is selected.

- [ ] Not applicable because a new community, new partner, or official firmware contribution is selected.
- [ ] The existing `group`, `catalogSection`, and `mode` are retained unless this PR intentionally changes them.
- [ ] The newest firmware version appears first in `flash.versions`.
- [ ] Every older version still referenced by `integration.json` remains available.
- [ ] Existing metadata and assets were reviewed and updated only where the new version changes them.

## Physical-device test

- Device and hardware revision:
- Installation method:
- Tested firmware version:
- Main workflow tested:
- Reboot and saved-state result:
- USB reconnection and repeated-install result:

- [ ] The submitted firmware-only package or a local build of the submitted source was installed on a physical reTerminal Sticky.
- [ ] First boot and the main user workflow passed.
- [ ] Touch and hardware buttons used by the firmware passed.
- [ ] Reboot, saved state, USB reconnection, and repeated installation were tested where applicable.

## Additional context

Add any package provenance, compatibility, rollout, support, or review context.
