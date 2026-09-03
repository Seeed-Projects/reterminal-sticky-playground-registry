#!/usr/bin/env node

import { createHash } from 'node:crypto';
import {
  copyFileSync,
  mkdirSync,
  readFileSync,
  writeFileSync,
} from 'node:fs';
import { basename, dirname, join, resolve } from 'node:path';

const ROOT_DIR = process.env.REGISTRY_ROOT
  ? resolve(process.env.REGISTRY_ROOT)
  : resolve(import.meta.dirname, '..');
const integrationId = process.argv[2];
const versionLabel = process.argv[3];

if (!integrationId || !versionLabel) {
  throw new Error('Usage: npm run package:esp-idf -- <firmware-id> <version>');
}

const integrationDir = join(ROOT_DIR, 'firmwares', integrationId);
const integrationPath = join(integrationDir, 'firmware.json');
const integration = JSON.parse(readFileSync(integrationPath, 'utf8'));
if (integration.id !== integrationId || integration.build?.system !== 'esp-idf') {
  throw new Error(`${integrationId} is not configured as an ESP-IDF firmware`);
}

const version = integration.flash?.versions?.find((entry) => entry.version === versionLabel);
if (!version) {
  throw new Error(`${integrationId} does not define firmware version ${versionLabel}`);
}
if (!version.manifestPath && version.sourceBuild !== true) {
  throw new Error(`${integrationId} ${versionLabel} does not define a package destination`);
}

const projectDir = join(integrationDir, integration.build.projectPath);
const buildDir = join(projectDir, 'build');
const flashArgs = JSON.parse(readFileSync(join(buildDir, 'flasher_args.json'), 'utf8'));
const configuredOutputDir = process.env.FIRMWARE_OUTPUT_DIR;
if (version.sourceBuild === true && !configuredOutputDir) {
  throw new Error('FIRMWARE_OUTPUT_DIR is required for a source-built firmware package');
}
const outputDir = configuredOutputDir
  ? resolve(configuredOutputDir)
  : join(integrationDir, dirname(version.manifestPath));
mkdirSync(outputDir, { recursive: true });
const packagedNames = new Set();

// Packages the exact ESP-IDF flash map into the Registry manifest format.
// 将 ESP-IDF 的准确烧录布局整理为 Registry manifest 格式。
const parts = Object.entries(flashArgs.flash_files || {})
  .map(([offset, relativePath]) => {
    const sourcePath = join(buildDir, relativePath);
    const fileName = basename(relativePath);
    if (packagedNames.has(fileName)) {
      throw new Error(`ESP-IDF flash map contains duplicate filename ${fileName}`);
    }
    packagedNames.add(fileName);
    const content = readFileSync(sourcePath);
    const sha256 = createHash('sha256').update(content).digest('hex');
    copyFileSync(sourcePath, join(outputDir, fileName));
    return {
      path: fileName,
      offset: Number.parseInt(offset, 16),
      size: content.length,
      sha256,
    };
  })
  .sort((left, right) => left.offset - right.offset);

if (parts.length === 0) {
  throw new Error(`No flash files were found in ${integration.build.projectPath}/build/flasher_args.json`);
}

const manifest = {
  integrationId: integration.id,
  name: integration.name,
  version: versionLabel,
  flashSize: flashArgs.flash_settings?.flash_size,
  builds: [
    {
      chipFamily: 'ESP32-S3',
      parts,
    },
  ],
};

if (process.env.REGISTRY_COMMIT) {
  if (!/^[a-f0-9]{40}$/.test(process.env.REGISTRY_COMMIT)) {
    throw new Error('REGISTRY_COMMIT must be a full lowercase commit SHA');
  }
  manifest.registryCommit = process.env.REGISTRY_COMMIT;
}

writeFileSync(
  join(outputDir, 'manifest.json'),
  `${JSON.stringify(manifest, null, 2)}\n`,
  'utf8',
);

console.log(`Packaged ${integrationId} ${versionLabel} with ${parts.length} firmware part(s).`);
