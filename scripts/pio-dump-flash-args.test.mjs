import assert from 'node:assert/strict';
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
const DUMP_SCRIPT = join(SCRIPT_DIR, 'pio-dump-flash-args.py');

// Runs the dump script with a stand-in for the SCons environment PlatformIO injects.
// 用一个替身对象模拟 PlatformIO 注入的 SCons 环境,以此运行导出脚本。
const HARNESS = `
import os
import sys

BUILD_DIR, FRAMEWORK_DIR, SCRIPT_PATH = sys.argv[1:4]


class Board:
    def get(self, key, default=None):
        return {"upload.flash_size": "16MB"}.get(key, default)


class Env:
    def get(self, key, default=None):
        if key == "FLASH_EXTRA_IMAGES":
            return [
                ("0x0", os.path.join("$BUILD_DIR", "bootloader.bin")),
                ("0x8000", os.path.join("$BUILD_DIR", "partitions.bin")),
                ("0xe000", os.path.join(FRAMEWORK_DIR, "boot_app0.bin")),
            ]
        return default

    def subst(self, value):
        return (value
                .replace("$BUILD_DIR", BUILD_DIR)
                .replace("\${PROGNAME}", "firmware")
                .replace("$ESP32_APP_OFFSET", "0x10000"))

    def BoardConfig(self):
        return Board()


with open(SCRIPT_PATH, encoding="utf8") as handle:
    source = handle.read()

exec(compile(source, SCRIPT_PATH, "exec"), {"Import": lambda name: None, "env": Env()})
`;

test('records every flash offset PlatformIO reports, including framework files', () => {
  const root = mkdtempSync(join(tmpdir(), 'sticky-pio-dump-test-'));
  try {
    const buildDir = join(root, 'project', '.pio', 'build', 'sticky');
    const frameworkDir = join(root, 'packages', 'framework-arduinoespressif32', 'tools', 'partitions');
    mkdirSync(buildDir, { recursive: true });
    mkdirSync(frameworkDir, { recursive: true });
    const harnessPath = join(root, 'harness.py');
    writeFileSync(harnessPath, HARNESS);

    const result = spawnSync(
      'python3',
      [harnessPath, buildDir, frameworkDir, DUMP_SCRIPT],
      { encoding: 'utf8' },
    );
    assert.equal(result.status, 0, result.stderr);

    const flasherArgs = JSON.parse(readFileSync(join(buildDir, 'flasher_args.json'), 'utf8'));
    assert.equal(flasherArgs.flash_settings.flash_size, '16MB');
    assert.deepEqual(flasherArgs.flash_files, {
      '0x0': 'bootloader.bin',
      '0x8000': 'partitions.bin',
      '0xe000': '../../../../packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin',
      '0x10000': 'firmware.bin',
    });
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});
