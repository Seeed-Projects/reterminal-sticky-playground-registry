# Contributing to the reTerminal Sticky Playground Registry

Thank you for helping expand the software and platform ecosystem around
reTerminal Sticky.

This repository is the public review and contribution layer for Sticky
Playground integrations. A contribution describes what a platform provides,
where its upstream project is maintained, how Sticky users enter the workflow,
and which resources the Playground may present.

The registry uses declarative files. Contributors provide metadata, images,
configuration fragments, release links, and firmware integrity information.
The private Sticky website remains responsible for page rendering, browser
flashing, domain configuration, deployment, and analytics.

For the Chinese guide, see [CONTRIBUTING.zh-CN.md](CONTRIBUTING.zh-CN.md).

## Start here

Every contribution follows the same high-level process:

1. Confirm that the project has a public upstream page and a support contact.
2. Choose one integration mode.
3. Fork this repository and create one integration directory.
4. Complete `integration.json` and add the required local resources.
5. Run `npm run validate`.
6. Open a pull request and describe the hardware and workflow you tested.
7. Respond to review feedback from the registry maintainers.

The four supported modes are:

| Mode | Best fit | Registry content | Playground result |
|---|---|---|---|
| `external` | The upstream project already provides an installer, browser toolbox, or complete documentation flow | Platform metadata and the maintained destination URL | The user continues in the upstream workflow |
| `template` | Each user needs a generated YAML, JSON, or other configuration file | Platform metadata and composable text fragments | The user previews, copies, or downloads a generated file |
| `download` | Users need a complete source project that they customize and build locally | Platform metadata, a versioned archive URL, and setup steps | The user downloads a project and follows a local build workflow |
| `flash` | A tested, ready-to-run firmware package can be installed without per-user source changes | Platform metadata and versioned manifest integrity information | The firmware becomes eligible for browser flashing after security review |

Choose the simplest mode that gives users a complete workflow. An upstream
installer is usually best represented by `external`; per-user configuration is
usually best represented by `template`; a customizable IDE project is usually
best represented by `download`; and a ready-to-run binary is represented by
`flash`.

## Contribution scope

The registry accepts:

- official platform integrations;
- community firmware and applications;
- links to upstream installers, toolboxes, and documentation;
- reusable configuration templates;
- versioned source-project downloads;
- versioned firmware manifests for browser-flash review;
- logos and screenshots that may be redistributed in this repository;
- updates to existing integration metadata and release information.

The website renderer, website routes, browser flashing implementation,
deployment credentials, domain settings, analytics, and internal release
configuration are maintained separately. A registry contribution can therefore
remain focused on one platform and does not require access to the private Sticky
website repository.

This repository is not a general binary archive. Firmware and source archives
should normally be published by the upstream project in an immutable release.
The registry records the reviewed location, version, and integrity information.

## Eligibility

A platform or project is ready for a registry pull request when:

- it has a public project, product, or documentation page;
- its author or maintaining team can be identified;
- users have a public support or issue-reporting destination;
- its reTerminal Sticky compatibility is tested or precisely described;
- the contribution can be expressed through one of the four supported modes;
- all submitted assets may be redistributed under their applicable licenses;
- user-specific credentials are represented by placeholders.

Community projects use `"group": "community"`.

The `"official"` group represents a coordinated workflow between Seeed Studio
and the upstream platform. If you are proposing a new official integration,
open an issue or contact the repository maintainers before preparing a large
submission. Maintainers will confirm the positioning, ownership, support path,
and release expectations.

## Repository structure

Production integrations live directly under `integrations/`:

```text
integrations/
  my-platform/
    integration.json
    assets/
      logo.svg
      preview.webp
    templates/
      header.yaml
      display.yaml
      sensor.yaml
```

The directory name is the stable integration ID. Use lowercase kebab-case:

```text
crosspoint-reader
my-dashboard
home-assistant-display
```

The same value must appear in `integration.json`:

```json
{
  "id": "my-platform"
}
```

Directories beginning with `_` are templates or examples. They are excluded
from production validation and registry publication.

## Create an integration

From the repository root:

```bash
cp -R integrations/_template integrations/my-platform
```

Then:

1. Rename the directory to the final integration ID.
2. Replace every placeholder in `integration.json`.
3. Keep only the object for the selected mode.
4. Add the logo and preview image under `assets/`.
5. Add template fragments under `templates/` when using template mode.
6. Run the validator.

```bash
npm run validate
```

The validator scans every production directory, so it also catches duplicate
IDs and invalid references across the whole registry.

## Common metadata

Every `integration.json` starts with the same common fields.

```json
{
  "schemaVersion": 1,
  "id": "my-platform",
  "name": "My Platform",
  "group": "community",
  "mode": "external",
  "status": "experimental",
  "summary": "Turn Sticky into a focused information display.",
  "description": "My Platform provides a managed information-display workflow for reTerminal Sticky, including content scheduling and device setup.",
  "author": {
    "name": "Project author or team",
    "url": "https://github.com/example"
  },
  "source": {
    "url": "https://github.com/example/my-platform",
    "license": "MIT"
  },
  "support": {
    "url": "https://github.com/example/my-platform/issues"
  },
  "documentationUrl": "https://github.com/example/my-platform#readme",
  "compatibility": {
    "devices": [
      "reterminal-sticky"
    ],
    "notes": "Tested on the production reTerminal Sticky hardware."
  },
  "assets": {
    "logo": "assets/logo.svg",
    "preview": "assets/preview.webp",
    "previewAlt": "My Platform dashboard shown on reTerminal Sticky"
  },
  "tags": [
    "dashboard",
    "productivity"
  ]
}
```

### Common field reference

| Field | Required | Description |
|---|---:|---|
| `schemaVersion` | Yes | Registry contract version. The current value is `1`. |
| `id` | Yes | Stable lowercase kebab-case identifier. It must match the directory name. |
| `name` | Yes | Public platform or project name, up to 80 characters. |
| `group` | Yes | `community` for normal third-party contributions or `official` for an approved official workflow. |
| `mode` | Yes | Exactly one of `external`, `template`, `download`, or `flash`. |
| `status` | Yes | `experimental`, `beta`, or `stable`. |
| `summary` | Yes | One short sentence, up to 140 characters. |
| `description` | Yes | A clear explanation of the user value, setup expectations, and main use case, up to 800 characters. |
| `author` | Yes | Project author or maintaining team, with a public HTTPS profile or organization URL. |
| `source` | Yes | Canonical upstream project URL and, when available, its license identifier. |
| `support` | Yes | Public issue tracker, support portal, or maintained discussion page. |
| `documentationUrl` | No | Direct setup or product documentation URL. |
| `compatibility` | Yes | Supported registry device IDs and concise hardware notes. |
| `assets` | Yes | Local logo, preview, and accessible preview description. |
| `tags` | No | Up to six short discovery tags. |

At schema version 1, the compatibility list is exactly:

```json
{
  "devices": [
    "reterminal-sticky"
  ]
}
```

Use `compatibility.notes` to identify a tested hardware revision, required
accessory, known product configuration, or other factual setup condition.

## Mode 1: external

Use `external` when the upstream project already owns the best installation or
configuration experience. Typical destinations include:

- an official browser installer;
- a firmware toolbox;
- a device onboarding application;
- a maintained board-support page;
- a complete upstream setup guide.

Add an `external` object:

```json
{
  "mode": "external",
  "external": {
    "label": "Open official toolbox",
    "url": "https://example.com/toolbox",
    "description": "Continue in the official toolbox to install firmware, configure the device, and manage content."
  }
}
```

### External field reference

| Field | Required | Description |
|---|---:|---|
| `external.label` | Yes | Short action label shown to users. |
| `external.url` | Yes | Maintained HTTPS destination. |
| `external.description` | Yes | What the user will do and receive at that destination. |

The linked workflow should contain enough information for a Sticky user to
continue without requiring undocumented private instructions.

## Mode 2: template

Use `template` when each user needs a configuration file assembled from
reusable parts. Suitable formats include YAML, JSON, TOML, and plain text.

Template fragments are stored inside the integration directory:

```text
integrations/my-platform/
  integration.json
  templates/
    header.yaml
    display.yaml
    sensor.yaml
    footer.yaml
```

Add a `template` object:

```json
{
  "mode": "template",
  "template": {
    "outputExtension": "yaml",
    "mimeType": "text/yaml",
    "fileNamePattern": "{integrationId}-{deviceId}",
    "headerPath": "templates/header.yaml",
    "footerPath": "templates/footer.yaml",
    "options": [
      {
        "id": "display",
        "label": "Sticky display",
        "description": "Adds the required reTerminal Sticky display configuration.",
        "path": "templates/display.yaml",
        "required": true,
        "defaultSelected": true
      },
      {
        "id": "sensor",
        "label": "Environmental sensor",
        "description": "Adds optional temperature and humidity sensor configuration.",
        "path": "templates/sensor.yaml",
        "required": false,
        "defaultSelected": true
      }
    ]
  }
}
```

The reserved filename placeholders are:

| Placeholder | Result |
|---|---|
| `{integrationId}` | The integration `id`. |
| `{deviceId}` | `reterminal-sticky`. |

### Template field reference

| Field | Required | Description |
|---|---:|---|
| `template.outputExtension` | Yes | Lowercase extension without a leading dot. |
| `template.mimeType` | Yes | MIME type for browser download. |
| `template.fileNamePattern` | Yes | Output filename without the extension. |
| `template.headerPath` | No | Local fragment always placed before selected options. |
| `template.footerPath` | No | Local fragment always placed after selected options. |
| `template.options` | Yes | One or more selectable template fragments. |
| `option.id` | Yes | Stable lowercase kebab-case option ID. |
| `option.label` | Yes | User-facing option name. |
| `option.description` | Yes | Clear explanation of what the fragment enables. |
| `option.path` | Yes | Local file path inside the integration directory. |
| `option.required` | No | `true` when the output is invalid without this fragment. |
| `option.defaultSelected` | No | Initial selection state in the Playground. |

Template content should:

- use documented placeholders for Wi-Fi credentials, tokens, account IDs, and
  other user-specific values;
- contain valid syntax when the default options are combined;
- keep hardware values aligned with tested reTerminal Sticky behavior;
- include comments only when they help users customize the generated file;
- remain plain text and reviewable in the pull request.

Include the fully assembled default output in the pull-request description or
as a review attachment. This lets reviewers validate the actual result rather
than inspecting fragments independently.

## Mode 3: download

Use `download` when users need a complete source project that they will edit,
compile, and flash with another tool such as PlatformIO, ESP-IDF, Arduino IDE,
or a platform-specific editor.

Publish the archive from the upstream project. A versioned GitHub Release asset
is preferred because it provides a stable project owner, release page, and
immutable version context.

Add a `download` object:

```json
{
  "mode": "download",
  "download": {
    "url": "https://github.com/example/my-platform/releases/download/v1.2.0/my-platform-sticky.zip",
    "version": "1.2.0",
    "fileName": "my-platform-sticky.zip",
    "sha256": "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef",
    "steps": [
      {
        "title": "Download the project",
        "description": "Download and extract the versioned Sticky project archive."
      },
      {
        "title": "Customize the configuration",
        "description": "Open the project in PlatformIO and fill in the documented local configuration values."
      },
      {
        "title": "Build and upload",
        "description": "Select the reTerminal Sticky environment, build the project, and upload it over USB."
      }
    ]
  }
}
```

### Download field reference

| Field | Required | Description |
|---|---:|---|
| `download.url` | Yes | HTTPS URL for a versioned source-project archive. |
| `download.version` | No | Human-readable upstream release version. |
| `download.fileName` | No | Suggested archive filename. |
| `download.sha256` | No | Lowercase SHA-256 of the archive. Strongly recommended for fixed assets. |
| `download.steps` | Yes | One to twelve ordered setup steps. |

Each step contains a short `title` and a complete `description`. Steps should
cover the required tool, project opening method, user configuration point,
correct Sticky build target, and upload method.

The archive should include its own README, license information, dependency
versions, and build configuration. The registry steps are a concise handoff,
while the upstream project remains the detailed technical source of truth.

## Mode 4: flash

Use `flash` for firmware that is already compiled, tested on reTerminal Sticky,
and ready to run without user-specific source changes.

Flash mode receives additional review because browser flashing modifies the
device. A contribution records immutable release provenance and integrity
metadata; maintainers separately decide when a version is eligible for website
publication.

Add a `flash` object:

```json
{
  "mode": "flash",
  "flash": {
    "versions": [
      {
        "version": "1.2.0",
        "channel": "stable",
        "manifestUrl": "https://raw.githubusercontent.com/example/my-platform/0123456789abcdef0123456789abcdef01234567/releases/sticky/1.2.0/manifest.json",
        "manifestSha256": "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef",
        "releaseUrl": "https://github.com/example/my-platform/releases/tag/v1.2.0"
      }
    ],
    "notes": [
      {
        "title": "Initial setup",
        "description": "After flashing, hold the power button for three seconds and follow the on-device Wi-Fi setup flow."
      }
    ]
  }
}
```

### Flash field reference

| Field | Required | Description |
|---|---:|---|
| `flash.versions` | Yes | One or more reviewed firmware versions. |
| `version.version` | Yes | Exact upstream release version. |
| `version.channel` | Yes | `experimental`, `beta`, or `stable`. |
| `version.manifestUrl` | Yes | Immutable HTTPS URL for the firmware manifest. |
| `version.manifestSha256` | Yes | Lowercase SHA-256 of the exact manifest bytes. |
| `version.releaseUrl` | Yes | Public upstream release page with release context and notes. |
| `flash.notes` | No | One to twelve ordered setup or post-flash notes. |

### Immutable manifest URLs

Use a URL tied to a release tag, release asset, or full commit SHA. Examples:

```text
https://github.com/owner/project/releases/download/v1.2.0/manifest.json
https://raw.githubusercontent.com/owner/project/<full-commit-sha>/path/manifest.json
```

The registry review is based on exact bytes. A branch URL such as `main`, a
moving `latest` URL, or a file that can be replaced in place does not provide
that guarantee.

### Manifest contract

A flash manifest should identify the package and every region written to the
device. The expected shape is:

```json
{
  "name": "My Platform for reTerminal Sticky",
  "version": "1.2.0",
  "builds": [
    {
      "chipFamily": "ESP32-S3",
      "parts": [
        {
          "path": "bootloader.bin",
          "offset": 0,
          "size": 24576,
          "sha256": "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
        },
        {
          "path": "partitions.bin",
          "offset": 32768,
          "size": 3072,
          "sha256": "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
        },
        {
          "path": "firmware.bin",
          "offset": 65536,
          "size": 1048576,
          "sha256": "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
        }
      ]
    }
  ]
}
```

Offsets and filenames above are examples, not universal values. Use the exact
flash layout produced and tested by the upstream project.

Every part must provide:

- a relative HTTPS-resolvable path;
- the exact integer flash offset;
- the exact file size in bytes;
- the lowercase SHA-256 of the binary.

The release should also document:

- the reTerminal Sticky hardware used for testing;
- flash size and partition layout;
- whether a full-chip erase is required;
- first-boot behavior;
- recovery or return-to-official-firmware instructions;
- known limitations and required external services.

### Generate hashes

On macOS:

```bash
shasum -a 256 manifest.json
shasum -a 256 firmware.bin
```

On Linux:

```bash
sha256sum manifest.json
sha256sum firmware.bin
```

Record the hash of the final uploaded release file. Rebuilding, reformatting, or
re-uploading a file changes its bytes and requires a new integrity value.

## Assets

Every integration includes:

```text
assets/
  logo.svg
  preview.webp
```

Accepted formats are:

- `.svg`
- `.png`
- `.jpg`
- `.jpeg`
- `.webp`

Limits enforced by the validator:

| Asset | Maximum size |
|---|---:|
| Logo | 1 MB |
| Preview | 5 MB |

Asset guidance:

- use the upstream project's current logo;
- use a preview that shows the actual platform, interface, or result;
- provide enough contrast for the logo to remain recognizable;
- use lowercase ASCII filenames;
- keep SVG files static and self-contained;
- provide meaningful `previewAlt` text describing what the image shows;
- confirm that the asset license or project permission allows redistribution.

The preview should help users understand the integration rather than act as a
generic decorative background.

## Links and ownership

All registry URLs use HTTPS.

Use:

- the canonical upstream project as `source.url`;
- the author or organization profile as `author.url`;
- the best public issue tracker or support portal as `support.url`;
- the most direct setup guide as `documentationUrl`;
- immutable version URLs for downloadable or flashable release artifacts.

Automated validation checks URL syntax. Maintainers also open important links
during review because availability, ownership, and destination content require
human confirmation.

If a platform changes ownership, moves repositories, or retires a workflow,
submit an update to the same integration directory so attribution and support
continue to point to the responsible project.

## Credentials and personal data

Registry content is public. Use placeholders for:

- Wi-Fi names and passwords;
- API keys and access tokens;
- account IDs;
- private server addresses;
- certificates and private keys;
- personal email addresses or device identifiers.

Suitable placeholders include:

```text
<YOUR_WIFI_SSID>
<YOUR_WIFI_PASSWORD>
<YOUR_API_KEY>
```

Template outputs may ask the user to fill values locally. Those values are not
stored in this registry.

## Local validation

### Prerequisite

Install Node.js 20 or newer:

```bash
node --version
```

Expected output begins with `v20` or a newer major version.

### Run the registry check

From the repository root:

```bash
npm test
npm run validate
```

`npm test` runs the validator regression suite in temporary directories.

Success:

```text
Registry validation passed (1 integration(s)).
```

Failure:

```text
Registry validation failed with 2 error(s):
- integrations/my-platform/integration.json.id: must match the directory name "my-platform"
- integrations/my-platform/integration.json.assets.preview: references a missing file: assets/preview.webp
```

The validator reports all issues it can find in one run. Correct the listed
paths and run the command again.

### What the validator checks

- `schemas/integration.schema.json` contains valid JSON;
- each production integration has `integration.json`;
- common and mode-specific fields use supported names;
- directory names and IDs match;
- IDs, tags, template options, and firmware versions are unique;
- group, mode, status, and channel values are supported;
- public links are valid HTTPS URLs;
- local resource paths stay inside the integration directory;
- referenced assets and template fragments exist;
- image contents match their extensions;
- SVG assets contain static graphics without scripts or external resources;
- asset formats and file sizes meet registry limits;
- exactly one mode-specific object is present;
- download and flash hashes use lowercase SHA-256 format.

The local validator does not compile upstream firmware, execute contributed
project scripts, or certify hardware behavior. Pull-request evidence and
maintainer review cover those areas.

## Pull request preparation

Keep one new platform or one focused update in each pull request. This gives the
project owner, reviewers, test evidence, and future history one clear boundary.

Before opening the pull request:

- run `npm run validate`;
- review the complete diff;
- open every submitted public link;
- verify image attribution and redistribution permission;
- assemble and parse the default output for template mode;
- download and inspect the archive for download mode;
- test the exact release and flash layout for flash mode;
- remove generated build folders, editor settings, local credentials, and
  unrelated files from the contribution.

The pull-request template asks for:

- platform name and integration ID;
- selected mode;
- upstream source;
- tested Sticky hardware;
- user-facing result;
- verification performed;
- mode-specific confirmation.

## Review criteria

Maintainers evaluate:

- clear benefit for reTerminal Sticky users;
- accurate project ownership and attribution;
- an active support path;
- a mode that matches the actual user workflow;
- factual compatibility information;
- concise and understandable user-facing copy;
- usable and legally redistributable assets;
- stable upstream release links;
- appropriate handling of user credentials;
- reproducible firmware integrity information for flash mode;
- long-term maintenance expectations.

Approval of a pull request means the registry metadata is accepted. Publication
on the Sticky Playground may follow separately after website compatibility,
content review, release scheduling, and, for flash mode, device validation.

## Updating an existing integration

Edit the existing directory when:

- publishing a new upstream version;
- changing a destination or support URL;
- improving descriptions or screenshots;
- adding or revising template fragments;
- changing compatibility notes;
- moving from `experimental` to `beta` or `stable`;
- changing mode because the upstream user workflow has materially changed.

For flash mode, retain previous version entries when their immutable artifacts
remain available and supported. Add the new version as a separate object. If an
old release is no longer safe or available, explain its removal in the pull
request.

Use the existing stable `id` when the platform name or branding changes. A new
ID is appropriate only when the contribution represents a genuinely different
project or user workflow.

## Removing or retiring an integration

Open a focused pull request when an integration has been retired, transferred,
made unavailable, or can no longer provide a supported Sticky workflow.

Include:

- the upstream retirement or transfer reference;
- the last known working version;
- any replacement project or migration path;
- whether previously published firmware should remain available for recovery.

Maintainers will coordinate registry removal with any corresponding Playground
entry.

## What happens after merge

After merge:

1. The accepted files become part of the public registry history.
2. The validation workflow runs again on `main`.
3. Maintainers may include the integration in a generated Playground catalog.
4. The private website may consume an approved registry revision during a
   separate build or release.
5. Website rollout may add additional presentation copy or operational checks
   without changing upstream ownership.

There is currently no automatic deployment from this registry to the private
Sticky website. The repository first establishes a stable public contribution
contract; website synchronization will be introduced separately.

## Maintainer responsibility and upstream responsibility

Registry maintainers are responsible for:

- the metadata contract and validation workflow;
- registry review and classification;
- private website rendering and deployment;
- final browser-flash enablement;
- removal of entries that no longer meet registry requirements.

Upstream integration maintainers are responsible for:

- project source and licensing;
- release artifacts and release notes;
- platform-specific documentation;
- user support and issue handling;
- notifying this registry when URLs, compatibility, ownership, or releases
  change.

This division lets third-party projects keep control of their software while the
Sticky Playground maintains a consistent and reviewable user experience.

## Getting help

Open a GitHub issue before preparing a large contribution when:

- you are unsure which mode fits;
- you want to propose an official integration;
- your firmware uses an unusual flash layout;
- one integration needs multiple independent user workflows;
- the current schema cannot represent a required field;
- you need clarification about assets or release integrity.

Include the upstream project URL and a short explanation of the intended Sticky
workflow. Maintainers can then confirm the smallest suitable contribution shape
before you invest in implementation.
