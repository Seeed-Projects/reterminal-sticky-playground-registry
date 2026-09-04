import assert from 'node:assert/strict';
import {
  mkdirSync,
  mkdtempSync,
  rmSync,
  writeFileSync,
} from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import test from 'node:test';
import {
  buildReviewCard,
  checkUrl,
  collectChangedEntries,
  findModelFiles,
  readFirmwareEntry,
  readPrintableEntry,
  scanForSecrets,
} from './review-card.mjs';

const printable = {
  schemaVersion: 1,
  id: 'sticky-desk-stand',
  name: 'Sticky Desk Stand',
  category: 'stand',
  summary: 'A desk stand that keeps Sticky upright while charging.',
  description: 'A compact desk stand with a USB-C cable channel underneath.',
  author: { name: 'Jane Maker', url: 'https://www.printables.com/@jane' },
  download: { platform: 'Printables', url: 'https://www.printables.com/model/1234', license: 'CC BY-SA 4.0' },
  preview: { image: 'assets/preview.jpg', alt: 'Sticky Desk Stand on a desk' },
};

function createRegistry() {
  const root = mkdtempSync(join(tmpdir(), 'sticky-review-card-test-'));
  const designDir = join(root, 'printables', 'sticky-desk-stand');
  mkdirSync(join(designDir, 'assets'), { recursive: true });
  writeFileSync(join(designDir, 'printable.json'), `${JSON.stringify(printable, null, 2)}\n`);
  writeFileSync(join(designDir, 'README.md'), '# Sticky Desk Stand\n');
  writeFileSync(join(designDir, 'assets', 'preview.jpg'), Buffer.alloc(120 * 1024, 1));
  return root;
}

test('groups changed files by the entry they belong to', () => {
  assert.deepEqual(collectChangedEntries([
    'printables/sticky-desk-stand/printable.json',
    'printables/sticky-desk-stand/assets/preview.jpg',
    'firmwares/trmnl/firmware.json',
    'printables/_template/README.md',
    'scripts/validate-registry.mjs',
  ]), {
    printables: ['sticky-desk-stand'],
    firmwares: ['trmnl'],
    outside: ['printables/_template/README.md', 'scripts/validate-registry.mjs'],
  });
});

test('reads a printable entry with its photo size and README', () => {
  const root = createRegistry();
  try {
    const entry = readPrintableEntry(root, 'sticky-desk-stand');
    assert.equal(entry.printable.name, 'Sticky Desk Stand');
    assert.equal(entry.hasReadme, true);
    assert.equal(entry.photoBytes, 120 * 1024);

    assert.deepEqual(readPrintableEntry(root, 'gone'), { id: 'gone', removed: true });
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});

test('reads a firmware entry with its version delivery methods', () => {
  const root = mkdtempSync(join(tmpdir(), 'sticky-review-firmware-test-'));
  try {
    const firmwareDir = join(root, 'firmwares', 'example-firmware');
    mkdirSync(firmwareDir, { recursive: true });
    writeFileSync(join(firmwareDir, 'firmware.json'), `${JSON.stringify({
      id: 'example-firmware',
      name: 'Example Firmware',
      catalogSection: 'community',
      category: 'tools',
      author: { name: 'Jane Maker' },
      source: { url: 'https://github.com/example/firmware', license: 'MIT' },
      flash: {
        versions: [
          { version: '2.0.0', channel: 'stable', sourceBuild: true },
          { version: '1.0.0', channel: 'stable', manifestPath: 'firmware/1.0.0/manifest.json' },
        ],
      },
    }, null, 2)}\n`);

    const entry = readFirmwareEntry(root, 'example-firmware');
    assert.deepEqual(entry.versions, [
      { version: '2.0.0', channel: 'stable', delivery: 'source build' },
      { version: '1.0.0', channel: 'stable', delivery: 'firmware package' },
    ]);
    assert.equal(entry.hasSourceLicense, false);
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});

test('reports reachable, blocked, and broken links', async () => {
  const responses = {
    'https://ok.example': { HEAD: 200 },
    'https://blocked.example': { HEAD: 403, GET: 403 },
    'https://gone.example': { HEAD: 410, GET: 410 },
    'https://head-only.example': { HEAD: 405, GET: 200 },
  };
  const fetchImpl = async (url, options) => new Response(null, { status: responses[url][options.method] });

  assert.deepEqual(await checkUrl('https://ok.example', { fetchImpl }), {
    url: 'https://ok.example',
    state: 'ok',
    detail: 'HTTP 200',
  });
  assert.equal((await checkUrl('https://blocked.example', { fetchImpl })).state, 'unverified');
  assert.equal((await checkUrl('https://gone.example', { fetchImpl })).state, 'broken');
  assert.equal((await checkUrl('https://head-only.example', { fetchImpl })).state, 'ok');

  const offline = await checkUrl('https://offline.example', {
    fetchImpl: async () => { throw new Error('getaddrinfo ENOTFOUND'); },
  });
  assert.equal(offline.state, 'unverified');
});

test('finds credentials and model files in the diff', () => {
  const root = mkdtempSync(join(tmpdir(), 'sticky-review-secret-test-'));
  try {
    mkdirSync(join(root, 'firmwares', 'example-firmware', 'source'), { recursive: true });
    writeFileSync(
      join(root, 'firmwares', 'example-firmware', 'source', 'config.h'),
      '#define WIFI_SSID "home"\n#define WIFI_PASSWORD "hunter2hunter2"\n',
    );
    writeFileSync(
      join(root, 'firmwares', 'example-firmware', 'README.md'),
      '# Example\n\nSet the Wi-Fi credentials at runtime.\n',
    );

    const findings = scanForSecrets(root, [
      'firmwares/example-firmware/source/config.h',
      'firmwares/example-firmware/README.md',
    ]);
    assert.equal(findings.length, 1);
    assert.equal(findings[0].file, 'firmwares/example-firmware/source/config.h');
    assert.equal(findings[0].line, 2);

    assert.deepEqual(
      findModelFiles(['printables/a/case.stl', 'printables/a/printable.json', 'printables/a/part.3MF']),
      ['printables/a/case.stl', 'printables/a/part.3MF'],
    );
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});

test('renders a quick-review card for one printable design', () => {
  const root = createRegistry();
  try {
    const changedFiles = [
      'printables/sticky-desk-stand/printable.json',
      'printables/sticky-desk-stand/README.md',
      'printables/sticky-desk-stand/assets/preview.jpg',
    ];
    const entries = collectChangedEntries(changedFiles);
    const card = buildReviewCard({
      entries,
      printableEntries: [readPrintableEntry(root, 'sticky-desk-stand')],
      linkChecks: [
        { url: printable.download.url, state: 'ok', detail: 'HTTP 200' },
        { url: printable.author.url, state: 'unverified', detail: 'HTTP 403, the host blocks automated checks' },
      ],
      rawBaseUrl: 'https://raw.githubusercontent.com/owner/repo/abc123',
    });

    assert.ok(card.startsWith('<!-- review-card -->'));
    assert.match(card, /\*\*quick review\*\*/);
    assert.match(card, /raw\.githubusercontent\.com\/owner\/repo\/abc123\/printables\/sticky-desk-stand\/assets\/preview\.jpg/);
    assert.match(card, /\| Download \| \[Printables\]\(https:\/\/www\.printables\.com\/model\/1234\) \|/);
    assert.match(card, /Download page: reachable \(HTTP 200\)/);
    assert.match(card, /Author page: could not verify/);
    assert.match(card, /\| Photo \| 120 KB \|/);
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});

test('names the download link generically when the entry has no platform', () => {
  const root = createRegistry();
  try {
    const designDir = join(root, 'printables', 'sticky-desk-stand');
    const withoutPlatform = { ...printable, download: { url: printable.download.url } };
    writeFileSync(join(designDir, 'printable.json'), `${JSON.stringify(withoutPlatform, null, 2)}\n`);

    const card = buildReviewCard({
      entries: collectChangedEntries(['printables/sticky-desk-stand/printable.json']),
      printableEntries: [readPrintableEntry(root, 'sticky-desk-stand')],
    });

    assert.match(card, /\| Download \| \[download page\]\(https:\/\/www\.printables\.com\/model\/1234\) \|/);
  } finally {
    rmSync(root, { recursive: true, force: true });
  }
});

test('raises the risk level for secrets, model files, and unrelated changes', () => {
  const entries = collectChangedEntries([
    'printables/sticky-desk-stand/case.stl',
    '.github/workflows/validate-registry.yml',
  ]);
  const card = buildReviewCard({
    entries,
    secrets: [{ file: 'firmwares/x/source/config.h', line: 2, name: 'Wi-Fi credential' }],
    modelFiles: findModelFiles(['printables/sticky-desk-stand/case.stl']),
  });

  assert.match(card, /\*\*needs a close look\*\*/);
  assert.match(card, /possible secret/);
  assert.match(card, /model files are committed/);
  assert.match(card, /files outside an entry directory/i);
  assert.match(card, /### Possible secrets/);
  assert.match(card, /`printables\/sticky-desk-stand\/case\.stl`/);
});

test('marks firmware changes as a normal review', () => {
  const entries = collectChangedEntries(['firmwares/trmnl/firmware.json']);
  const card = buildReviewCard({
    entries,
    firmwareEntries: [{
      id: 'trmnl',
      firmware: { name: 'TRMNL', catalogSection: 'platform', author: { name: 'TRMNL' }, source: { license: 'MIT' } },
      versions: [{ version: '1.8.10', channel: 'stable', delivery: 'firmware package' }],
      hasSourceLicense: false,
    }],
  });

  assert.match(card, /\*\*normal review\*\*/);
  assert.match(card, /### Firmware `trmnl`/);
  assert.match(card, /\| 1\.8\.10 \| stable \| firmware package \|/);
});
