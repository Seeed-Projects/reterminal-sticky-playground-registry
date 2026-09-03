## Contribution type

Select one and complete only the matching section below.

- [ ] Firmware: new community firmware
- [ ] Firmware: new partner firmware coordinated with the platform owner
- [ ] Firmware: official Sticky firmware update maintained by Seeed
- [ ] Firmware: update to an existing firmware
- [ ] 3D printable design: new design or update

## Common verification

- [ ] `npm test` passes locally.
- [ ] `npm run validate` passes locally.
- [ ] The directory name and the `id` in the metadata file are the same lowercase kebab-case identifier.
- [ ] All links use HTTPS and open the intended page.
- [ ] The submitted files contain no user-specific credentials, API keys, tokens, passwords, or private keys.

---

## 3D printable design

Complete this section for **3D printable design**. Skip the firmware sections.

- Design name:
- Directory: `printables/`
- Download page:
- Platform (Printables, MakerWorld, Thingiverse, GrabCAD, GitHub, other):
- Category (`case`, `stand`, `mount`, `accessory`, `reference`):

- [ ] `printable.json` follows [docs/contributing-printables.md](https://github.com/Seeed-Projects/reterminal-sticky-playground-registry/blob/main/docs/contributing-printables.md).
- [ ] `download.url` is the author's public page that hosts the files; no model files are committed here.
- [ ] `author.name` credits the designer, and `download.license` matches the download page when a license is shown.
- [ ] `assets/preview.jpg` is a real photo of the printed design fitted on reTerminal Sticky.
- [ ] The README lists print settings and assembly notes.

---

## Firmware

Complete the fields and sections below for any **Firmware** contribution type.

- Firmware name:
- Directory: `firmwares/`
- Firmware version:
- Upstream project:
- Source license (community and partner firmware-only packages):
- Firmware artifact origin and SHA-256 (firmware-only packages):

### Package type

- [ ] Source contribution built by GitHub Actions
- [ ] Firmware-only package

### What this firmware provides

Describe what Sticky users can do with this firmware. For an update, summarize
the user-visible changes.

### Firmware verification

- [ ] `firmware.json` uses the `group`, `catalogSection`, `mode`, and `category` documented in [docs/contributing-firmware.md](https://github.com/Seeed-Projects/reterminal-sticky-playground-registry/blob/main/docs/contributing-firmware.md) for the selected type.
- [ ] The README identifies the package origin, firmware version, and tested hardware.
- [ ] Community entries include author attribution and a real Sticky preview; partner entries use the official logo as identity artwork.

Source contribution:

- [ ] `source.path` and `build` identify the tested source tree, build system, version, and target.
- [ ] The local source includes its license and every dependency needed for a clean build.
- [ ] The newest firmware version sets `sourceBuild: true`.
- [ ] The GitHub Actions source build passes and provides the firmware artifact.

Firmware-only package:

- [ ] `source.url` points to the maintained upstream project and `source.license` names its license.
- [ ] Every required `.bin` file is committed under `firmware/<version>/`.
- [ ] The local manifest records every firmware file, flash offset, byte size, and SHA-256.

Official Sticky firmware update:

- [ ] `sticky-factory` retains `"group": "official"`, `"catalogSection": "official"`, and `"mode": "flash"`.
- [ ] The newest version appears first and uses `firmware/<version>/manifest.json` through `manifestPath`.
- [ ] Existing Release-backed versions keep their `manifestUrl`, `manifestSha256`, and `releaseUrl` records.
- [ ] The README and this PR record the official artifact origin, byte size, SHA-256, device, chip, and flash offset.

Update to an existing firmware:

- [ ] The newest version appears first in `flash.versions`.
- [ ] Every older version still referenced by `firmware.json` remains available.
- [ ] Existing metadata and assets were updated only where the new version changes them.

### Physical-device test

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

---

## Additional context

Add any provenance, compatibility, rollout, support, or review context.
