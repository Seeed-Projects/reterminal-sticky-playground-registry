#!/usr/bin/env node

import { readFileSync, readdirSync } from 'node:fs';
import { join, resolve } from 'node:path';

const ROOT_DIR = process.env.REGISTRY_ROOT
  ? resolve(process.env.REGISTRY_ROOT)
  : resolve(import.meta.dirname, '..');
const INTEGRATIONS_DIR = join(ROOT_DIR, 'firmwares');

// Paths that affect every firmware build rather than a single project.
// 影响所有固件构建的路径，而不是只影响某一个项目。
const SHARED_BUILD_PATHS = [
  'scripts/',
  'package.json',
  'package-lock.json',
  '.github/workflows/validate-registry.yml',
];

const changedFiles = (process.env.CHANGED_FILES || '')
  .split('\n')
  .map((line) => line.trim())
  .filter(Boolean);

const buildsEverything = changedFiles.length === 0
  || changedFiles.some((file) => SHARED_BUILD_PATHS.some((prefix) => file.startsWith(prefix)));

// Reports whether the change set reaches the given firmware directory.
// 判断本次改动是否涉及指定固件的目录。
function isTouched(id) {
  return buildsEverything || changedFiles.some((file) => file.startsWith(`firmwares/${id}/`));
}

function slugifyVersion(value) {
  const slug = String(value)
    .trim()
    .toLowerCase()
    .replace(/[^a-z0-9.]+/g, '-')
    .replace(/^-+|-+$/g, '');
  if (!slug || !/^[a-z0-9][a-z0-9.-]*$/.test(slug)) {
    throw new Error(`Cannot create a safe version slug from "${value}"`);
  }
  return slug;
}

const targets = readdirSync(INTEGRATIONS_DIR, { withFileTypes: true })
  .filter((entry) => entry.isDirectory() && !entry.name.startsWith('_'))
  .map((entry) => {
    const integration = JSON.parse(readFileSync(
      join(INTEGRATIONS_DIR, entry.name, 'firmware.json'),
      'utf8',
    ));
    const version = integration.flash?.versions?.[0];
    if (
      integration.build?.system !== 'esp-idf'
      || integration.catalogSection === 'draft'
      || version?.sourceBuild !== true
      || !isTouched(integration.id)
    ) return null;
    return {
      id: integration.id,
      name: integration.name,
      path: `firmwares/${integration.id}/${integration.build.projectPath}`,
      idfVersion: integration.build.version,
      target: integration.build.target,
      version: version.version,
      versionSlug: slugifyVersion(version.version),
    };
  })
  .filter(Boolean);

process.stdout.write(JSON.stringify({ include: targets }));
