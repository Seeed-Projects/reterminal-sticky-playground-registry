# reTerminal Sticky Playground Registry

Public firmware and 3D printable contribution repository for official, partner,
and community integrations in the reTerminal Sticky Playground.

The Sticky website repository remains private. Community developers contribute
metadata, visual assets, and either complete buildable source or a verified
firmware package here. GitHub Actions builds source contributions and publishes
their installable files. The Sticky website pins a reviewed Registry commit,
mirrors its verified firmware files, and generates the public catalog cards and
browser flashing pages.

Partner integrations are coordinated with the platform owner and use official
project, documentation, support, and identity assets. Their cards can use the
official logo as the catalog and installation artwork. Author attribution is
optional because the platform identity is already represented by the
integration name and official links.

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

A normal community firmware contribution is a direct-flash entry. Users stay on
the Sticky website and install it through the browser. A 3D printable
contribution stores metadata and a preview photo here, then links users to the
author's download page.

## Repository structure

```text
integrations/
  _template/
  _template_printables/
  <firmware-id>/
    integration.json
    README.md
    assets/
    source/  # source contributions
    firmware/  # firmware-only contributions
      <version>/
        manifest.json
        *.bin
  <printable-id>/
    integration.json
    README.md
    assets/
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

The guides define the source, firmware-only, and 3D printable contribution
paths, firmware package, metadata, local validation, physical-device test, pull
request checks, and post-merge Sticky release flow.
