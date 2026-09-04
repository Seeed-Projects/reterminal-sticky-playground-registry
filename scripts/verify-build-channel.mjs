#!/usr/bin/env node

import { spawnSync } from 'node:child_process';
import { createHash } from 'node:crypto';
import {
  cpSync,
  mkdirSync,
  mkdtempSync,
  readFileSync,
  writeFileSync,
} from 'node:fs';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';

const REPOSITORY_ROOT = resolve(import.meta.dirname, '..');
const VERSION = '0.0.0';
const ENTRY_ID = 'channel-starter';

// How each build channel compiles its starter project and what CI must install for it.
// 每条编译通道如何编译自己的示例工程,以及 CI 需要为它准备什么。
const CHANNELS = new Map([
  ['platformio', {
    starter: 'platformio-starter',
    // PlatformIO takes the project directory as an argument, so any name works.
    // PlatformIO 通过参数接收工程目录,目录名可以任意。
    projectDirName: 'source',
    build: { system: 'platformio', environment: 'sticky' },
    compile: (projectDir) => ({
      command: 'pio',
      args: ['run', '-d', projectDir, '-e', 'sticky'],
      env: {
        PLATFORMIO_EXTRA_SCRIPTS: `post:${join(REPOSITORY_ROOT, 'scripts', 'pio-dump-flash-args.py')}`,
      },
    }),
  }],
  ['arduino', {
    starter: 'arduino-starter',
    // Arduino CLI requires the sketch directory and the .ino file to share a name.
    // Arduino CLI 要求 sketch 目录名与 .ino 文件名相同。
    projectDirName: 'arduino-starter',
    build: { system: 'arduino', profile: 'sticky' },
    compile: (projectDir) => ({
      command: 'arduino-cli',
      args: ['compile', '--profile', 'sticky', '--build-path', join(projectDir, 'build'), projectDir],
      env: {},
    }),
  }],
]);

const systemName = process.argv[2];
const channel = CHANNELS.get(systemName);
if (!channel) {
  throw new Error(`Usage: node scripts/verify-build-channel.mjs <${[...CHANNELS.keys()].join('|')}>`);
}

// Copies the starter project into a throwaway Registry entry the packaging script accepts.
// 把示例工程复制成一个临时的 Registry 条目,供打包脚本处理。
function createRegistryEntry() {
  const registryRoot = mkdtempSync(join(tmpdir(), `sticky-channel-${systemName}-`));
  const entryDir = join(registryRoot, 'firmwares', ENTRY_ID);
  const projectDir = join(entryDir, channel.projectDirName);
  mkdirSync(entryDir, { recursive: true });
  cpSync(join(REPOSITORY_ROOT, 'examples', channel.starter), projectDir, { recursive: true });
  writeFileSync(
    join(entryDir, 'firmware.json'),
    `${JSON.stringify({
      id: ENTRY_ID,
      name: 'Build channel starter',
      build: { ...channel.build, projectPath: channel.projectDirName },
      flash: { versions: [{ version: VERSION, sourceBuild: true }] },
    }, null, 2)}\n`,
  );
  return { registryRoot, projectDir, outputDir: join(registryRoot, 'output') };
}

function run(command, args, env) {
  console.log(`$ ${command} ${args.join(' ')}`);
  const result = spawnSync(command, args, {
    stdio: 'inherit',
    env: { ...process.env, ...env },
  });
  if (result.error) {
    throw new Error(`Cannot run ${command}: ${result.error.message}`);
  }
  if (result.status !== 0) {
    throw new Error(`${command} exited with status ${result.status}`);
  }
}

// Checks that every manifest part points at a real file with the recorded size and hash.
// 检查 manifest 中每个片段都对应一个真实文件,且大小与哈希都吻合。
function assertPackagedFiles(outputDir) {
  const manifest = JSON.parse(readFileSync(join(outputDir, 'manifest.json'), 'utf8'));
  const parts = manifest.builds?.[0]?.parts ?? [];
  if (parts.length < 2) {
    throw new Error(`Expected at least a bootloader and an application part, got ${parts.length}`);
  }

  let previousOffset = -1;
  for (const part of parts) {
    const content = readFileSync(join(outputDir, part.path));
    if (content.length !== part.size) {
      throw new Error(`${part.path} is ${content.length} bytes but the manifest records ${part.size}`);
    }
    const sha256 = createHash('sha256').update(content).digest('hex');
    if (sha256 !== part.sha256) {
      throw new Error(`${part.path} does not match the sha256 recorded in the manifest`);
    }
    if (part.offset <= previousOffset) {
      throw new Error(`${part.path} is not ordered after offset ${previousOffset}`);
    }
    previousOffset = part.offset;
  }

  return { manifest, parts };
}

const { registryRoot, projectDir, outputDir } = createRegistryEntry();
const { command, args, env } = channel.compile(projectDir);
run(command, args, env);
run(process.execPath, [
  join(REPOSITORY_ROOT, 'scripts', `package-${systemName}.mjs`),
  ENTRY_ID,
  VERSION,
], {
  REGISTRY_ROOT: registryRoot,
  FIRMWARE_OUTPUT_DIR: outputDir,
});

const { manifest, parts } = assertPackagedFiles(outputDir);
console.log(`\nThe ${systemName} channel produced a ${manifest.flashSize ?? 'unknown'} flash layout:`);
for (const part of parts) {
  console.log(`  0x${part.offset.toString(16).padStart(6, '0')}  ${String(part.size).padStart(9)} bytes  ${part.path}`);
}
