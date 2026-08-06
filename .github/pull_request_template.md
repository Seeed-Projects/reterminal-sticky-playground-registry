## Contribution

- Platform or project name:
- Integration ID:
- Firmware version:
- Upstream project:
- Source license:

### Contribution type

- [ ] New community integration
- [ ] Existing integration update

### Package type

- [ ] Source contribution built by GitHub Actions
- [ ] Firmware-only package

## What this contribution provides

Describe what Sticky users can do with this integration and what they receive at
the end of the flow. For an update, summarize the user-visible changes.

## Common verification

List the commands and manual checks completed for this contribution.

- [ ] `npm test` passes locally.
- [ ] `npm run validate` passes locally.
- [ ] The integration directory and `id` use the same lowercase kebab-case name.
- [ ] The integration README identifies the package origin and firmware version.
- [ ] The author, source, support, and documentation links are current.
- [ ] The logo and preview image are included, or the existing approved assets are reused unchanged.
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
- [ ] `source.url` points to the upstream source and `source.license` identifies its license.
- [ ] The README identifies the exact package origin, firmware version, and tested hardware.
- [ ] Every required `.bin` file is committed under `firmware/<version>/`.
- [ ] The local manifest records every firmware file, flash offset, byte size, and SHA-256.

## New community integration verification

Complete this section when **New community integration** is selected.

- [ ] Not applicable because **Existing integration update** is selected.
- [ ] `integration.json` uses `"group": "community"`, `"catalogSection": "community"`, and `"mode": "flash"`.
- [ ] The project metadata, assets, compatibility, installation notes, and support links are complete.

## Existing integration update verification

Complete this section when **Existing integration update** is selected.

- [ ] Not applicable because **New community integration** is selected.
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
