import assert from 'node:assert/strict';
import {
  mkdirSync,
  mkdtempSync,
  rmSync,
  writeFileSync,
} from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join } from 'node:path';
import { spawnSync } from 'node:child_process';
import test from 'node:test';
import { fileURLToPath } from 'node:url';

const SCRIPT_DIR = dirname(fileURLToPath(import.meta.url));
const LIST_SCRIPT = join(SCRIPT_DIR, 'list-build-targets.mjs');

function writeIntegration(root, integration) {
  const integrationDir = join(root, 'firmwares', integration.id);
  mkdirSync(integrationDir, { recursive: true });
  writeFileSync(
    join(integrationDir, 'firmware.json'),
    `${JSON.stringify(integration, null, 2)}\n`,
  );
}

function writeSourceIntegration(root, id, name) {
  writeIntegration(root, {
    id,
    name,
    catalogSection: 'community',
    build: {
      system: 'esp-idf',
      version: 'v5.3.2',
      target: 'esp32s3',
      projectPath: 'source',
    },
    flash: {
      versions: [{ version: '1.0.0', sourceBuild: true }],
    },
  });
}

function runList(root, changedFiles) {
  const result = spawnSync(process.execPath, [LIST_SCRIPT], {
    encoding: 'utf8',
    env: {
      ...process.env,
      REGISTRY_ROOT: root,
      ...(changedFiles === undefined ? {} : { CHANGED_FILES: changedFiles }),
    },
  });
  assert.equal(result.status, 0, result.stderr);
  return JSON.parse(result.stdout).include.map((target) => target.id);
}

test('lists only publishable source-built integrations', () => {
  const root = mkdtempSync(join(tmpdir(), 'sticky-target-list-test-'));
  try {
    writeIntegration(root, {
      id: 'source-project',
      name: 'Source Project',
      catalogSection: 'community',
      build: {
        system: 'esp-idf',
        version: 'v5.3.2',
        target: 'esp32s3',
        projectPath: 'source',
      },
      flash: {
        versions: [{ version: '2.0.0 RC1', sourceBuild: true }],
      },
    });
    writeIntegration(root, {
      id: 'firmware-only',
      name: 'Firmware Only',
      catalogSection: 'community',
      flash: {
        versions: [{
          version: '1.0.0',
          manifestPath: 'firmware/1.0.0/manifest.json',
        }],
      },
    });
    writeIntegration(root, {
      id: 'draft-source',
      name: 'Draft Source',
      catalogSection: 'draft',
      build: {
        system: 'esp-idf',
        version: 'v5.4.1',
        target: 'esp32s3',
        projectPath: 'source',
      },
      flash: {
        versions: [{ version: '1.0.0', sourceBuild: true }],
      },
    });

    const result = spawnSync(process.execPath, [LIST_SCRIPT], {
      encoding: 'utf8',
      env: { ...process.env, REGISTRY_ROOT: root },
    });
    assert.equal(result.status, 0, result.stderr);
    assert.deepEqual(JSON.parse(result.stdout), {
      include: [{
        id: 'source-project',
        name: 'Source Project',
        path: 'firmwares/source-project/source',
        idfVersion: 'v5.3.2',
        target: 'esp32s3',
        version: '2.0.0 RC1',
        versionSlug: '2.0.0-rc1',
      }],
    });
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});

test('narrows the target list to the firmwares a change touches', () => {
  const root = mkdtempSync(join(tmpdir(), 'sticky-target-list-test-'));
  try {
    writeSourceIntegration(root, 'alpha', 'Alpha');
    writeSourceIntegration(root, 'beta', 'Beta');

    assert.deepEqual(runList(root, undefined), ['alpha', 'beta']);
    assert.deepEqual(runList(root, ''), ['alpha', 'beta']);
    assert.deepEqual(
      runList(root, 'printables/sticky-case/printable.json\nprintables/sticky-case/README.md'),
      [],
    );
    assert.deepEqual(runList(root, 'firmwares/beta/source/main/main.c'), ['beta']);
    assert.deepEqual(runList(root, 'scripts/package-esp-idf.mjs'), ['alpha', 'beta']);
    assert.deepEqual(runList(root, '.github/workflows/validate-registry.yml'), ['alpha', 'beta']);
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});
