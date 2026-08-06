import assert from 'node:assert/strict';
import { createHash } from 'node:crypto';
import {
  mkdirSync,
  mkdtempSync,
  renameSync,
  rmSync,
  writeFileSync,
} from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join } from 'node:path';
import { spawnSync } from 'node:child_process';
import test from 'node:test';
import { fileURLToPath } from 'node:url';

const TEST_DIR = dirname(fileURLToPath(import.meta.url));
const VALIDATOR_PATH = join(TEST_DIR, 'validate-registry.mjs');
const TEMP_ROOTS = [];

function createRegistry() {
  const root = mkdtempSync(join(tmpdir(), 'sticky-registry-test-'));
  TEMP_ROOTS.push(root);
  mkdirSync(join(root, 'integrations'), { recursive: true });
  mkdirSync(join(root, 'schemas'), { recursive: true });
  writeFileSync(join(root, 'schemas', 'integration.schema.json'), '{}\n');
  return root;
}

function validBase(id, mode) {
  return {
    schemaVersion: 1,
    id,
    name: 'Example Platform',
    group: 'community',
    catalogSection: 'platform',
    mode,
    status: 'experimental',
    summary: 'A test integration for reTerminal Sticky.',
    description: 'This entry is generated in a temporary directory for validator regression tests.',
    author: {
      name: 'Example Team',
      url: 'https://github.com/example',
    },
    source: {
      url: 'https://github.com/example/platform',
      license: 'MIT',
    },
    support: {
      url: 'https://github.com/example/platform/issues',
    },
    documentationUrl: 'https://github.com/example/platform#readme',
    compatibility: {
      devices: ['reterminal-sticky'],
    },
    assets: {
      logo: 'assets/logo.svg',
      preview: 'assets/preview.webp',
      previewAlt: 'Example Platform running on reTerminal Sticky',
    },
    tags: ['test'],
  };
}

function writeIntegration(root, integration) {
  const integrationDir = join(root, 'integrations', integration.id);
  mkdirSync(join(integrationDir, 'assets'), { recursive: true });
  writeFileSync(
    join(integrationDir, 'assets', 'logo.svg'),
    '<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1 1"><path d="M0 0h1v1H0z"/></svg>\n',
  );
  writeFileSync(
    join(integrationDir, 'assets', 'preview.webp'),
    Buffer.concat([
      Buffer.from('RIFF', 'ascii'),
      Buffer.from([0x04, 0x00, 0x00, 0x00]),
      Buffer.from('WEBP', 'ascii'),
    ]),
  );
  writeFileSync(
    join(integrationDir, 'integration.json'),
    `${JSON.stringify(integration, null, 2)}\n`,
  );
  return integrationDir;
}

function runValidator(root) {
  return spawnSync(process.execPath, [VALIDATOR_PATH], {
    encoding: 'utf8',
    env: {
      ...process.env,
      REGISTRY_ROOT: root,
    },
  });
}

test.after(() => {
  for (const root of TEMP_ROOTS) {
    rmSync(root, { recursive: true, force: true });
  }
});

test('accepts an empty production registry', () => {
  const root = createRegistry();
  const result = runValidator(root);

  assert.equal(result.status, 0);
  assert.match(result.stdout, /Registry validation passed \(0 integration\(s\)\)\./);
});

test('accepts an integration without an optional logo', () => {
  const root = createRegistry();
  const integration = validBase('preview-only', 'external');
  delete integration.assets.logo;
  integration.external = {
    label: 'Open official tool',
    url: 'https://example.com/tool',
    description: 'Continue in the maintained upstream tool.',
  };
  writeIntegration(root, integration);

  const result = runValidator(root);

  assert.equal(result.status, 0);
  assert.match(result.stdout, /Registry validation passed \(1 integration\(s\)\)\./);
});

test('accepts a valid external integration', () => {
  const root = createRegistry();
  const integration = validBase('external-platform', 'external');
  integration.external = {
    label: 'Open official tool',
    url: 'https://example.com/tool',
    description: 'Continue in the maintained upstream tool.',
  };
  writeIntegration(root, integration);

  const result = runValidator(root);

  assert.equal(result.status, 0);
  assert.match(result.stdout, /Registry validation passed \(1 integration\(s\)\)\./);
});

test('rejects an unsafe ESP-IDF Docker tag', () => {
  const root = createRegistry();
  const integration = validBase('unsafe-idf-tag', 'external');
  integration.source.path = 'source';
  integration.build = {
    system: 'esp-idf',
    version: 'v5.4;echo',
    target: 'esp32s3',
    projectPath: 'source',
  };
  integration.external = {
    label: 'Open official tool',
    url: 'https://example.com/tool',
    description: 'Continue in the maintained upstream tool.',
  };
  const integrationDir = writeIntegration(root, integration);
  mkdirSync(join(integrationDir, 'source'), { recursive: true });
  writeFileSync(join(integrationDir, 'source', 'CMakeLists.txt'), 'project(example)\n');

  const result = runValidator(root);

  assert.equal(result.status, 1);
  assert.match(result.stderr, /build\.version: has an invalid format/);
});

test('accepts a valid template integration with local fragments', () => {
  const root = createRegistry();
  const integration = validBase('template-platform', 'template');
  integration.template = {
    outputExtension: 'yaml',
    mimeType: 'text/yaml',
    fileNamePattern: '{integrationId}-{deviceId}',
    headerPath: 'templates/header.yaml',
    options: [
      {
        id: 'display',
        label: 'Sticky display',
        description: 'Adds the required display configuration.',
        path: 'templates/display.yaml',
        required: true,
        defaultSelected: true,
      },
    ],
  };
  const integrationDir = writeIntegration(root, integration);
  mkdirSync(join(integrationDir, 'templates'), { recursive: true });
  writeFileSync(join(integrationDir, 'templates', 'header.yaml'), 'device: reterminal-sticky\n');
  writeFileSync(join(integrationDir, 'templates', 'display.yaml'), 'display: enabled\n');

  const result = runValidator(root);

  assert.equal(result.status, 0);
});

test('accepts a valid download integration', () => {
  const root = createRegistry();
  const integration = validBase('download-platform', 'download');
  integration.download = {
    url: 'https://github.com/example/platform/releases/download/v1.0.0/project.zip',
    version: '1.0.0',
    fileName: 'project.zip',
    sha256: 'a'.repeat(64),
    steps: [
      {
        title: 'Download the project',
        description: 'Download and extract the versioned project archive.',
      },
    ],
  };
  writeIntegration(root, integration);

  const result = runValidator(root);

  assert.equal(result.status, 0);
});

test('accepts a valid flash integration', () => {
  const root = createRegistry();
  const integration = validBase('flash-platform', 'flash');
  integration.flash = {
    versions: [
      {
        version: '1.0.0',
        channel: 'stable',
        manifestUrl: 'https://github.com/example/platform/releases/download/v1.0.0/manifest.json',
        manifestSha256: 'b'.repeat(64),
        releaseUrl: 'https://github.com/example/platform/releases/tag/v1.0.0',
      },
    ],
  };
  writeIntegration(root, integration);

  const result = runValidator(root);

  assert.equal(result.status, 0);
});

test('accepts a community source contribution without committed firmware', () => {
  const root = createRegistry();
  const integration = validBase('community-firmware', 'flash');
  integration.catalogSection = 'community';
  integration.source.path = 'source';
  integration.build = {
    system: 'esp-idf',
    version: 'v5.0.5',
    target: 'esp32s3',
    projectPath: 'source',
  };
  integration.flash = {
    versions: [{
      version: '1.0.0',
      channel: 'stable',
      sourceBuild: true,
    }],
  };
  const integrationDir = writeIntegration(root, integration);
  writeFileSync(join(integrationDir, 'README.md'), '# Community Firmware\n');
  mkdirSync(join(integrationDir, 'source'), { recursive: true });
  writeFileSync(join(integrationDir, 'source', 'CMakeLists.txt'), 'project(example)\n');
  writeFileSync(join(integrationDir, 'source', 'LICENSE'), 'MIT License\n');
  const result = runValidator(root);

  assert.equal(result.status, 0, result.stderr);
});

test('accepts a firmware-only community contribution with verified binaries', () => {
  const root = createRegistry();
  const integration = validBase('firmware-only', 'flash');
  integration.catalogSection = 'community';
  integration.flash = {
    versions: [{
      version: '1.0.0',
      channel: 'stable',
      manifestPath: 'firmware/1.0.0/manifest.json',
    }],
  };
  const integrationDir = writeIntegration(root, integration);
  writeFileSync(join(integrationDir, 'README.md'), '# Firmware-only contribution\n');
  mkdirSync(join(integrationDir, 'firmware', '1.0.0'), { recursive: true });
  const binary = Buffer.from([0xe9, 0x01, 0x02, 0x03]);
  const sha256 = createHash('sha256').update(binary).digest('hex');
  writeFileSync(join(integrationDir, 'firmware', '1.0.0', 'firmware.bin'), binary);
  writeFileSync(
    join(integrationDir, 'firmware', '1.0.0', 'manifest.json'),
    `${JSON.stringify({
      name: 'Firmware-only contribution',
      version: '1.0.0',
      flashSize: '16MB',
      builds: [{
        chipFamily: 'ESP32-S3',
        parts: [{ path: 'firmware.bin', offset: 65536, size: binary.length, sha256 }],
      }],
    }, null, 2)}\n`,
  );

  const result = runValidator(root);

  assert.equal(result.status, 0, result.stderr);
});

test('rejects a firmware-only community contribution without a source license', () => {
  const root = createRegistry();
  const integration = validBase('firmware-only-no-license', 'flash');
  integration.catalogSection = 'community';
  delete integration.source.license;
  integration.flash = {
    versions: [{
      version: '1.0.0',
      channel: 'stable',
      manifestPath: 'firmware/1.0.0/manifest.json',
    }],
  };
  const integrationDir = writeIntegration(root, integration);
  writeFileSync(join(integrationDir, 'README.md'), '# Firmware-only contribution\n');

  const result = runValidator(root);

  assert.equal(result.status, 1);
  assert.match(result.stderr, /source\.license: is required for firmware-only contributions/);
});

test('rejects a community contribution with local source but no build config', () => {
  const root = createRegistry();
  const integration = validBase('incomplete-source', 'flash');
  integration.catalogSection = 'community';
  integration.source.path = 'source';
  integration.flash = {
    versions: [{
      version: '1.0.0',
      channel: 'stable',
      sourceBuild: true,
    }],
  };
  const integrationDir = writeIntegration(root, integration);
  writeFileSync(join(integrationDir, 'README.md'), '# Incomplete source contribution\n');
  mkdirSync(join(integrationDir, 'source'), { recursive: true });
  writeFileSync(join(integrationDir, 'source', 'LICENSE'), 'MIT License\n');
  const result = runValidator(root);

  assert.equal(result.status, 1);
  assert.match(result.stderr, /build: is required when source\.path is provided/);
});

test('rejects a non-flash community catalog entry', () => {
  const root = createRegistry();
  const integration = validBase('source-link-only', 'download');
  integration.catalogSection = 'community';
  integration.download = {
    url: 'https://github.com/example/platform/releases/download/v1.0.0/project.zip',
    steps: [{ title: 'Download', description: 'Download the source archive.' }],
  };
  writeIntegration(root, integration);

  const result = runValidator(root);

  assert.equal(result.status, 1);
  assert.match(result.stderr, /must be "flash" for the community catalog section/);
});

test('reports a directory and integration ID mismatch', () => {
  const root = createRegistry();
  const integration = validBase('metadata-id', 'external');
  integration.external = {
    label: 'Open official tool',
    url: 'https://example.com/tool',
    description: 'Continue in the maintained upstream tool.',
  };
  const integrationDir = writeIntegration(root, integration);
  const mismatchedDir = join(root, 'integrations', 'directory-id');
  renameSync(integrationDir, mismatchedDir);

  const result = runValidator(root);

  assert.equal(result.status, 1);
  assert.match(result.stderr, /must match the directory name "directory-id"/);
});

test('rejects a text file disguised as an SVG asset', () => {
  const root = createRegistry();
  const integration = validBase('unsafe-assets', 'external');
  integration.external = {
    label: 'Open official tool',
    url: 'https://example.com/tool',
    description: 'Continue in the maintained upstream tool.',
  };
  const integrationDir = writeIntegration(root, integration);
  writeFileSync(join(integrationDir, 'assets', 'logo.svg'), 'plain text\n');

  const result = runValidator(root);

  assert.equal(result.status, 1);
  assert.match(result.stderr, /does not contain an SVG root element/);
});

test('rejects an invalid flash manifest hash', () => {
  const root = createRegistry();
  const integration = validBase('invalid-flash', 'flash');
  integration.flash = {
    versions: [
      {
        version: '1.0.0',
        channel: 'stable',
        manifestUrl: 'https://github.com/example/platform/releases/download/v1.0.0/manifest.json',
        manifestSha256: 'not-a-sha256',
        releaseUrl: 'https://github.com/example/platform/releases/tag/v1.0.0',
      },
    ],
  };
  writeIntegration(root, integration);

  const result = runValidator(root);

  assert.equal(result.status, 1);
  assert.match(result.stderr, /manifestSha256: must contain between 64 and 64 characters/);
});
