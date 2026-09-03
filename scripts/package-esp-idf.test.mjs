import assert from 'node:assert/strict';
import { createHash } from 'node:crypto';
import {
  mkdirSync,
  mkdtempSync,
  readFileSync,
  rmSync,
  writeFileSync,
} from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join } from 'node:path';
import { spawnSync } from 'node:child_process';
import test from 'node:test';
import { fileURLToPath } from 'node:url';

const SCRIPT_DIR = dirname(fileURLToPath(import.meta.url));
const PACKAGE_SCRIPT = join(SCRIPT_DIR, 'package-esp-idf.mjs');
const VERIFY_SCRIPT = join(SCRIPT_DIR, 'verify-esp-idf-build.mjs');

test('packages and verifies an ESP-IDF flash map', () => {
  const root = mkdtempSync(join(tmpdir(), 'sticky-package-test-'));
  try {
    const integrationDir = join(root, 'firmwares', 'example-firmware');
    const buildDir = join(integrationDir, 'source', 'build');
    mkdirSync(join(buildDir, 'bootloader'), { recursive: true });
    writeFileSync(
      join(integrationDir, 'firmware.json'),
      `${JSON.stringify({
        id: 'example-firmware',
        name: 'Example Firmware',
        build: {
          system: 'esp-idf',
          projectPath: 'source',
        },
        flash: {
          versions: [{
            version: '1.0.0',
            manifestPath: 'firmware/1.0.0/manifest.json',
          }],
        },
      }, null, 2)}\n`,
    );

    const bootloader = Buffer.from([0xe9, 0x01, 0x02]);
    const application = Buffer.from([0xe9, 0x03, 0x04, 0x05]);
    writeFileSync(join(buildDir, 'bootloader', 'bootloader.bin'), bootloader);
    writeFileSync(join(buildDir, 'app.bin'), application);
    writeFileSync(
      join(buildDir, 'flasher_args.json'),
      `${JSON.stringify({
        flash_files: {
          '0x0': 'bootloader/bootloader.bin',
          '0x10000': 'app.bin',
        },
        flash_settings: {
          flash_size: '16MB',
        },
      }, null, 2)}\n`,
    );

    const packageResult = spawnSync(
      process.execPath,
      [PACKAGE_SCRIPT, 'example-firmware', '1.0.0'],
      {
        encoding: 'utf8',
        env: { ...process.env, REGISTRY_ROOT: root },
      },
    );
    assert.equal(packageResult.status, 0, packageResult.stderr);

    const manifest = JSON.parse(readFileSync(
      join(integrationDir, 'firmware', '1.0.0', 'manifest.json'),
      'utf8',
    ));
    assert.deepEqual(
      manifest.builds[0].parts.map((part) => [part.path, part.offset, part.size]),
      [
        ['bootloader.bin', 0, bootloader.length],
        ['app.bin', 65536, application.length],
      ],
    );
    assert.equal(
      manifest.builds[0].parts[1].sha256,
      createHash('sha256').update(application).digest('hex'),
    );

    const verifyResult = spawnSync(
      process.execPath,
      [VERIFY_SCRIPT, 'example-firmware'],
      {
        encoding: 'utf8',
        env: { ...process.env, REGISTRY_ROOT: root },
      },
    );
    assert.equal(verifyResult.status, 0, verifyResult.stderr);
    assert.match(verifyResult.stdout, /Verified example-firmware 1\.0\.0/);
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});

test('packages a source-built firmware into an Action output directory', () => {
  const root = mkdtempSync(join(tmpdir(), 'sticky-source-package-test-'));
  try {
    const integrationDir = join(root, 'firmwares', 'source-firmware');
    const buildDir = join(integrationDir, 'source', 'build');
    const outputDir = join(root, '.firmware-output', 'source-firmware', '2.0.0-rc1');
    mkdirSync(join(buildDir, 'partition_table'), { recursive: true });
    writeFileSync(
      join(integrationDir, 'firmware.json'),
      `${JSON.stringify({
        id: 'source-firmware',
        name: 'Source Firmware',
        build: {
          system: 'esp-idf',
          projectPath: 'source',
        },
        flash: {
          versions: [{
            version: '2.0.0-rc1',
            sourceBuild: true,
          }],
        },
      }, null, 2)}\n`,
    );

    const partitionTable = Buffer.from([0xaa, 0xbb, 0xcc]);
    const application = Buffer.from([0xe9, 0x06, 0x07, 0x08]);
    writeFileSync(join(buildDir, 'partition_table', 'partition-table.bin'), partitionTable);
    writeFileSync(join(buildDir, 'source.bin'), application);
    writeFileSync(
      join(buildDir, 'flasher_args.json'),
      `${JSON.stringify({
        flash_files: {
          '0x8000': 'partition_table/partition-table.bin',
          '0x10000': 'source.bin',
        },
        flash_settings: {
          flash_size: '8MB',
        },
      }, null, 2)}\n`,
    );

    const registryCommit = 'c'.repeat(40);
    const packageResult = spawnSync(
      process.execPath,
      [PACKAGE_SCRIPT, 'source-firmware', '2.0.0-rc1'],
      {
        encoding: 'utf8',
        env: {
          ...process.env,
          REGISTRY_ROOT: root,
          FIRMWARE_OUTPUT_DIR: outputDir,
          REGISTRY_COMMIT: registryCommit,
        },
      },
    );
    assert.equal(packageResult.status, 0, packageResult.stderr);

    const manifest = JSON.parse(readFileSync(join(outputDir, 'manifest.json'), 'utf8'));
    assert.equal(manifest.integrationId, 'source-firmware');
    assert.equal(manifest.registryCommit, registryCommit);
    assert.equal(manifest.version, '2.0.0-rc1');
    assert.equal(manifest.flashSize, '8MB');
    assert.deepEqual(
      manifest.builds[0].parts.map((part) => [part.path, part.offset]),
      [
        ['partition-table.bin', 32768],
        ['source.bin', 65536],
      ],
    );
    assert.equal(
      manifest.builds[0].parts[0].sha256,
      createHash('sha256').update(partitionTable).digest('hex'),
    );
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});
