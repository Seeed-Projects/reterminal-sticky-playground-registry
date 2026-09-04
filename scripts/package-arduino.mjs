#!/usr/bin/env node

import { join } from 'node:path';

import {
  readArduinoFlashArgs,
  resolveTarget,
  writeFirmwarePackage,
} from './firmware-package.mjs';

const integrationId = process.argv[2];
const versionLabel = process.argv[3];

const { integration, projectDir, outputDir } = resolveTarget('arduino', integrationId, versionLabel);
const { files, flashSize } = readArduinoFlashArgs(join(projectDir, 'build'));
const parts = writeFirmwarePackage({
  integration,
  versionLabel,
  outputDir,
  files,
  flashSize,
});

console.log(`Packaged ${integrationId} ${versionLabel} with ${parts.length} firmware part(s).`);
