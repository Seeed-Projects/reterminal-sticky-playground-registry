# reTerminal Sticky Playground Registry

Public contribution repository for the reTerminal Sticky Playground. It holds
two kinds of content, published on two pages of the Sticky website:

| Content | Website page | Directory | Guide |
|---|---|---|---|
| Firmware users flash from the browser | [Firmware](https://www.seeedstudio.com/sticky/playground/firmware/) | `firmwares/` | [Contributing firmware](docs/contributing-firmware.md) |
| Community 3D printable cases, stands, mounts, and accessories | [3D Printables](https://www.seeedstudio.com/sticky/playground/3d-printables/) | `printables/` | [Contributing a 3D printable design](docs/contributing-printables.md) |

Start with [CONTRIBUTING.md](CONTRIBUTING.md) ([中文](CONTRIBUTING.zh-CN.md)),
which explains the shared rules, local checks, review, and release flow, then
follow the guide for your content type.

Printable designs have a shortcut: the **Submit with the form** button on the
[3D Printables page](https://www.seeedstudio.com/sticky/playground/3d-printables/)
collects the card text and the photo, then opens the pull request here for you.
A GitHub account is not required to use it.

## How publishing works

The Sticky website repository is private. It pins one reviewed commit of this
Registry, mirrors the verified firmware files, copies the preview images, and
generates the public catalog cards and browser flashing pages.

```text
Contributor PR
  -> firmware package or printable card + metadata + assets
  -> Registry validation (and source build for firmware)
  -> maintainer review and merge
  -> versioned firmware Release (source-built firmware)
  -> Sticky website pins the Registry commit
  -> local build and physical-device test
  -> Sticky website deployment
```

Firmware cards are direct-flash entries: users stay on the Sticky website and
install through the browser. Printable cards store metadata and one preview
photo here and link users to the author's download page.

Partner firmware is coordinated with the platform owner and uses official
project, documentation, support, and identity assets. Official Sticky firmware
is maintained by Seeed.

## Repository structure

```text
firmwares/
  _template/
  <firmware-id>/
    firmware.json
    README.md
    assets/
    source/                  source contributions
    firmware/<version>/      firmware-only contributions
      manifest.json
      *.bin
printables/
  _template/
  <design-id>/
    printable.json
    README.md
    assets/
      preview.jpg
schemas/
  firmware.schema.json
  printable.schema.json
scripts/
  validate-registry.mjs
  list-build-targets.mjs
  package-esp-idf.mjs
  package-platformio.mjs
  package-arduino.mjs
  verify-esp-idf-build.mjs
  verify-build-channel.mjs
examples/
  platformio-starter/
  arduino-starter/
docs/
  contributing-firmware.md / .zh-CN.md
  contributing-printables.md / .zh-CN.md
```

## Commands

Requires Node.js 20 or newer.

```bash
npm test
npm run validate
npm run create:manifest -- <firmware-id> <version>   # firmware-only packages
```

`npm run validate` checks every directory under `firmwares/` and `printables/`.
`npm run create:manifest` writes `firmware/<version>/manifest.json` from the
`.bin` files in that directory, filling in every size and SHA-256.
For source-built firmware, declare the project's build system in `firmware.json`
and set the newest version to `sourceBuild: true`; GitHub Actions builds the
project with ESP-IDF, PlatformIO, or Arduino, packages the toolchain's flash map,
and publishes the firmware Release after merge.
