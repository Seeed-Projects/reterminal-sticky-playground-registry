# reTerminal Sticky Playground Registry

Public firmware contribution and review repository for the reTerminal Sticky
Playground.

The Sticky website repository remains private. Community developers contribute
metadata, visual assets, and either complete buildable source or a verified
firmware package here. GitHub Actions builds source contributions and publishes
their installable files. The Sticky website pins a reviewed Registry commit,
mirrors its verified firmware files, and generates the public catalog cards and
browser flashing pages.

## Published community flow

```text
Contributor PR
  -> source project or firmware-only package + metadata + assets
  -> Registry validation and source build
  -> PR artifact for review
  -> maintainer review and merge
  -> versioned firmware Release
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
    source/  # source contributions
    firmware/  # firmware-only contributions
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

For source contributions, declare the project's ESP-IDF version in
`integration.json` and set the newest firmware version to `sourceBuild: true`.
GitHub Actions builds the project, packages ESP-IDF's flash map, and publishes
the firmware after merge.

## Contribution guides

- [English contribution guide](CONTRIBUTING.md)
- [中文贡献指南](CONTRIBUTING.zh-CN.md)

The guides define the source and firmware-only contribution paths,
firmware package, metadata, local validation, physical-device test, pull request
checks, and post-merge Sticky release flow.
