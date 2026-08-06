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
  const integrationDir = join(root, 'integrations', integration.id);
  mkdirSync(integrationDir, { recursive: true });
  writeFileSync(
    join(integrationDir, 'integration.json'),
    `${JSON.stringify(integration, null, 2)}\n`,
  );
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
        path: 'integrations/source-project/source',
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
