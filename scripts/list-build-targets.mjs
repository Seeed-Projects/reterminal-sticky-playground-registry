#!/usr/bin/env node

import { readFileSync, readdirSync } from 'node:fs';
import { join, resolve } from 'node:path';

const ROOT_DIR = resolve(import.meta.dirname, '..');
const INTEGRATIONS_DIR = join(ROOT_DIR, 'integrations');

const targets = readdirSync(INTEGRATIONS_DIR, { withFileTypes: true })
  .filter((entry) => entry.isDirectory() && !entry.name.startsWith('_'))
  .map((entry) => {
    const integration = JSON.parse(readFileSync(
      join(INTEGRATIONS_DIR, entry.name, 'integration.json'),
      'utf8',
    ));
    if (integration.build?.system !== 'esp-idf') return null;
    return {
      id: integration.id,
      path: `integrations/${integration.id}/${integration.build.projectPath}`,
      idfVersion: integration.build.version,
      target: integration.build.target,
    };
  })
  .filter(Boolean);

process.stdout.write(JSON.stringify({ include: targets }));
