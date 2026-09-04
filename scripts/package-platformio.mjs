#!/usr/bin/env node

import { join } from 'node:path';

import {
  readFlasherArgsJson,
  resolveTarget,
  writeFirmwarePackage,
} from './firmware-package.mjs';

const integrationId = process.argv[2];
const versionLabel = process.argv[3];

const { integration, projectDir, outputDir } = resolveTarget('platformio', integrationId, versionLabel);
const buildDir = join(projectDir, '.pio', 'build', integration.build.environment);
const { files, flashSize } = readFlasherArgsJson(buildDir);
const parts = writeFirmwarePackage({
  integration,
  versionLabel,
  outputDir,
  files,
  flashSize,
});

console.log(`Packaged ${integrationId} ${versionLabel} with ${parts.length} firmware part(s).`);
