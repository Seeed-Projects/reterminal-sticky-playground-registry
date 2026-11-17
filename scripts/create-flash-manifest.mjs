#!/usr/bin/env node

import { createHash } from 'node:crypto';
import {
  existsSync,
  readFileSync,
  readdirSync,
  writeFileSync,
} from 'node:fs';
import { basename, join, resolve } from 'node:path';

const DEFAULT_FLASH_SIZE = '16MB';
const CHIP_FAMILY = 'ESP32-S3';
const VARIANT_PATTERN = /^[a-z0-9]+(?:-[a-z0-9]+)*$/;

// Reads "<file>.bin@<offset>" pairs and ESP-IDF flasher_args.json into one offset table.
// 把 "<file>.bin@<offset>" 参数和 ESP-IDF 的 flasher_args.json 合并成一张偏移表。
export function parseOffsets({ parts = [], flasherArgs }) {
  const offsets = new Map();

  if (flasherArgs) {
    for (const [offset, relativePath] of Object.entries(flasherArgs.flash_files || {})) {
      offsets.set(basename(relativePath), parseOffset(offset));
    }
  }

  for (const entry of parts) {
    const separator = entry.lastIndexOf('@');
    if (separator < 1) {
      throw new Error(`Invalid --part value "${entry}". Use <file>.bin@<offset>, for example app.bin@0x10000`);
    }
    offsets.set(entry.slice(0, separator), parseOffset(entry.slice(separator + 1)));
  }

  return offsets;
}

function parseOffset(value) {
  const text = String(value).trim();
  const offset = /^0x/i.test(text) ? Number.parseInt(text, 16) : Number.parseInt(text, 10);
  if (!Number.isInteger(offset) || offset < 0) {
    throw new Error(`Invalid flash offset "${value}". Use a decimal number or a 0x-prefixed address`);
  }
  return offset;
}

export function listBinaries(versionDir) {
  return readdirSync(versionDir)
    .filter((name) => name.toLowerCase().endsWith('.bin'))
    .sort();
}

// Returns the package directory of one version, or of one variant inside that version.
// 返回某个版本的固件包目录；指定变体时返回该版本下这个变体的目录。
export function packagePathFor(version, variant) {
  if (variant !== undefined && !VARIANT_PATTERN.test(variant)) {
    throw new Error(`Invalid variant "${variant}". Use lowercase letters, digits, and single hyphens, for example chinese`);
  }
  return variant ? `firmware/${version}/${variant}` : `firmware/${version}`;
}

// Builds the manifest the browser flasher reads, with byte sizes and SHA-256 filled in.
// 生成浏览器烧录页读取的 manifest，自动填入文件大小和 SHA-256。
export function createFlashManifest({
  rootDir,
  firmwareId,
  version,
  variant,
  offsets = new Map(),
  flashSize = DEFAULT_FLASH_SIZE,
}) {
  const firmwareDir = join(rootDir, 'firmwares', firmwareId);
  const metadataPath = join(firmwareDir, 'firmware.json');
  if (!existsSync(metadataPath)) {
    throw new Error(`${firmwareId} has no firmware.json. Create firmwares/${firmwareId}/firmware.json first`);
  }
  const firmware = JSON.parse(readFileSync(metadataPath, 'utf8'));

  const packagePath = packagePathFor(version, variant);
  const versionDir = join(firmwareDir, packagePath);
  if (!existsSync(versionDir)) {
    throw new Error(`Create firmwares/${firmwareId}/${packagePath}/ and copy the .bin files into it first`);
  }

  const binaries = listBinaries(versionDir);
  if (binaries.length === 0) {
    throw new Error(`No .bin files found in firmwares/${firmwareId}/${packagePath}/`);
  }

  const resolvedOffsets = new Map(offsets);
  if (resolvedOffsets.size === 0) {
    if (binaries.length > 1) {
      throw new Error(
        `Found ${binaries.length} .bin files (${binaries.join(', ')}). `
        + 'Give each one an offset with --part <file>.bin@<offset>, or point at the build with --flasher-args <path>',
      );
    }
    resolvedOffsets.set(binaries[0], 0);
  }

  for (const name of resolvedOffsets.keys()) {
    if (!binaries.includes(name)) {
      throw new Error(`${name} is not in firmwares/${firmwareId}/${packagePath}/`);
    }
  }
  for (const name of binaries) {
    if (!resolvedOffsets.has(name)) {
      throw new Error(`${name} has no offset. Add --part ${name}@<offset>, or remove the file`);
    }
  }

  const manifestPath = join(versionDir, 'manifest.json');
  const previous = existsSync(manifestPath)
    ? JSON.parse(readFileSync(manifestPath, 'utf8'))
    : {};

  const parts = binaries
    .map((name) => {
      const content = readFileSync(join(versionDir, name));
      const sha256 = createHash('sha256').update(content).digest('hex');
      return {
        path: name,
        offset: resolvedOffsets.get(name),
        size: content.length,
        sha256,
        ...carriedPartFields(previous, name, sha256),
      };
    })
    .sort((left, right) => left.offset - right.offset);

  for (let index = 1; index < parts.length; index += 1) {
    const previous = parts[index - 1];
    if (parts[index].offset < previous.offset + previous.size) {
      throw new Error(`${parts[index].path} at ${toHex(parts[index].offset)} overlaps ${previous.path}`);
    }
  }

  const variantLabel = firmware.flash?.variants?.find((entry) => entry.id === variant)?.label || variant;
  const manifest = {
    name: variant ? `${firmware.name} ${variantLabel}` : firmware.name,
    version,
    flashSize,
    ...carriedFlasherSettings(previous),
    builds: [
      {
        chipFamily: CHIP_FAMILY,
        parts,
      },
    ],
  };

  writeFileSync(manifestPath, `${JSON.stringify(manifest, null, 2)}\n`, 'utf8');
  return { manifest, manifestPath, parts };
}

// Keeps the flasher tuning values an earlier manifest already carried.
// 保留上一版 manifest 中已有的烧录参数。
function carriedFlasherSettings(previous) {
  const carried = {};
  for (const key of ['flashMode', 'flashFreq', 'baudRate', 'new_install_prompt_erase']) {
    if (previous[key] !== undefined) carried[key] = previous[key];
  }
  return carried;
}

// Keeps the MD5 of a part whose bytes did not change since the earlier manifest.
// 文件内容未变化时，保留上一版 manifest 中该分区的 MD5。
function carriedPartFields(previous, path, sha256) {
  const earlier = (previous.builds?.[0]?.parts || [])
    .find((part) => part.path === path && part.sha256 === sha256);
  return earlier?.md5 ? { md5: earlier.md5 } : {};
}

function toHex(offset) {
  return `0x${offset.toString(16)}`;
}

function parseArguments(argv) {
  const options = { parts: [], flashSize: DEFAULT_FLASH_SIZE };
  const positional = [];

  for (let index = 0; index < argv.length; index += 1) {
    const argument = argv[index];
    if (argument === '--part') {
      options.parts.push(argv[++index]);
    } else if (argument === '--flasher-args') {
      options.flasherArgsPath = argv[++index];
    } else if (argument === '--flash-size') {
      options.flashSize = argv[++index];
    } else if (argument === '--variant') {
      options.variant = argv[++index];
    } else if (argument.startsWith('--')) {
      throw new Error(`Unknown option ${argument}`);
    } else {
      positional.push(argument);
    }
  }

  const [firmwareId, version] = positional;
  if (!firmwareId || !version) {
    throw new Error(
      'Usage: npm run create:manifest -- <firmware-id> <version> [--variant <variant-id>] '
      + '[--part <file>.bin@<offset>]... [--flasher-args <path>] [--flash-size 16MB]',
    );
  }

  return { ...options, firmwareId, version };
}

function main() {
  const options = parseArguments(process.argv.slice(2));
  const rootDir = process.env.REGISTRY_ROOT
    ? resolve(process.env.REGISTRY_ROOT)
    : resolve(import.meta.dirname, '..');

  const flasherArgs = options.flasherArgsPath
    ? JSON.parse(readFileSync(resolve(options.flasherArgsPath), 'utf8'))
    : undefined;

  const { manifestPath, parts } = createFlashManifest({
    rootDir,
    firmwareId: options.firmwareId,
    version: options.version,
    variant: options.variant,
    offsets: parseOffsets({ parts: options.parts, flasherArgs }),
    flashSize: options.flashSize,
  });

  console.log(`Wrote ${manifestPath} with ${parts.length} firmware part(s):`);
  for (const part of parts) {
    console.log(`  ${toHex(part.offset).padStart(8)}  ${String(part.size).padStart(9)} bytes  ${part.path}`);
  }
  console.log('');
  const manifestPathEntry = `${packagePathFor(options.version, options.variant)}/manifest.json`;
  if (options.variant) {
    console.log(
      `Add this package to the variants list of version ${options.version} `
      + `in firmwares/${options.firmwareId}/firmware.json, and declare "${options.variant}" in flash.variants:`,
    );
    console.log(`${JSON.stringify({ id: options.variant, manifestPath: manifestPathEntry }, null, 2)}`);
    return;
  }
  console.log(`Add this version to firmwares/${options.firmwareId}/firmware.json under flash.versions:`);
  console.log(`${JSON.stringify({
    version: options.version,
    channel: 'stable',
    manifestPath: manifestPathEntry,
  }, null, 2)}`);
}

if (process.argv[1] && resolve(process.argv[1]) === resolve(import.meta.filename)) {
  main();
}
