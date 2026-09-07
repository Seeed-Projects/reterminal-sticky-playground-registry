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
  collectReservedCatalogTouches,
  findModelFiles,
  isReservedCatalog,
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

test('marks community firmware changes as a normal review', () => {
  const entries = collectChangedEntries(['firmwares/inx-pro/firmware.json']);
  const card = buildReviewCard({
    entries,
    firmwareEntries: [{
      id: 'inx-pro',
      firmware: {
        name: 'Inx Pro',
        group: 'community',
        catalogSection: 'community',
        author: { name: 'Inx Pro' },
        source: { license: 'MIT' },
      },
      versions: [{ version: '1.0.0-beta', channel: 'beta', delivery: 'firmware package' }],
      hasSourceLicense: false,
    }],
  });

  assert.match(card, /\*\*normal review\*\*/);
  assert.match(card, /### Firmware `inx-pro`/);
  assert.match(card, /\| 1\.0\.0-beta \| beta \| firmware package \|/);
  assert.doesNotMatch(card, /Reserved catalog entries/);
});

test('treats official and partner catalog labels as reserved', () => {
  assert.equal(isReservedCatalog({ group: 'official', catalogSection: 'official' }), true);
  assert.equal(isReservedCatalog({ group: 'partner', catalogSection: 'platform' }), true);
  assert.equal(isReservedCatalog({ group: 'community', catalogSection: 'platform' }), true);
  assert.equal(isReservedCatalog({ group: 'community', catalogSection: 'community' }), false);
  assert.equal(isReservedCatalog(undefined), false);
});

test('flags a new submission that marks itself as official or partner', () => {
  const pullEntry = {
    id: 'new-partner',
    firmware: { name: 'New Partner', group: 'partner', catalogSection: 'platform' },
    versions: [],
  };
  const touches = collectReservedCatalogTouches([pullEntry], [{ id: 'new-partner', removed: true }]);

  assert.deepEqual(touches, [{
    id: 'new-partner',
    name: 'New Partner',
    catalog: 'partner / platform',
    reason: 'this submission marks itself as official or partner',
  }]);

  const card = buildReviewCard({
    entries: collectChangedEntries(['firmwares/new-partner/firmware.json']),
    firmwareEntries: [pullEntry],
    baseFirmwareEntries: [{ id: 'new-partner', removed: true }],
  });

  assert.match(card, /\*\*needs a close look\*\*/);
  assert.match(card, /an official or partner firmware entry changed/);
  assert.match(card, /### Reserved catalog entries/);
  assert.match(card, /Community submissions belong in Community Firmwares/);
  assert.match(card, /`new-partner` \(partner \/ platform\) — this submission marks itself as official or partner/);
});

test('flags a change to an official or partner entry that already exists on main', () => {
  const baseEntry = {
    id: 'sticky-factory',
    firmware: { name: 'reTerminal Sticky', group: 'official', catalogSection: 'official' },
  };
  const pullEntry = {
    id: 'sticky-factory',
    firmware: { name: 'reTerminal Sticky', group: 'community', catalogSection: 'community' },
  };

  assert.deepEqual(collectReservedCatalogTouches([pullEntry], [baseEntry]), [{
    id: 'sticky-factory',
    name: 'reTerminal Sticky',
    catalog: 'official / official',
    reason: 'this official or partner entry already exists on main',
  }]);

  const removed = collectReservedCatalogTouches(
    [{ id: 'trmnl', removed: true }],
    [{ id: 'trmnl', firmware: { name: 'TRMNL', group: 'partner', catalogSection: 'platform' } }],
  );
  assert.equal(removed[0].reason, 'this official or partner entry would be removed');
  assert.equal(removed[0].catalog, 'partner / platform');
});
