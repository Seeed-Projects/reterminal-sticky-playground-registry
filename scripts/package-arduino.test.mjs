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
const PACKAGE_SCRIPT = join(SCRIPT_DIR, 'package-arduino.mjs');

function createSketchProject(root, flashSettingsLine) {
  const integrationDir = join(root, 'firmwares', 'sketch-firmware');
  const buildDir = join(integrationDir, 'source', 'build');
  mkdirSync(buildDir, { recursive: true });
  writeFileSync(
    join(integrationDir, 'firmware.json'),
    `${JSON.stringify({
      id: 'sketch-firmware',
      name: 'Sketch Firmware',
      build: {
        system: 'arduino',
        profile: 'sticky',
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

  const files = {
    'blink.ino.bootloader.bin': Buffer.from([0xe9, 0x01]),
    'blink.ino.partitions.bin': Buffer.from([0xaa, 0x50, 0x02]),
    'boot_app0.bin': Buffer.from([0x00, 0x00, 0x00, 0x03]),
    'blink.ino.bin': Buffer.from([0xe9, 0x04, 0x05, 0x06, 0x07]),
  };
  for (const [fileName, content] of Object.entries(files)) {
    writeFileSync(join(buildDir, fileName), content);
  }

  writeFileSync(
    join(buildDir, 'flash_args'),
    [
      flashSettingsLine,
      '0x0 blink.ino.bootloader.bin',
      '0x8000 blink.ino.partitions.bin',
      '0xe000 boot_app0.bin',
      '0x10000 blink.ino.bin',
      '',
    ].join('\n'),
  );

  return { integrationDir, files };
}

function runPackage(root) {
  const result = spawnSync(
    process.execPath,
    [PACKAGE_SCRIPT, 'sketch-firmware', '1.0.0'],
    {
      encoding: 'utf8',
      env: { ...process.env, REGISTRY_ROOT: root },
    },
  );
  assert.equal(result.status, 0, result.stderr);
  return result;
}

test('packages the flash map that the arduino-esp32 core reports', () => {
  const root = mkdtempSync(join(tmpdir(), 'sticky-arduino-package-test-'));
  try {
    const { integrationDir, files } = createSketchProject(
      root,
      '--flash-mode dio --flash-freq 80m --flash-size 16MB',
    );

    runPackage(root);

    const manifest = JSON.parse(readFileSync(
      join(integrationDir, 'firmware', '1.0.0', 'manifest.json'),
      'utf8',
    ));
    assert.equal(manifest.integrationId, 'sketch-firmware');
    assert.equal(manifest.flashSize, '16MB');
    assert.equal(manifest.builds[0].chipFamily, 'ESP32-S3');
    assert.deepEqual(
      manifest.builds[0].parts.map((part) => [part.path, part.offset, part.size]),
      [
        ['blink.ino.bootloader.bin', 0, 2],
        ['blink.ino.partitions.bin', 32768, 3],
        ['boot_app0.bin', 57344, 4],
        ['blink.ino.bin', 65536, 5],
      ],
    );
    assert.equal(
      manifest.builds[0].parts[3].sha256,
      createHash('sha256').update(files['blink.ino.bin']).digest('hex'),
    );
    assert.deepEqual(
      readFileSync(join(integrationDir, 'firmware', '1.0.0', 'boot_app0.bin')),
      files['boot_app0.bin'],
    );
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});

test('reads the flash size from cores that still use underscored esptool flags', () => {
  const root = mkdtempSync(join(tmpdir(), 'sticky-arduino-package-test-'));
  try {
    const { integrationDir } = createSketchProject(
      root,
      '--flash_mode dio --flash_freq 80m --flash_size 8MB',
    );

    runPackage(root);

    const manifest = JSON.parse(readFileSync(
      join(integrationDir, 'firmware', '1.0.0', 'manifest.json'),
      'utf8',
    ));
    assert.equal(manifest.flashSize, '8MB');
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});
