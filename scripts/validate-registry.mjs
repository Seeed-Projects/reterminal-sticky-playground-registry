#!/usr/bin/env node

import {
  createHash,
} from 'node:crypto';
import {
  existsSync,
  lstatSync,
  readFileSync,
  readdirSync,
  realpathSync,
  statSync,
} from 'node:fs';
import { basename, dirname, extname, isAbsolute, join, relative, resolve, sep } from 'node:path';
import { fileURLToPath } from 'node:url';

const SCRIPT_DIR = dirname(fileURLToPath(import.meta.url));
const REPOSITORY_ROOT = process.env.REGISTRY_ROOT
  ? resolve(process.env.REGISTRY_ROOT)
  : resolve(SCRIPT_DIR, '..');
const INTEGRATIONS_DIR = join(REPOSITORY_ROOT, 'integrations');
const SCHEMA_PATH = join(REPOSITORY_ROOT, 'schemas', 'integration.schema.json');

const ALLOWED_GROUPS = new Set(['official', 'community']);
const ALLOWED_MODES = new Set(['external', 'template', 'download', 'flash']);
const ALLOWED_STATUSES = new Set(['experimental', 'beta', 'stable']);
const ALLOWED_CATALOG_SECTIONS = new Set(['official', 'platform', 'community', 'draft']);
const ALLOWED_BUILD_SYSTEMS = new Set(['esp-idf']);
const ALLOWED_ASSET_EXTENSIONS = new Set(['.png', '.jpg', '.jpeg', '.webp', '.svg']);
const ID_PATTERN = /^[a-z0-9]+(?:-[a-z0-9]+)*$/;
const SHA256_PATTERN = /^[a-f0-9]{64}$/;
const MODE_FIELDS = ['external', 'template', 'download', 'flash'];
const COMMON_FIELDS = new Set([
  'schemaVersion',
  'id',
  'name',
  'group',
  'catalogSection',
  'mode',
  'status',
  'summary',
  'description',
  'author',
  'origin',
  'source',
  'support',
  'documentationUrl',
  'compatibility',
  'assets',
  'tags',
  'build',
  ...MODE_FIELDS,
]);

const errors = [];

function addError(scope, message) {
  errors.push(`${scope}: ${message}`);
}

function isObject(value) {
  return value !== null && typeof value === 'object' && !Array.isArray(value);
}

function validateObjectKeys(value, requiredKeys, allowedKeys, scope) {
  if (!isObject(value)) {
    addError(scope, 'must be an object');
    return false;
  }

  for (const key of requiredKeys) {
    if (!(key in value)) {
      addError(scope, `missing required field "${key}"`);
    }
  }

  for (const key of Object.keys(value)) {
    if (!allowedKeys.has(key)) {
      addError(scope, `contains unsupported field "${key}"`);
    }
  }

  return true;
}

function validateString(value, scope, { min = 1, max = Infinity, pattern } = {}) {
  if (typeof value !== 'string') {
    addError(scope, 'must be a string');
    return false;
  }

  if (value.length < min || value.length > max) {
    addError(scope, `must contain between ${min} and ${max} characters`);
    return false;
  }

  if (pattern && !pattern.test(value)) {
    addError(scope, 'has an invalid format');
    return false;
  }

  return true;
}

function validateHttpsUrl(value, scope) {
  if (!validateString(value, scope, { max: 2048 })) {
    return false;
  }

  try {
    const url = new URL(value);
    if (url.protocol !== 'https:') {
      addError(scope, 'must use HTTPS');
      return false;
    }
  } catch {
    addError(scope, 'must be a valid absolute URL');
    return false;
  }

  return true;
}

function validateEnum(value, allowedValues, scope) {
  if (!allowedValues.has(value)) {
    addError(scope, `must be one of: ${[...allowedValues].join(', ')}`);
    return false;
  }
  return true;
}

function validateOptionalBoolean(value, scope) {
  if (value !== undefined && typeof value !== 'boolean') {
    addError(scope, 'must be a boolean');
  }
}

function validateAttribution(value, scope) {
  const allowedKeys = new Set(['name', 'url']);
  if (!validateObjectKeys(value, ['name'], allowedKeys, scope)) {
    return;
  }

  validateString(value.name, `${scope}.name`, { max: 80 });
  if (value.url !== undefined) {
    validateHttpsUrl(value.url, `${scope}.url`);
  }
}

function validateSource(value, scope) {
  const allowedKeys = new Set(['url', 'license', 'path']);
  if (!validateObjectKeys(value, ['url'], allowedKeys, scope)) {
    return;
  }

  validateHttpsUrl(value.url, `${scope}.url`);
  if (value.license !== undefined) {
    validateString(value.license, `${scope}.license`, { max: 80 });
  }
}

function validateLocalDirectoryPath(value, integrationDir, scope) {
  if (!validateString(value, scope, { max: 240 })) {
    return null;
  }
  if (isAbsolute(value) || value.split(/[\\/]/).includes('..')) {
    addError(scope, 'must stay inside the integration directory');
    return null;
  }

  const resolvedPath = resolve(integrationDir, value);
  if (!existsSync(resolvedPath) || !statSync(resolvedPath).isDirectory()) {
    addError(scope, `references a missing directory: ${value}`);
    return null;
  }
  if (lstatSync(resolvedPath).isSymbolicLink()) {
    addError(scope, 'must reference a regular directory, not a symbolic link');
    return null;
  }

  const realIntegrationDir = realpathSync(integrationDir);
  const realDirectoryPath = realpathSync(resolvedPath);
  if (!realDirectoryPath.startsWith(`${realIntegrationDir}${sep}`)) {
    addError(scope, 'must stay inside the integration directory');
    return null;
  }
  return resolvedPath;
}

function validateBuild(value, integrationDir, source, scope) {
  const allowedKeys = new Set(['system', 'version', 'target', 'projectPath']);
  if (!validateObjectKeys(value, ['system', 'version', 'target', 'projectPath'], allowedKeys, scope)) {
    return;
  }
  validateEnum(value.system, ALLOWED_BUILD_SYSTEMS, `${scope}.system`);
  validateString(value.version, `${scope}.version`, {
    max: 64,
    pattern: /^[A-Za-z0-9][A-Za-z0-9._-]*$/,
  });
  validateString(value.target, `${scope}.target`, { max: 40, pattern: ID_PATTERN });
  const projectPath = validateLocalDirectoryPath(value.projectPath, integrationDir, `${scope}.projectPath`);
  if (source?.path && value.projectPath !== source.path) {
    addError(`${scope}.projectPath`, 'must match source.path');
  }
  if (projectPath && !existsSync(join(projectPath, 'CMakeLists.txt'))) {
    addError(`${scope}.projectPath`, 'must contain CMakeLists.txt for an ESP-IDF project');
  }
}

function validateCommunityProjectFiles(integrationDir, sourcePath, scope) {
  validateLocalFilePath('README.md', integrationDir, `${scope}.README.md`);
  if (sourcePath) {
    validateLocalFilePath(
      join(sourcePath, 'LICENSE'),
      integrationDir,
      `${scope}.source.LICENSE`,
    );
  }
}

function validateSupport(value, scope) {
  const allowedKeys = new Set(['url']);
  if (!validateObjectKeys(value, ['url'], allowedKeys, scope)) {
    return;
  }
  validateHttpsUrl(value.url, `${scope}.url`);
}

function validateCompatibility(value, scope) {
  const allowedKeys = new Set(['devices', 'notes']);
  if (!validateObjectKeys(value, ['devices'], allowedKeys, scope)) {
    return;
  }

  if (
    !Array.isArray(value.devices)
    || value.devices.length !== 1
    || value.devices[0] !== 'reterminal-sticky'
  ) {
    addError(`${scope}.devices`, 'must be exactly ["reterminal-sticky"]');
  }

  if (value.notes !== undefined) {
    validateString(value.notes, `${scope}.notes`, { min: 0, max: 400 });
  }
}

// Resolves contributor-provided paths inside one integration directory.
// 在单个集成目录内解析贡献者提供的相对路径。
function validateLocalFilePath(value, integrationDir, scope, options = {}) {
  if (!validateString(value, scope, { max: 240 })) {
    return null;
  }

  if (isAbsolute(value) || value.split(/[\\/]/).includes('..')) {
    addError(scope, 'must stay inside the integration directory');
    return null;
  }

  const resolvedPath = resolve(integrationDir, value);
  const relativePath = relative(integrationDir, resolvedPath);
  if (relativePath.startsWith(`..${sep}`) || relativePath === '..') {
    addError(scope, 'must stay inside the integration directory');
    return null;
  }

  if (!existsSync(resolvedPath)) {
    addError(scope, `references a missing file: ${value}`);
    return null;
  }

  if (lstatSync(resolvedPath).isSymbolicLink()) {
    addError(scope, 'must reference a regular file, not a symbolic link');
    return null;
  }

  if (!statSync(resolvedPath).isFile()) {
    addError(scope, 'must reference a regular file');
    return null;
  }

  const realIntegrationDir = realpathSync(integrationDir);
  const realFilePath = realpathSync(resolvedPath);
  if (!realFilePath.startsWith(`${realIntegrationDir}${sep}`)) {
    addError(scope, 'must stay inside the integration directory');
    return null;
  }

  if (options.extensions && !options.extensions.has(extname(value).toLowerCase())) {
    addError(scope, `must use one of these file extensions: ${[...options.extensions].join(', ')}`);
  }

  if (options.maxBytes && statSync(resolvedPath).size > options.maxBytes) {
    addError(scope, `must not exceed ${Math.round(options.maxBytes / 1024 / 1024)} MB`);
  }

  return resolvedPath;
}

// Confirms that an asset matches its extension and contains static image data.
// 确认资源内容与后缀一致，并且只包含静态图片数据。
function validateAssetContent(filePath, scope) {
  if (!filePath) {
    return;
  }

  const extension = extname(filePath).toLowerCase();
  const content = readFileSync(filePath);
  const startsWith = (bytes) => (
    content.length >= bytes.length
    && bytes.every((byte, index) => content[index] === byte)
  );

  if (extension === '.png' && !startsWith([0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a])) {
    addError(scope, 'does not contain a valid PNG file signature');
    return;
  }

  if ((extension === '.jpg' || extension === '.jpeg') && !startsWith([0xff, 0xd8, 0xff])) {
    addError(scope, 'does not contain a valid JPEG file signature');
    return;
  }

  if (
    extension === '.webp'
    && (
      content.length < 12
      || content.toString('ascii', 0, 4) !== 'RIFF'
      || content.toString('ascii', 8, 12) !== 'WEBP'
    )
  ) {
    addError(scope, 'does not contain a valid WebP file signature');
    return;
  }

  if (extension === '.svg') {
    const svg = content.toString('utf8');
    if (!/<svg\b/i.test(svg)) {
      addError(scope, 'does not contain an SVG root element');
    }
    if (/<script\b/i.test(svg) || /<foreignObject\b/i.test(svg)) {
      addError(scope, 'must contain static SVG graphics only');
    }
    if (/\son[a-z]+\s*=/i.test(svg) || /javascript\s*:/i.test(svg)) {
      addError(scope, 'must not contain executable SVG event handlers');
    }
    if (/(?:href|xlink:href)\s*=\s*["']\s*(?:https?:|\/\/)/i.test(svg)) {
      addError(scope, 'must not load external SVG resources');
    }
  }
}

function validateAssets(value, integrationDir, scope) {
  const allowedKeys = new Set(['logo', 'preview', 'previewAlt']);
  if (!validateObjectKeys(value, ['preview', 'previewAlt'], allowedKeys, scope)) {
    return;
  }

  if (value.logo !== undefined) {
    const logoPath = validateLocalFilePath(value.logo, integrationDir, `${scope}.logo`, {
      extensions: ALLOWED_ASSET_EXTENSIONS,
      maxBytes: 1024 * 1024,
    });
    validateAssetContent(logoPath, `${scope}.logo`);
  }
  const previewPath = validateLocalFilePath(value.preview, integrationDir, `${scope}.preview`, {
    extensions: ALLOWED_ASSET_EXTENSIONS,
    maxBytes: 5 * 1024 * 1024,
  });
  validateAssetContent(previewPath, `${scope}.preview`);
  validateString(value.previewAlt, `${scope}.previewAlt`, { max: 180 });
}

function validateInstructions(value, scope) {
  if (!Array.isArray(value) || value.length === 0 || value.length > 12) {
    addError(scope, 'must be an array containing 1 to 12 steps');
    return;
  }

  value.forEach((step, index) => {
    const stepScope = `${scope}[${index}]`;
    const allowedKeys = new Set(['title', 'description']);
    if (!validateObjectKeys(step, ['title', 'description'], allowedKeys, stepScope)) {
      return;
    }
    validateString(step.title, `${stepScope}.title`, { max: 100 });
    validateString(step.description, `${stepScope}.description`, { max: 500 });
  });
}

function validateExternalMode(value, scope) {
  const allowedKeys = new Set(['label', 'url', 'description']);
  if (!validateObjectKeys(value, ['label', 'url', 'description'], allowedKeys, scope)) {
    return;
  }

  validateString(value.label, `${scope}.label`, { max: 60 });
  validateHttpsUrl(value.url, `${scope}.url`);
  validateString(value.description, `${scope}.description`, { max: 400 });
}

function validateTemplateMode(value, integrationDir, scope) {
  const allowedKeys = new Set([
    'outputExtension',
    'mimeType',
    'fileNamePattern',
    'headerPath',
    'footerPath',
    'options',
  ]);
  const requiredKeys = ['outputExtension', 'mimeType', 'fileNamePattern', 'options'];
  if (!validateObjectKeys(value, requiredKeys, allowedKeys, scope)) {
    return;
  }

  validateString(value.outputExtension, `${scope}.outputExtension`, {
    max: 12,
    pattern: /^[a-z0-9]+$/,
  });
  validateString(value.mimeType, `${scope}.mimeType`, {
    max: 80,
    pattern: /^[a-z0-9.+-]+\/[a-z0-9.+-]+$/,
  });
  validateString(value.fileNamePattern, `${scope}.fileNamePattern`, { max: 120 });

  if (value.headerPath !== undefined) {
    validateLocalFilePath(value.headerPath, integrationDir, `${scope}.headerPath`);
  }
  if (value.footerPath !== undefined) {
    validateLocalFilePath(value.footerPath, integrationDir, `${scope}.footerPath`);
  }

  if (!Array.isArray(value.options) || value.options.length === 0) {
    addError(`${scope}.options`, 'must contain at least one template option');
    return;
  }

  const optionIds = new Set();
  value.options.forEach((option, index) => {
    const optionScope = `${scope}.options[${index}]`;
    const allowedOptionKeys = new Set([
      'id',
      'label',
      'description',
      'path',
      'required',
      'defaultSelected',
    ]);
    if (
      !validateObjectKeys(
        option,
        ['id', 'label', 'description', 'path'],
        allowedOptionKeys,
        optionScope,
      )
    ) {
      return;
    }

    if (validateString(option.id, `${optionScope}.id`, { max: 64, pattern: ID_PATTERN })) {
      if (optionIds.has(option.id)) {
        addError(`${optionScope}.id`, `duplicates template option "${option.id}"`);
      }
      optionIds.add(option.id);
    }

    validateString(option.label, `${optionScope}.label`, { max: 80 });
    validateString(option.description, `${optionScope}.description`, { max: 300 });
    validateLocalFilePath(option.path, integrationDir, `${optionScope}.path`);
    validateOptionalBoolean(option.required, `${optionScope}.required`);
    validateOptionalBoolean(option.defaultSelected, `${optionScope}.defaultSelected`);
  });
}

function validateDownloadMode(value, scope) {
  const allowedKeys = new Set(['url', 'version', 'fileName', 'sha256', 'steps']);
  if (!validateObjectKeys(value, ['url', 'steps'], allowedKeys, scope)) {
    return;
  }

  validateHttpsUrl(value.url, `${scope}.url`);
  if (value.version !== undefined) {
    validateString(value.version, `${scope}.version`, { max: 40 });
  }
  if (value.fileName !== undefined) {
    validateString(value.fileName, `${scope}.fileName`, { max: 120 });
  }
  if (value.sha256 !== undefined) {
    validateString(value.sha256, `${scope}.sha256`, { min: 64, max: 64, pattern: SHA256_PATTERN });
  }
  validateInstructions(value.steps, `${scope}.steps`);
}

function validateLocalFlashManifest(manifestPath, integrationDir, version, scope) {
  if (typeof manifestPath === 'string' && !manifestPath.startsWith('firmware/')) {
    addError(scope, 'must be inside the firmware/ directory');
  }
  const resolvedManifestPath = validateLocalFilePath(manifestPath, integrationDir, scope, {
    extensions: new Set(['.json']),
    maxBytes: 1024 * 1024,
  });
  if (!resolvedManifestPath) return;

  let manifest;
  try {
    manifest = JSON.parse(readFileSync(resolvedManifestPath, 'utf8'));
  } catch (error) {
    addError(scope, `contains invalid JSON (${error.message})`);
    return;
  }
  if (manifest.version !== version) {
    addError(scope, `version must match firmware version "${version}"`);
  }
  if (!Array.isArray(manifest.builds) || manifest.builds.length === 0) {
    addError(scope, 'must contain at least one build');
    return;
  }

  for (const [buildIndex, build] of manifest.builds.entries()) {
    const buildScope = `${scope}.builds[${buildIndex}]`;
    if (!Array.isArray(build.parts) || build.parts.length === 0) {
      addError(buildScope, 'must contain at least one firmware part');
      continue;
    }
    const ranges = [];
    for (const [partIndex, part] of build.parts.entries()) {
      const partScope = `${buildScope}.parts[${partIndex}]`;
      if (!isObject(part)) {
        addError(partScope, 'must be an object');
        continue;
      }
      if (!Number.isInteger(part.offset) || part.offset < 0) {
        addError(`${partScope}.offset`, 'must be a non-negative integer');
      }
      if (!Number.isInteger(part.size) || part.size <= 0) {
        addError(`${partScope}.size`, 'must be a positive integer');
      }
      if (!validateString(part.path, `${partScope}.path`, { max: 120 })) {
        continue;
      }
      if (part.path !== basename(part.path) || extname(part.path).toLowerCase() !== '.bin') {
        addError(`${partScope}.path`, 'must be one .bin filename beside the manifest');
        continue;
      }
      if (!validateString(part.sha256, `${partScope}.sha256`, { min: 64, max: 64, pattern: SHA256_PATTERN })) {
        continue;
      }
      const binaryPath = validateLocalFilePath(
        join(dirname(manifestPath), part.path),
        integrationDir,
        `${partScope}.path`,
        { extensions: new Set(['.bin']), maxBytes: 32 * 1024 * 1024 },
      );
      if (!binaryPath) continue;
      const content = readFileSync(binaryPath);
      if (content.length !== part.size) {
        addError(`${partScope}.size`, `expected ${part.size} bytes but found ${content.length}`);
      }
      const actualHash = createHash('sha256').update(content).digest('hex');
      if (actualHash !== part.sha256) {
        addError(`${partScope}.sha256`, 'does not match the firmware file');
      }
      if (Number.isInteger(part.offset) && Number.isInteger(part.size) && part.size > 0) {
        ranges.push({ start: part.offset, end: part.offset + part.size, partScope });
      }
    }
    ranges.sort((a, b) => a.start - b.start);
    for (let index = 1; index < ranges.length; index += 1) {
      if (ranges[index].start < ranges[index - 1].end) {
        addError(ranges[index].partScope, 'overlaps the previous firmware part');
      }
    }
  }
}

function validateFlashMode(value, integrationDir, scope) {
  const allowedKeys = new Set(['versions', 'notes']);
  if (!validateObjectKeys(value, ['versions'], allowedKeys, scope)) {
    return;
  }

  if (!Array.isArray(value.versions) || value.versions.length === 0) {
    addError(`${scope}.versions`, 'must contain at least one firmware version');
    return;
  }

  const versions = new Set();
  value.versions.forEach((entry, index) => {
    const versionScope = `${scope}.versions[${index}]`;
    const allowedVersionKeys = new Set([
      'version',
      'channel',
      'manifestUrl',
      'manifestSha256',
      'releaseUrl',
      'manifestPath',
      'sourceBuild',
    ]);
    const requiredVersionKeys = ['version', 'channel'];
    if (!validateObjectKeys(entry, requiredVersionKeys, allowedVersionKeys, versionScope)) {
      return;
    }

    if (validateString(entry.version, `${versionScope}.version`, { max: 40 })) {
      if (versions.has(entry.version)) {
        addError(`${versionScope}.version`, `duplicates firmware version "${entry.version}"`);
      }
      versions.add(entry.version);
    }

    validateEnum(entry.channel, ALLOWED_STATUSES, `${versionScope}.channel`);
    const deliveryFields = [
      entry.manifestPath !== undefined,
      entry.manifestUrl !== undefined || entry.manifestSha256 !== undefined || entry.releaseUrl !== undefined,
      entry.sourceBuild === true,
    ].filter(Boolean).length;
    if (deliveryFields !== 1) {
      addError(versionScope, 'must use exactly one firmware delivery method');
      return;
    }

    if (entry.manifestPath !== undefined) {
      validateLocalFlashManifest(entry.manifestPath, integrationDir, entry.version, `${versionScope}.manifestPath`);
    } else if (entry.sourceBuild === true) {
      // GitHub Actions creates the manifest and firmware release from source.
      // GitHub Actions 会根据源码生成 manifest 和固件 Release。
    } else {
      validateHttpsUrl(entry.manifestUrl, `${versionScope}.manifestUrl`);
      validateString(entry.manifestSha256, `${versionScope}.manifestSha256`, {
        min: 64,
        max: 64,
        pattern: SHA256_PATTERN,
      });
      validateHttpsUrl(entry.releaseUrl, `${versionScope}.releaseUrl`);
    }
  });

  if (value.notes !== undefined) {
    validateInstructions(value.notes, `${scope}.notes`);
  }
}

// Validates one production integration and all local files referenced by it.
// 校验一个正式集成条目及其引用的全部本地文件。
function validateIntegration(integrationDir, directoryName, seenIds) {
  const metadataPath = join(integrationDir, 'integration.json');
  const scope = `integrations/${directoryName}/integration.json`;

  if (!existsSync(metadataPath)) {
    addError(`integrations/${directoryName}`, 'is missing integration.json');
    return;
  }

  let integration;
  try {
    integration = JSON.parse(readFileSync(metadataPath, 'utf8'));
  } catch (error) {
    addError(scope, `contains invalid JSON (${error.message})`);
    return;
  }

  const requiredFields = [
    'schemaVersion',
    'id',
    'name',
    'group',
    'catalogSection',
    'mode',
    'status',
    'summary',
    'description',
    'author',
    'source',
    'support',
    'compatibility',
    'assets',
  ];
  if (!validateObjectKeys(integration, requiredFields, COMMON_FIELDS, scope)) {
    return;
  }

  if (integration.schemaVersion !== 1) {
    addError(`${scope}.schemaVersion`, 'must be 1');
  }

  if (validateString(integration.id, `${scope}.id`, { min: 2, max: 64, pattern: ID_PATTERN })) {
    if (integration.id !== directoryName) {
      addError(`${scope}.id`, `must match the directory name "${directoryName}"`);
    }
    if (seenIds.has(integration.id)) {
      addError(`${scope}.id`, `duplicates integration id "${integration.id}"`);
    }
    seenIds.add(integration.id);
  }

  validateString(integration.name, `${scope}.name`, { max: 80 });
  validateEnum(integration.group, ALLOWED_GROUPS, `${scope}.group`);
  validateEnum(integration.catalogSection, ALLOWED_CATALOG_SECTIONS, `${scope}.catalogSection`);
  validateEnum(integration.mode, ALLOWED_MODES, `${scope}.mode`);
  validateEnum(integration.status, ALLOWED_STATUSES, `${scope}.status`);
  validateString(integration.summary, `${scope}.summary`, { max: 140 });
  validateString(integration.description, `${scope}.description`, { max: 800 });
  validateAttribution(integration.author, `${scope}.author`);
  if (integration.origin !== undefined) {
    validateAttribution(integration.origin, `${scope}.origin`);
  }
  validateSource(integration.source, `${scope}.source`);
  if (integration.source?.path !== undefined) {
    validateLocalDirectoryPath(integration.source.path, integrationDir, `${scope}.source.path`);
  }
  validateSupport(integration.support, `${scope}.support`);
  validateCompatibility(integration.compatibility, `${scope}.compatibility`);
  validateAssets(integration.assets, integrationDir, `${scope}.assets`);

  if (integration.documentationUrl !== undefined) {
    validateHttpsUrl(integration.documentationUrl, `${scope}.documentationUrl`);
  }

  if (integration.tags !== undefined) {
    if (!Array.isArray(integration.tags) || integration.tags.length > 6) {
      addError(`${scope}.tags`, 'must be an array containing no more than 6 tags');
    } else {
      const tags = new Set();
      integration.tags.forEach((tag, index) => {
        if (validateString(tag, `${scope}.tags[${index}]`, { max: 32 })) {
          if (tags.has(tag)) {
            addError(`${scope}.tags[${index}]`, `duplicates tag "${tag}"`);
          }
          tags.add(tag);
        }
      });
    }
  }

  if (integration.build !== undefined) {
    validateBuild(integration.build, integrationDir, integration.source, `${scope}.build`);
  }

  if (integration.catalogSection === 'community') {
    validateCommunityProjectFiles(integrationDir, integration.source?.path, scope);
    const hasLocalSource = Boolean(integration.source?.path);
    const hasBuildConfig = integration.build !== undefined;
    if (integration.group !== 'community') {
      addError(`${scope}.group`, 'must be "community" for the community catalog section');
    }
    if (integration.mode !== 'flash') {
      addError(`${scope}.mode`, 'must be "flash" for the community catalog section');
    }
    if (hasBuildConfig && !hasLocalSource) {
      addError(`${scope}.source.path`, 'is required when build is provided');
    }
    if (hasLocalSource && !hasBuildConfig) {
      addError(`${scope}.build`, 'is required when source.path is provided');
    }
    if (!hasLocalSource && !integration.source?.license) {
      addError(`${scope}.source.license`, 'is required for firmware-only contributions');
    }
    const versions = integration.flash?.versions || [];
    if (hasLocalSource && versions[0]?.sourceBuild !== true) {
      addError(`${scope}.flash.versions[0].sourceBuild`, 'must be true for a source contribution');
    }
    if (!hasLocalSource) {
      for (const [index, version] of versions.entries()) {
        if (!version.manifestPath) {
          addError(`${scope}.flash.versions[${index}].manifestPath`, 'is required for a firmware-only contribution');
        }
      }
    }
  }

  for (const modeField of MODE_FIELDS) {
    if (modeField === integration.mode) {
      if (!(modeField in integration)) {
        addError(scope, `mode "${integration.mode}" requires the "${modeField}" object`);
      }
    } else if (modeField in integration) {
      addError(scope, `mode "${integration.mode}" cannot include the "${modeField}" object`);
    }
  }

  if (integration.mode === 'external' && integration.external !== undefined) {
    validateExternalMode(integration.external, `${scope}.external`);
  }
  if (integration.mode === 'template' && integration.template !== undefined) {
    validateTemplateMode(integration.template, integrationDir, `${scope}.template`);
  }
  if (integration.mode === 'download' && integration.download !== undefined) {
    validateDownloadMode(integration.download, `${scope}.download`);
  }
  if (integration.mode === 'flash' && integration.flash !== undefined) {
    validateFlashMode(integration.flash, integrationDir, `${scope}.flash`);
  }
}

function main() {
  try {
    JSON.parse(readFileSync(SCHEMA_PATH, 'utf8'));
  } catch (error) {
    addError('schemas/integration.schema.json', `contains invalid JSON (${error.message})`);
  }

  if (!existsSync(INTEGRATIONS_DIR)) {
    addError('integrations', 'directory is missing');
  }

  const integrationDirectories = existsSync(INTEGRATIONS_DIR)
    ? readdirSync(INTEGRATIONS_DIR, { withFileTypes: true })
      .filter((entry) => entry.isDirectory() && !entry.name.startsWith('_') && !entry.name.startsWith('.'))
      .sort((a, b) => a.name.localeCompare(b.name))
    : [];

  const seenIds = new Set();
  for (const entry of integrationDirectories) {
    validateIntegration(join(INTEGRATIONS_DIR, entry.name), entry.name, seenIds);
  }

  if (errors.length > 0) {
    console.error(`Registry validation failed with ${errors.length} error(s):`);
    errors.forEach((error) => console.error(`- ${error}`));
    process.exitCode = 1;
    return;
  }

  console.log(`Registry validation passed (${integrationDirectories.length} integration(s)).`);
}

main();
