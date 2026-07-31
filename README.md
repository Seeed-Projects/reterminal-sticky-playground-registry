# reTerminal Sticky Playground Registry

The reTerminal Sticky Playground Registry is the public contribution entry point
for platforms, firmware projects, configuration templates, source-project
downloads, and official tools that support reTerminal Sticky.

This repository contains declarative integration metadata and contributor-owned
resources. The reTerminal Sticky website, domain configuration, deployment
configuration, analytics, and private application source remain in their
respective internal repositories.

## Contributing

- [English contribution guide](CONTRIBUTING.md)
- [中文贡献指南](CONTRIBUTING.zh-CN.md)

Every contribution starts by choosing one integration mode:

| Mode | Contributor provides | User receives |
|---|---|---|
| `external` | An upstream installer, toolbox, or documentation link | A direct handoff to the platform's maintained workflow |
| `template` | Declarative configuration fragments | A generated YAML, JSON, or other configuration file |
| `download` | A versioned source-project archive and setup steps | A project to customize and build locally |
| `flash` | A versioned firmware manifest with integrity metadata | A firmware package eligible for browser flashing after review |

The default group for third-party contributions is `community`. The `official`
group is used for integrations that Seeed Studio and the upstream platform have
agreed to present as an official workflow.

## Repository layout

```text
.
├── integrations/
│   ├── _template/
│   │   └── integration.json
│   └── <integration-id>/
│       ├── integration.json
│       ├── assets/
│       │   ├── logo.svg
│       │   └── preview.webp
│       └── templates/            # Template mode only
├── schemas/
│   └── integration.schema.json
├── scripts/
│   └── validate-registry.mjs
└── .github/
    ├── pull_request_template.md
    └── workflows/
        └── validate-registry.yml
```

Each production directory directly under `integrations/` represents one
platform or project. Its directory name and the `id` in `integration.json` must
match. Directories beginning with `_` are examples or templates and are not
published as production registry entries.

## Contribution lifecycle

1. A contributor forks this repository.
2. The contributor copies `integrations/_template/` to a new integration
   directory.
3. The contributor selects one mode and completes its metadata and resources.
4. `npm run validate` checks the contribution locally.
5. A pull request runs the same read-only validation in GitHub Actions.
6. Maintainers review attribution, compatibility, user experience, and, for
   flash mode, firmware integrity and release provenance.
7. An accepted registry entry becomes eligible for a future Sticky Playground
   catalog release.

Merging a registry entry records approval of its metadata. It does not guarantee
immediate publication on the reTerminal Sticky website. Website ingestion and
rollout remain a separate maintainer-controlled process.

## Trust and safety

Registry entries are data, not executable website extensions. Integration
folders contain metadata, approved media, configuration fragments, and links to
upstream resources. Rendering logic, browser flashing logic, deployment
credentials, and website configuration remain maintainer-owned.

Flash-mode submissions receive additional review because they can modify a
user's device. Every firmware version therefore identifies a versioned release
page, an HTTPS manifest URL, and the exact SHA-256 of that manifest. The
manifest is expected to provide the size, flash offset, and SHA-256 of every
firmware file it references.

## Local validation

Node.js 20 or newer is required. The registry has no third-party runtime or
development dependencies.

```bash
npm test
npm run validate
```

`npm test` exercises all four modes and representative failure cases. A new
checkout containing only the contribution template should report:

```text
Registry validation passed (0 integration(s)).
```

The validator checks production integration folders, required fields, supported
mode values, IDs, HTTPS URLs, local file references, asset limits, duplicate
entries, image file signatures, static SVG safety, and flash-manifest integrity
metadata.

## Project status

This repository defines the public contribution contract, validation workflow,
and versioned integration catalog. The initial catalog contains:

- reTerminal Sticky Official Firmware, versions 0.2.1 through 1.0.1;
- CrossPoint Reader, Sticky RC2.

The private reTerminal Sticky website consumes a maintainer-selected Registry
commit. Registry review and website publication remain separate approval steps,
so a merged contribution can be tested before it appears on the production
website.

## License

Repository content is available under the [MIT License](LICENSE). Assets,
templates, source projects, and firmware referenced from upstream projects
remain subject to their respective licenses.
