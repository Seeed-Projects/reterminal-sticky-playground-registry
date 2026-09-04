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
import { createFlashManifest, parseOffsets } from './create-flash-manifest.mjs';

const SCRIPT_DIR = dirname(fileURLToPath(import.meta.url));
const CREATE_SCRIPT = join(SCRIPT_DIR, 'create-flash-manifest.mjs');

const bootloader = Buffer.from([0xe9, 0x01, 0x02]);
const partitionTable = Buffer.from([0xaa, 0xbb, 0xcc, 0xdd]);
const application = Buffer.from([0xe9, 0x03, 0x04, 0x05, 0x06]);

function createRegistry(files = { 'bootloader.bin': bootloader, 'app.bin': application }) {
  const root = mkdtempSync(join(tmpdir(), 'sticky-manifest-test-'));
  const firmwareDir = join(root, 'firmwares', 'example-firmware');
  const versionDir = join(firmwareDir, 'firmware', '1.0.0');
  mkdirSync(versionDir, { recursive: true });
  writeFileSync(
    join(firmwareDir, 'firmware.json'),
    `${JSON.stringify({ id: 'example-firmware', name: 'Example Firmware' }, null, 2)}\n`,
  );
  for (const [name, content] of Object.entries(files)) {
    writeFileSync(join(versionDir, name), content);
  }
  return { root, versionDir };
}

test('reads offsets from command line parts and from an ESP-IDF build', () => {
  assert.deepEqual(
    Array.from(parseOffsets({ parts: ['app.bin@0x10000', 'bootloader.bin@0'] })),
    [['app.bin', 65536], ['bootloader.bin', 0]],
  );

  assert.deepEqual(
    Array.from(parseOffsets({
      flasherArgs: {
        flash_files: {
          '0x0': 'bootloader/bootloader.bin',
          '0x8000': 'partition_table/partition-table.bin',
        },
      },
    })),
    [['bootloader.bin', 0], ['partition-table.bin', 32768]],
  );

  assert.throws(() => parseOffsets({ parts: ['app.bin'] }), /Use <file>\.bin@<offset>/);
  assert.throws(() => parseOffsets({ parts: ['app.bin@later'] }), /Invalid flash offset/);
});

test('fills in sizes and SHA-256 values for every firmware part', () => {
  const { root, versionDir } = createRegistry();
  try {
    const { manifest } = createFlashManifest({
      rootDir: root,
      firmwareId: 'example-firmware',
      version: '1.0.0',
      offsets: parseOffsets({ parts: ['bootloader.bin@0x0', 'app.bin@0x10000'] }),
    });

    assert.equal(manifest.name, 'Example Firmware');
    assert.equal(manifest.version, '1.0.0');
    assert.equal(manifest.flashSize, '16MB');
    assert.equal(manifest.builds[0].chipFamily, 'ESP32-S3');
    assert.deepEqual(manifest.builds[0].parts, [
      {
        path: 'bootloader.bin',
        offset: 0,
        size: bootloader.length,
        sha256: createHash('sha256').update(bootloader).digest('hex'),
      },
      {
        path: 'app.bin',
        offset: 65536,
        size: application.length,
        sha256: createHash('sha256').update(application).digest('hex'),
      },
    ]);

    const written = JSON.parse(readFileSync(join(versionDir, 'manifest.json'), 'utf8'));
    assert.deepEqual(written, manifest);
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});

test('uses offset 0 when the version directory holds one merged image', () => {
  const { root } = createRegistry({ 'merged.bin': application });
  try {
    const { manifest } = createFlashManifest({
      rootDir: root,
      firmwareId: 'example-firmware',
      version: '1.0.0',
    });
    assert.deepEqual(manifest.builds[0].parts.map((part) => [part.path, part.offset]), [['merged.bin', 0]]);
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});

test('asks for offsets when several files could go anywhere', () => {
  const { root } = createRegistry();
  try {
    assert.throws(() => createFlashManifest({
      rootDir: root,
      firmwareId: 'example-firmware',
      version: '1.0.0',
    }), /Give each one an offset/);
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});

test('reports files that are unlisted, unknown, or overlapping', () => {
  const { root } = createRegistry({
    'bootloader.bin': bootloader,
    'partition-table.bin': partitionTable,
  });
  try {
    assert.throws(() => createFlashManifest({
      rootDir: root,
      firmwareId: 'example-firmware',
      version: '1.0.0',
      offsets: parseOffsets({ parts: ['bootloader.bin@0'] }),
    }), /partition-table\.bin has no offset/);

    assert.throws(() => createFlashManifest({
      rootDir: root,
      firmwareId: 'example-firmware',
      version: '1.0.0',
      offsets: parseOffsets({ parts: ['bootloader.bin@0', 'partition-table.bin@0x8000', 'ghost.bin@0x9000'] }),
    }), /ghost\.bin is not in/);

    assert.throws(() => createFlashManifest({
      rootDir: root,
      firmwareId: 'example-firmware',
      version: '1.0.0',
      offsets: parseOffsets({ parts: ['bootloader.bin@0', 'partition-table.bin@1'] }),
    }), /overlaps bootloader\.bin/);
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});

test('keeps the flasher settings and MD5 values an earlier manifest carried', () => {
  const { root, versionDir } = createRegistry({ 'merged.bin': application });
  try {
    writeFileSync(join(versionDir, 'manifest.json'), `${JSON.stringify({
      name: 'Example Firmware',
      version: '1.0.0',
      flashSize: '16MB',
      flashMode: 'dio',
      baudRate: 921600,
      new_install_prompt_erase: true,
      builds: [{
        chipFamily: 'ESP32-S3',
        parts: [{
          path: 'merged.bin',
          offset: 0,
          size: application.length,
          sha256: createHash('sha256').update(application).digest('hex'),
          md5: 'a'.repeat(32),
        }],
      }],
    }, null, 2)}\n`);

    const { manifest } = createFlashManifest({
      rootDir: root,
      firmwareId: 'example-firmware',
      version: '1.0.0',
    });

    assert.equal(manifest.flashMode, 'dio');
    assert.equal(manifest.baudRate, 921600);
    assert.equal(manifest.new_install_prompt_erase, true);
    assert.equal(manifest.builds[0].parts[0].md5, 'a'.repeat(32));
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});

test('drops the MD5 of a part whose bytes changed', () => {
  const { root, versionDir } = createRegistry({ 'merged.bin': application });
  try {
    writeFileSync(join(versionDir, 'manifest.json'), `${JSON.stringify({
      name: 'Example Firmware',
      version: '1.0.0',
      builds: [{
        chipFamily: 'ESP32-S3',
        parts: [{
          path: 'merged.bin',
          offset: 0,
          size: bootloader.length,
          sha256: createHash('sha256').update(bootloader).digest('hex'),
          md5: 'a'.repeat(32),
        }],
      }],
    }, null, 2)}\n`);

    const { manifest } = createFlashManifest({
      rootDir: root,
      firmwareId: 'example-firmware',
      version: '1.0.0',
    });

    assert.equal(manifest.builds[0].parts[0].md5, undefined);
    assert.equal(manifest.builds[0].parts[0].size, application.length);
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});

test('reports a missing firmware.json and a missing version directory', () => {
  const { root } = createRegistry();
  try {
    assert.throws(() => createFlashManifest({
      rootDir: root,
      firmwareId: 'unknown-firmware',
      version: '1.0.0',
    }), /has no firmware\.json/);

    assert.throws(() => createFlashManifest({
      rootDir: root,
      firmwareId: 'example-firmware',
      version: '2.0.0',
    }), /copy the \.bin files into it first/);
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});

test('runs from the command line with an ESP-IDF flasher_args.json', () => {
  const { root, versionDir } = createRegistry({
    'bootloader.bin': bootloader,
    'partition-table.bin': partitionTable,
    'app.bin': application,
  });
  try {
    const flasherArgsPath = join(root, 'flasher_args.json');
    writeFileSync(flasherArgsPath, `${JSON.stringify({
      flash_files: {
        '0x0': 'bootloader/bootloader.bin',
        '0x8000': 'partition_table/partition-table.bin',
        '0x10000': 'app.bin',
      },
    }, null, 2)}\n`);

    const result = spawnSync(
      process.execPath,
      [CREATE_SCRIPT, 'example-firmware', '1.0.0', '--flasher-args', flasherArgsPath],
      { encoding: 'utf8', env: { ...process.env, REGISTRY_ROOT: root } },
    );

    assert.equal(result.status, 0, result.stderr);
    assert.match(result.stdout, /3 firmware part\(s\)/);
    assert.match(result.stdout, /"manifestPath": "firmware\/1\.0\.0\/manifest\.json"/);

    const manifest = JSON.parse(readFileSync(join(versionDir, 'manifest.json'), 'utf8'));
    assert.deepEqual(
      manifest.builds[0].parts.map((part) => [part.path, part.offset]),
      [['bootloader.bin', 0], ['partition-table.bin', 32768], ['app.bin', 65536]],
    );
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});
