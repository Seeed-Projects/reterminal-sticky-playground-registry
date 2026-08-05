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
  throw new Error('Usage: npm run package:esp-idf -- <integration-id> <version>');
}

const integrationDir = join(ROOT_DIR, 'integrations', integrationId);
const integrationPath = join(integrationDir, 'integration.json');
const integration = JSON.parse(readFileSync(integrationPath, 'utf8'));
if (integration.id !== integrationId || integration.build?.system !== 'esp-idf') {
  throw new Error(`${integrationId} is not configured as an ESP-IDF firmware`);
}

const version = integration.flash?.versions?.find((entry) => entry.version === versionLabel);
if (!version?.manifestPath) {
  throw new Error(`${integrationId} ${versionLabel} must define flash.versions[].manifestPath`);
}

const projectDir = join(integrationDir, integration.build.projectPath);
const buildDir = join(projectDir, 'build');
const flashArgs = JSON.parse(readFileSync(join(buildDir, 'flasher_args.json'), 'utf8'));
const outputDir = join(integrationDir, dirname(version.manifestPath));
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

writeFileSync(
  join(integrationDir, version.manifestPath),
  `${JSON.stringify(manifest, null, 2)}\n`,
  'utf8',
);

console.log(`Packaged ${integrationId} ${versionLabel} with ${parts.length} firmware part(s).`);
