## Integration

- Platform or project name:
- Integration ID:
- Firmware version:
- Upstream project:
- Tested reTerminal Sticky hardware:

## What this contribution provides

Describe what Sticky users can do with this integration and what they receive at
the end of the flow.

## Verification

Describe the checks you performed. Include the tested firmware or upstream
release version when applicable.

- [ ] `npm run validate` passes locally.
- [ ] The integration directory and `id` use the same lowercase kebab-case name.
- [ ] The contribution uses either complete source plus build metadata, or firmware-only plus an upstream source URL and license.
- [ ] The integration README identifies the tested package origin and firmware version.
- [ ] The project author, upstream source, support, and documentation links are current.
- [ ] The logo and preview image are included and may be redistributed here.
- [ ] Compatibility has been tested on reTerminal Sticky or clearly documented.
- [ ] User-specific credentials are represented by placeholders.

## Firmware package confirmation

- [ ] `integration.json` uses `"catalogSection": "community"` and `"mode": "flash"`.
- [ ] Complete-source contributions identify the tested build version, target, and source path.
- [ ] Complete-source ESP-IDF contributions enable `CONFIG_APP_REPRODUCIBLE_BUILD=y`.
- [ ] The local manifest records every firmware file, flash offset, size, and SHA-256.
- [ ] I installed this package on a physical reTerminal Sticky and tested its main workflow.

## Maintainer notes

Add any rollout, support, or compatibility context that reviewers should know.
