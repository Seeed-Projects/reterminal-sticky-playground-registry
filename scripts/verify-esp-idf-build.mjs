#!/usr/bin/env node

import { createHash } from 'node:crypto';
import { existsSync, readFileSync } from 'node:fs';
import { basename, dirname, join, resolve } from 'node:path';

const ROOT_DIR = process.env.REGISTRY_ROOT
  ? resolve(process.env.REGISTRY_ROOT)
  : resolve(import.meta.dirname, '..');
const integrationId = process.argv[2];

if (!integrationId || !/^[a-z0-9]+(?:-[a-z0-9]+)*$/.test(integrationId)) {
  throw new Error('Usage: node scripts/verify-esp-idf-build.mjs <firmware-id>');
}

const integrationDir = join(ROOT_DIR, 'firmwares', integrationId);
const integration = JSON.parse(readFileSync(join(integrationDir, 'firmware.json'), 'utf8'));
const version = integration.flash?.versions?.[0];
if (integration.build?.system !== 'esp-idf' || !version?.manifestPath) {
  throw new Error(`${integrationId} is not a locally packaged ESP-IDF firmware`);
}

const projectDir = join(integrationDir, integration.build.projectPath);
const buildDir = join(projectDir, 'build');
const flashArgs = JSON.parse(readFileSync(join(buildDir, 'flasher_args.json'), 'utf8'));
const manifest = JSON.parse(readFileSync(join(integrationDir, version.manifestPath), 'utf8'));
const build = manifest.builds?.find((entry) => entry.chipFamily === 'ESP32-S3') || manifest.builds?.[0];
if (!build?.parts?.length) {
  throw new Error(`${version.manifestPath} does not contain a firmware build`);
}

// Maps ESP-IDF output offsets to build files for byte-for-byte package verification.
// 将 ESP-IDF 输出偏移映射到构建文件，用于逐字节校验固件包。
const buildFiles = new Map(
  Object.entries(flashArgs.flash_files || {}).map(([offset, path]) => [
    Number.parseInt(offset, 16),
    join(buildDir, path),
  ]),
);

if (build.parts.length !== buildFiles.size) {
  throw new Error(
    `${version.manifestPath} must contain every file from the ESP-IDF flash map`,
  );
}

for (const part of build.parts) {
  const builtPath = buildFiles.get(part.offset);
  if (!builtPath || !existsSync(builtPath)) {
    throw new Error(`Missing ESP-IDF output for offset 0x${part.offset.toString(16)}`);
  }
  const built = readFileSync(builtPath);
  const packagedPath = join(integrationDir, dirname(version.manifestPath), basename(part.path));
  const packaged = readFileSync(packagedPath);
  const builtHash = createHash('sha256').update(built).digest('hex');
  const packagedHash = createHash('sha256').update(packaged).digest('hex');
  if (built.length !== part.size || builtHash !== part.sha256 || packagedHash !== part.sha256) {
    throw new Error(`${basename(part.path)} does not match the ESP-IDF build output`);
  }
}

console.log(`Verified ${integrationId} ${version.version} against a clean ESP-IDF build.`);
