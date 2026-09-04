#!/usr/bin/env node

import { existsSync, readFileSync, statSync } from 'node:fs';
import { extname, join, resolve } from 'node:path';

const MARKER = '<!-- review-card -->';
const MODEL_EXTENSIONS = new Set(['.stl', '.3mf', '.step', '.stp', '.obj', '.f3d', '.ipt', '.sldprt']);
const SECRET_PATTERNS = [
  { name: 'GitHub token', pattern: /gh[pousr]_[A-Za-z0-9]{16,}/ },
  { name: 'AWS access key', pattern: /AKIA[0-9A-Z]{16}/ },
  { name: 'Private key block', pattern: /-----BEGIN (?:RSA |EC |OPENSSH |PGP )?PRIVATE KEY-----/ },
  { name: 'Generic API key', pattern: /(?:api[_-]?key|secret|password|passwd)["'\s:=]+[A-Za-z0-9/+_-]{16,}/i },
  { name: 'Wi-Fi credential', pattern: /(?:WIFI_PASS|WIFI_PASSWORD|ssid)["'\s:=]+[^\s"']{8,}/i },
];
const SCANNABLE_EXTENSIONS = new Set(['.json', '.md', '.c', '.h', '.cpp', '.hpp', '.py', '.ini', '.yml', '.yaml', '.txt', '.cfg', '.conf', '.sh']);

// Groups the files of a pull request by the Registry entry they belong to.
// 按所属的 Registry 条目对 Pull Request 改动的文件分组。
export function collectChangedEntries(changedFiles) {
  const printables = new Set();
  const firmwares = new Set();
  const outside = [];

  for (const file of changedFiles) {
    const segments = file.split('/');
    if (segments[0] === 'printables' && segments.length > 2 && !segments[1].startsWith('_')) {
      printables.add(segments[1]);
    } else if (segments[0] === 'firmwares' && segments.length > 2 && !segments[1].startsWith('_')) {
      firmwares.add(segments[1]);
    } else {
      outside.push(file);
    }
  }

  return {
    printables: Array.from(printables).sort(),
    firmwares: Array.from(firmwares).sort(),
    outside,
  };
}

export function readPrintableEntry(rootDir, id) {
  const metadataPath = join(rootDir, 'printables', id, 'printable.json');
  if (!existsSync(metadataPath)) return { id, removed: true };

  const printable = JSON.parse(readFileSync(metadataPath, 'utf8'));
  const imagePath = join(rootDir, 'printables', id, printable.preview?.image || '');
  return {
    id,
    printable,
    photoBytes: existsSync(imagePath) ? statSync(imagePath).size : 0,
    hasReadme: existsSync(join(rootDir, 'printables', id, 'README.md')),
  };
}

export function readFirmwareEntry(rootDir, id) {
  const metadataPath = join(rootDir, 'firmwares', id, 'firmware.json');
  if (!existsSync(metadataPath)) return { id, removed: true };

  const firmware = JSON.parse(readFileSync(metadataPath, 'utf8'));
  const versions = firmware.flash?.versions || [];
  return {
    id,
    firmware,
    versions: versions.map((version) => ({
      version: version.version,
      channel: version.channel,
      delivery: version.sourceBuild === true
        ? 'source build'
        : (version.manifestPath ? 'firmware package' : 'registry release'),
    })),
    hasSourceLicense: existsSync(join(rootDir, 'firmwares', id, 'source', 'LICENSE')),
  };
}

// Confirms a download or profile page answers, without failing the card when a host blocks CI.
// 确认下载页或主页可以访问；主机屏蔽 CI 时不把结果算作失败。
export async function checkUrl(url, { fetchImpl = globalThis.fetch, timeoutMs = 10_000 } = {}) {
  const attempt = async (method) => {
    const controller = new AbortController();
    const timer = setTimeout(() => controller.abort(), timeoutMs);
    try {
      const response = await fetchImpl(url, { method, redirect: 'follow', signal: controller.signal });
      return response.status;
    } finally {
      clearTimeout(timer);
    }
  };

  try {
    let status = await attempt('HEAD');
    if (status === 403 || status === 405 || status === 404) {
      status = await attempt('GET');
    }
    if (status >= 200 && status < 400) return { url, state: 'ok', detail: `HTTP ${status}` };
    if (status === 403 || status === 429) return { url, state: 'unverified', detail: `HTTP ${status}, the host blocks automated checks` };
    return { url, state: 'broken', detail: `HTTP ${status}` };
  } catch (error) {
    return { url, state: 'unverified', detail: error.name === 'AbortError' ? 'no answer within 10s' : error.message };
  }
}

export function scanForSecrets(rootDir, changedFiles) {
  const findings = [];

  for (const file of changedFiles) {
    const path = join(rootDir, file);
    if (!SCANNABLE_EXTENSIONS.has(extname(file).toLowerCase()) || !existsSync(path)) continue;
    if (statSync(path).size > 512 * 1024) continue;

    const lines = readFileSync(path, 'utf8').split('\n');
    for (const [index, line] of lines.entries()) {
      for (const { name, pattern } of SECRET_PATTERNS) {
        if (pattern.test(line)) {
          findings.push({ file, line: index + 1, name });
          break;
        }
      }
    }
  }

  return findings;
}

export function findModelFiles(changedFiles) {
  return changedFiles.filter((file) => MODEL_EXTENSIONS.has(extname(file).toLowerCase()));
}

function assessRisk({ entries, secrets, modelFiles, linkChecks }) {
  const reasons = [];
  if (secrets.length > 0) reasons.push('a possible secret appears in the diff');
  if (modelFiles.length > 0) reasons.push('model files are committed instead of linked');
  if (entries.outside.length > 0) reasons.push('files outside an entry directory changed');
  if (reasons.length > 0) return { level: 'needs a close look', reasons };

  if (entries.firmwares.length > 0) reasons.push('firmware runs on the device and needs the hardware test record');
  if (linkChecks.some((check) => check.state === 'broken')) reasons.push('a link did not answer');
  if (reasons.length > 0) return { level: 'normal review', reasons };

  return { level: 'quick review', reasons: ['card metadata and one photo only'] };
}

function formatBytes(bytes) {
  return bytes >= 1024 * 1024
    ? `${(bytes / (1024 * 1024)).toFixed(1)} MB`
    : `${Math.round(bytes / 1024)} KB`;
}

function linkState(linkChecks, url) {
  const check = linkChecks.find((item) => item.url === url);
  if (!check) return '';
  if (check.state === 'ok') return `reachable (${check.detail})`;
  if (check.state === 'broken') return `**did not answer** (${check.detail})`;
  return `could not verify (${check.detail})`;
}

function printableSection(entry, { linkChecks, rawBaseUrl }) {
  if (entry.removed) {
    return [`### Printable \`${entry.id}\` (removed)`, ''];
  }

  const { printable } = entry;
  const lines = [
    `### Printable \`${entry.id}\``,
    '',
  ];

  if (rawBaseUrl && printable.preview?.image) {
    lines.push(
      `<img src="${rawBaseUrl}/printables/${entry.id}/${printable.preview.image}" alt="${printable.preview.alt || ''}" width="320">`,
      '',
    );
  }

  lines.push(
    '| Field | Value |',
    '| --- | --- |',
    `| Name | ${printable.name} |`,
    `| Category | ${printable.category} |`,
    `| Author | ${printable.author?.url ? `[${printable.author.name}](${printable.author.url})` : printable.author?.name} |`,
    `| Download | [${printable.download?.platform}](${printable.download?.url}) |`,
    `| Licence | ${printable.download?.license || '**not provided**'} |`,
    `| Photo | ${entry.photoBytes ? formatBytes(entry.photoBytes) : '**missing**'} |`,
    `| README | ${entry.hasReadme ? 'present' : '**missing**'} |`,
    '',
    `Download page: ${linkState(linkChecks, printable.download?.url)}`,
  );

  if (printable.author?.url) {
    lines.push(`Author page: ${linkState(linkChecks, printable.author.url)}`);
  }

  lines.push(
    '',
    `> ${printable.summary}`,
    '',
  );
  return lines;
}

function firmwareSection(entry) {
  if (entry.removed) {
    return [`### Firmware \`${entry.id}\` (removed)`, ''];
  }

  const { firmware } = entry;
  const lines = [
    `### Firmware \`${entry.id}\``,
    '',
    '| Field | Value |',
    '| --- | --- |',
    `| Name | ${firmware.name} |`,
    `| Catalog | ${firmware.catalogSection || 'community'}${firmware.category ? ` / ${firmware.category}` : ''} |`,
    `| Author | ${firmware.author?.name || '**not provided**'} |`,
    `| Licence | ${firmware.source?.license || '**not provided**'} |`,
    `| Source licence file | ${entry.hasSourceLicense ? 'present' : 'not applicable'} |`,
    '',
  ];

  if (entry.versions.length > 0) {
    lines.push(
      '| Version | Channel | Delivery |',
      '| --- | --- | --- |',
      ...entry.versions.map((version) => `| ${version.version} | ${version.channel} | ${version.delivery} |`),
      '',
    );
  }

  return lines;
}

export function buildReviewCard({
  entries,
  printableEntries = [],
  firmwareEntries = [],
  linkChecks = [],
  secrets = [],
  modelFiles = [],
  rawBaseUrl = '',
}) {
  const risk = assessRisk({ entries, secrets, modelFiles, linkChecks });
  const lines = [
    MARKER,
    '## Review card',
    '',
    `**${risk.level}** — ${risk.reasons.join('; ')}.`,
    '',
  ];

  for (const entry of printableEntries) {
    lines.push(...printableSection(entry, { linkChecks, rawBaseUrl }));
  }
  for (const entry of firmwareEntries) {
    lines.push(...firmwareSection(entry));
  }

  if (secrets.length > 0) {
    lines.push(
      '### Possible secrets',
      '',
      ...secrets.map((finding) => `- \`${finding.file}\` line ${finding.line}: ${finding.name}`),
      '',
      'Replace the value with a placeholder and describe the runtime setup in the README.',
      '',
    );
  }

  if (modelFiles.length > 0) {
    lines.push(
      '### Model files in the diff',
      '',
      ...modelFiles.map((file) => `- \`${file}\``),
      '',
      'Printable entries link to a download page instead of committing model files.',
      '',
    );
  }

  if (entries.outside.length > 0) {
    lines.push(
      '### Files outside an entry directory',
      '',
      ...entries.outside.slice(0, 20).map((file) => `- \`${file}\``),
      '',
    );
  }

  lines.push(
    '<sub>Generated for each push to this pull request. Registry validation runs separately in Checks.</sub>',
    '',
  );

  return lines.join('\n');
}

async function main() {
  const rootDir = resolve(process.env.REGISTRY_ROOT || '.');
  const changedFiles = (process.env.CHANGED_FILES || '')
    .split('\n')
    .map((line) => line.trim())
    .filter(Boolean);

  const entries = collectChangedEntries(changedFiles);
  const printableEntries = entries.printables.map((id) => readPrintableEntry(rootDir, id));
  const firmwareEntries = entries.firmwares.map((id) => readFirmwareEntry(rootDir, id));

  const urls = new Set();
  for (const entry of printableEntries) {
    if (entry.printable?.download?.url) urls.add(entry.printable.download.url);
    if (entry.printable?.author?.url) urls.add(entry.printable.author.url);
  }
  const linkChecks = await Promise.all(Array.from(urls).map((url) => checkUrl(url)));

  process.stdout.write(buildReviewCard({
    entries,
    printableEntries,
    firmwareEntries,
    linkChecks,
    secrets: scanForSecrets(rootDir, changedFiles),
    modelFiles: findModelFiles(changedFiles),
    rawBaseUrl: process.env.RAW_BASE_URL || '',
  }));
}

if (process.argv[1] && resolve(process.argv[1]) === resolve(import.meta.filename)) {
  await main();
}
