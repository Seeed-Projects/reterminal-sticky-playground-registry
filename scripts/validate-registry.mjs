#!/usr/bin/env node

import {
  existsSync,
  lstatSync,
  readFileSync,
  readdirSync,
  realpathSync,
  statSync,
} from 'node:fs';
import { dirname, extname, isAbsolute, join, relative, resolve, sep } from 'node:path';
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
const ALLOWED_ASSET_EXTENSIONS = new Set(['.png', '.jpg', '.jpeg', '.webp', '.svg']);
const ID_PATTERN = /^[a-z0-9]+(?:-[a-z0-9]+)*$/;
const SHA256_PATTERN = /^[a-f0-9]{64}$/;
const MODE_FIELDS = ['external', 'template', 'download', 'flash'];
const COMMON_FIELDS = new Set([
  'schemaVersion',
  'id',
  'name',
  'group',
  'mode',
  'status',
  'summary',
  'description',
  'author',
  'source',
  'support',
  'documentationUrl',
  'compatibility',
  'assets',
  'tags',
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
  if (!validateObjectKeys(value, ['name', 'url'], allowedKeys, scope)) {
    return;
  }

  validateString(value.name, `${scope}.name`, { max: 80 });
  validateHttpsUrl(value.url, `${scope}.url`);
}

function validateSource(value, scope) {
  const allowedKeys = new Set(['url', 'license']);
  if (!validateObjectKeys(value, ['url'], allowedKeys, scope)) {
    return;
  }

  validateHttpsUrl(value.url, `${scope}.url`);
  if (value.license !== undefined) {
    validateString(value.license, `${scope}.license`, { max: 80 });
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
  if (!validateObjectKeys(value, ['logo', 'preview', 'previewAlt'], allowedKeys, scope)) {
    return;
  }

  const logoPath = validateLocalFilePath(value.logo, integrationDir, `${scope}.logo`, {
    extensions: ALLOWED_ASSET_EXTENSIONS,
    maxBytes: 1024 * 1024,
  });
  const previewPath = validateLocalFilePath(value.preview, integrationDir, `${scope}.preview`, {
    extensions: ALLOWED_ASSET_EXTENSIONS,
    maxBytes: 5 * 1024 * 1024,
  });
  validateAssetContent(logoPath, `${scope}.logo`);
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

function validateFlashMode(value, scope) {
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
    ]);
    const requiredVersionKeys = [
      'version',
      'channel',
      'manifestUrl',
      'manifestSha256',
      'releaseUrl',
    ];
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
    validateHttpsUrl(entry.manifestUrl, `${versionScope}.manifestUrl`);
    validateString(entry.manifestSha256, `${versionScope}.manifestSha256`, {
      min: 64,
      max: 64,
      pattern: SHA256_PATTERN,
    });
    validateHttpsUrl(entry.releaseUrl, `${versionScope}.releaseUrl`);
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
  validateEnum(integration.mode, ALLOWED_MODES, `${scope}.mode`);
  validateEnum(integration.status, ALLOWED_STATUSES, `${scope}.status`);
  validateString(integration.summary, `${scope}.summary`, { max: 140 });
  validateString(integration.description, `${scope}.description`, { max: 800 });
  validateAttribution(integration.author, `${scope}.author`);
  validateSource(integration.source, `${scope}.source`);
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
    validateFlashMode(integration.flash, `${scope}.flash`);
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
