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
const PACKAGE_SCRIPT = join(SCRIPT_DIR, 'package-platformio.mjs');

test('packages the flash map that the PlatformIO build dumps', () => {
  const root = mkdtempSync(join(tmpdir(), 'sticky-platformio-package-test-'));
  try {
    const integrationDir = join(root, 'firmwares', 'pio-firmware');
    const buildDir = join(integrationDir, 'source', '.pio', 'build', 'sticky');
    const outputDir = join(root, '.firmware-output', 'pio-firmware', '1.2.0');
    mkdirSync(buildDir, { recursive: true });
    writeFileSync(
      join(integrationDir, 'firmware.json'),
      `${JSON.stringify({
        id: 'pio-firmware',
        name: 'PlatformIO Firmware',
        build: {
          system: 'platformio',
          environment: 'sticky',
          projectPath: 'source',
        },
        flash: {
          versions: [{
            version: '1.2.0',
            sourceBuild: true,
          }],
        },
      }, null, 2)}\n`,
    );

    const bootloader = Buffer.from([0xe9, 0x01, 0x02]);
    const partitions = Buffer.from([0xaa, 0x50]);
    const application = Buffer.from([0xe9, 0x03, 0x04, 0x05]);
    writeFileSync(join(buildDir, 'bootloader.bin'), bootloader);
    writeFileSync(join(buildDir, 'partitions.bin'), partitions);
    writeFileSync(join(buildDir, 'firmware.bin'), application);
    writeFileSync(
      join(buildDir, 'flasher_args.json'),
      `${JSON.stringify({
        flash_files: {
          '0x0': 'bootloader.bin',
          '0x8000': 'partitions.bin',
          '0x10000': 'firmware.bin',
        },
        flash_settings: {
          flash_size: '16MB',
        },
      }, null, 2)}\n`,
    );

    const result = spawnSync(
      process.execPath,
      [PACKAGE_SCRIPT, 'pio-firmware', '1.2.0'],
      {
        encoding: 'utf8',
        env: {
          ...process.env,
          REGISTRY_ROOT: root,
          FIRMWARE_OUTPUT_DIR: outputDir,
        },
      },
    );
    assert.equal(result.status, 0, result.stderr);

    const manifest = JSON.parse(readFileSync(join(outputDir, 'manifest.json'), 'utf8'));
    assert.equal(manifest.integrationId, 'pio-firmware');
    assert.equal(manifest.version, '1.2.0');
    assert.equal(manifest.flashSize, '16MB');
    assert.deepEqual(
      manifest.builds[0].parts.map((part) => [part.path, part.offset, part.size]),
      [
        ['bootloader.bin', 0, bootloader.length],
        ['partitions.bin', 32768, partitions.length],
        ['firmware.bin', 65536, application.length],
      ],
    );
    assert.equal(
      manifest.builds[0].parts[2].sha256,
      createHash('sha256').update(application).digest('hex'),
    );
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});

test('refuses to package a firmware that declares a different build system', () => {
  const root = mkdtempSync(join(tmpdir(), 'sticky-platformio-package-test-'));
  try {
    const integrationDir = join(root, 'firmwares', 'idf-firmware');
    mkdirSync(integrationDir, { recursive: true });
    writeFileSync(
      join(integrationDir, 'firmware.json'),
      `${JSON.stringify({
        id: 'idf-firmware',
        name: 'IDF Firmware',
        build: {
          system: 'esp-idf',
          version: 'v5.4',
          target: 'esp32s3',
          projectPath: 'source',
        },
        flash: {
          versions: [{ version: '1.0.0', manifestPath: 'firmware/1.0.0/manifest.json' }],
        },
      }, null, 2)}\n`,
    );

    const result = spawnSync(
      process.execPath,
      [PACKAGE_SCRIPT, 'idf-firmware', '1.0.0'],
      {
        encoding: 'utf8',
        env: { ...process.env, REGISTRY_ROOT: root },
      },
    );

    assert.equal(result.status, 1);
    assert.match(result.stderr, /is not configured as a platformio firmware/);
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});
