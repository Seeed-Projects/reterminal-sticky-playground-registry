import { createHash } from 'node:crypto';
import {
  copyFileSync,
  mkdirSync,
  readFileSync,
  writeFileSync,
} from 'node:fs';
import { basename, dirname, join, resolve } from 'node:path';

const CHIP_FAMILY = 'ESP32-S3';

const ROOT_DIR = process.env.REGISTRY_ROOT
  ? resolve(process.env.REGISTRY_ROOT)
  : resolve(import.meta.dirname, '..');

function parseOffset(value) {
  const text = String(value).trim();
  const offset = /^0x/i.test(text) ? Number.parseInt(text, 16) : Number.parseInt(text, 10);
  if (!Number.isInteger(offset) || offset < 0) {
    throw new Error(`Invalid flash offset "${value}". Use a decimal number or a 0x-prefixed address`);
  }
  return offset;
}

// Loads a firmware entry built with the given system and resolves its project and output directories.
// 读取由指定编译系统构建的固件条目,并解析出它的工程目录与产物目录。
export function resolveTarget(system, integrationId, versionLabel) {
  if (!integrationId || !versionLabel) {
    throw new Error(`Usage: npm run package:${system} -- <firmware-id> <version>`);
  }

  const integrationDir = join(ROOT_DIR, 'firmwares', integrationId);
  const integration = JSON.parse(readFileSync(join(integrationDir, 'firmware.json'), 'utf8'));
  if (integration.id !== integrationId || integration.build?.system !== system) {
    throw new Error(`${integrationId} is not configured as a ${system} firmware`);
  }

  const version = integration.flash?.versions?.find((entry) => entry.version === versionLabel);
  if (!version) {
    throw new Error(`${integrationId} does not define firmware version ${versionLabel}`);
  }
  if (!version.manifestPath && version.sourceBuild !== true) {
    throw new Error(`${integrationId} ${versionLabel} does not define a package destination`);
  }

  const configuredOutputDir = process.env.FIRMWARE_OUTPUT_DIR;
  if (version.sourceBuild === true && !configuredOutputDir) {
    throw new Error('FIRMWARE_OUTPUT_DIR is required for a source-built firmware package');
  }

  return {
    integration,
    projectDir: join(integrationDir, integration.build.projectPath),
    outputDir: configuredOutputDir
      ? resolve(configuredOutputDir)
      : join(integrationDir, dirname(version.manifestPath)),
  };
}

// Reads an ESP-IDF style flasher_args.json into an offset and file list.
// 读取 ESP-IDF 风格的 flasher_args.json,得到偏移地址与文件清单。
export function readFlasherArgsJson(buildDir) {
  const flashArgs = JSON.parse(readFileSync(join(buildDir, 'flasher_args.json'), 'utf8'));
  return {
    files: Object.entries(flashArgs.flash_files || {}).map(([offset, relativePath]) => ({
      offset: parseOffset(offset),
      sourcePath: join(buildDir, relativePath),
    })),
    flashSize: flashArgs.flash_settings?.flash_size,
  };
}

// Reads the flash_args file that the arduino-esp32 core writes next to its build artifacts.
// 读取 arduino-esp32 内核在编译产物旁生成的 flash_args 文件。
export function readArduinoFlashArgs(buildDir) {
  const lines = readFileSync(join(buildDir, 'flash_args'), 'utf8')
    .split('\n')
    .map((line) => line.trim())
    .filter(Boolean);
  const settings = lines.shift() ?? '';

  return {
    files: lines.map((line) => {
      const [offset, relativePath] = line.split(/\s+/);
      if (!offset || !relativePath) {
        throw new Error(`Cannot read a flash offset and file name from "${line}"`);
      }
      return { offset: parseOffset(offset), sourcePath: join(buildDir, relativePath) };
    }),
    flashSize: /--flash[-_]size\s+(\S+)/.exec(settings)?.[1],
  };
}

// Copies every flash file into the output directory and writes the Registry manifest beside them.
// 把每个烧录文件复制到产物目录,并在旁边生成 Registry 格式的 manifest。
export function writeFirmwarePackage({
  integration,
  versionLabel,
  outputDir,
  files,
  flashSize,
}) {
  if (files.length === 0) {
    throw new Error(`No flash files were found for ${integration.id} ${versionLabel}`);
  }

  mkdirSync(outputDir, { recursive: true });
  const packagedNames = new Set();
  const parts = files
    .map(({ offset, sourcePath }) => {
      const fileName = basename(sourcePath);
      if (packagedNames.has(fileName)) {
        throw new Error(`The flash map contains duplicate filename ${fileName}`);
      }
      packagedNames.add(fileName);
      const content = readFileSync(sourcePath);
      copyFileSync(sourcePath, join(outputDir, fileName));
      return {
        path: fileName,
        offset,
        size: content.length,
        sha256: createHash('sha256').update(content).digest('hex'),
      };
    })
    .sort((left, right) => left.offset - right.offset);

  const manifest = {
    integrationId: integration.id,
    name: integration.name,
    version: versionLabel,
    flashSize,
    builds: [
      {
        chipFamily: CHIP_FAMILY,
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

  return parts;
}
