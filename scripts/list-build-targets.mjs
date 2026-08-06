#!/usr/bin/env node

import { readFileSync, readdirSync } from 'node:fs';
import { join, resolve } from 'node:path';

const ROOT_DIR = process.env.REGISTRY_ROOT
  ? resolve(process.env.REGISTRY_ROOT)
  : resolve(import.meta.dirname, '..');
const INTEGRATIONS_DIR = join(ROOT_DIR, 'integrations');

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
      join(INTEGRATIONS_DIR, entry.name, 'integration.json'),
      'utf8',
    ));
    const version = integration.flash?.versions?.[0];
    if (
      integration.build?.system !== 'esp-idf'
      || integration.catalogSection === 'draft'
      || version?.sourceBuild !== true
    ) return null;
    return {
      id: integration.id,
      name: integration.name,
      path: `integrations/${integration.id}/${integration.build.projectPath}`,
      idfVersion: integration.build.version,
      target: integration.build.target,
      version: version.version,
      versionSlug: slugifyVersion(version.version),
    };
  })
  .filter(Boolean);

process.stdout.write(JSON.stringify({ include: targets }));
