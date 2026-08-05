# reTerminal Sticky Playground Registry

Public firmware contribution and review repository for the reTerminal Sticky
Playground.

The Sticky website repository remains private. Community developers contribute
complete buildable source, ready-to-flash firmware, metadata, and visual assets
here. The Sticky website pins a reviewed Registry commit, mirrors its verified
firmware files, and generates the public catalog cards and browser flashing
pages.

## Published community flow

```text
Contributor PR
  -> source + firmware + manifest + assets
  -> Registry validation and clean rebuild
  -> maintainer review and merge
  -> Sticky preview branch pins the Registry commit
  -> local page and physical-device flash test
  -> Sticky main and Kubernetes deployment
```

A normal community contribution is a direct-flash firmware entry. Users stay on
the Sticky website and install it through the browser.

## Repository structure

```text
integrations/
  _template/
  <firmware-id>/
    integration.json
    README.md
    assets/
    source/
    firmware/
      <version>/
        manifest.json
        *.bin
schemas/
  integration.schema.json
scripts/
  validate-registry.mjs
  package-esp-idf.mjs
  verify-esp-idf-build.mjs
```

## Commands

Requires Node.js 20 or newer.

```bash
npm test
npm run validate
```

For an ESP-IDF project that has already been built under its `source/`
directory:

```bash
npm run package:esp-idf -- <integration-id> <version>
```

This command packages the exact ESP-IDF flash map and generates the local
manifest used by the Sticky browser flasher.

## Contribution guides

- [English contribution guide](CONTRIBUTING.md)
- [中文贡献指南](CONTRIBUTING.zh-CN.md)

The guides define the required source tree, firmware package, metadata, local
validation, physical-device test, pull request checks, and post-merge Sticky
release flow.
