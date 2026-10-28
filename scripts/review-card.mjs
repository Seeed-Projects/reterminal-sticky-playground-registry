#!/usr/bin/env node

import { existsSync, readFileSync, statSync } from 'node:fs';
import { extname, join, resolve } from 'node:path';
import { isIP } from 'node:net';

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
const RESERVED_GROUPS = new Set(['official', 'partner']);
const RESERVED_CATALOG_SECTIONS = new Set(['official', 'platform']);

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
export async function checkUrl(url, { fetchImpl = globalThis.fetch, timeoutMs = 10_000, followRedirects = true } = {}) {
  if (!followRedirects) {
    try {
      const destination = new URL(url);
      if (destination.protocol !== 'https:' || destination.username || destination.password
        || isIP(destination.hostname.replace(/^\[|\]$/g, '')) || !destination.hostname.includes('.')
        || /\.(localhost|local|internal|test|invalid)$/i.test(destination.hostname)) {
        return { url, state: 'broken', detail: 'Use a public HTTPS website without embedded credentials' };
      }
    } catch {
      return { url, state: 'broken', detail: 'Use a valid public HTTPS website' };
    }
  }
  const attempt = async (method) => {
    const controller = new AbortController();
    const timer = setTimeout(() => controller.abort(), timeoutMs);
    try {
      return await fetchImpl(url, { method, redirect: followRedirects ? 'follow' : 'manual', signal: controller.signal });
    } finally {
      clearTimeout(timer);
    }
  };

  try {
    let response = await attempt('HEAD');
    let status = response.status;
    if (status === 403 || status === 405 || status === 404) {
      response = await attempt('GET');
      status = response.status;
    }
    if (!followRedirects && status >= 300 && status < 400) {
      return { url, state: 'unverified', detail: `HTTP ${status}; redirect to ${response.headers.get('location') || 'an unspecified URL'}; inspect the destination manually` };
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

// Official Sticky firmware and Partner Firmwares are reserved catalog entries.
// 官方固件与 Partner Firmwares 属于保留目录条目。
export function isReservedCatalog(firmware) {
  return Boolean(
    firmware
    && (RESERVED_GROUPS.has(firmware.group) || RESERVED_CATALOG_SECTIONS.has(firmware.catalogSection)),
  );
}

function catalogLabel(firmware) {
  const group = firmware?.group || 'unknown';
  const section = firmware?.catalogSection || 'unknown';
  return `${group} / ${section}`;
}

// Lists official or partner firmware entries this pull request creates, edits, or removes.
// 列出本 Pull Request 新建、修改或删除的官方或合作伙伴固件条目。
export function collectReservedCatalogTouches(firmwareEntries, baseFirmwareEntries = []) {
  const baseById = new Map(baseFirmwareEntries.map((entry) => [entry.id, entry]));
  const touches = [];

  for (const entry of firmwareEntries) {
    const base = baseById.get(entry.id);
    const pullReserved = !entry.removed && isReservedCatalog(entry.firmware);
    const baseReserved = Boolean(base) && !base.removed && isReservedCatalog(base.firmware);
    if (!pullReserved && !baseReserved) continue;

    const identity = baseReserved ? base.firmware : entry.firmware;
    let reason = 'this submission marks itself as official or partner';
    if (baseReserved && entry.removed) {
      reason = 'this official or partner entry would be removed';
    } else if (baseReserved) {
      reason = 'this official or partner entry already exists on main';
    }

    touches.push({
      id: entry.id,
      name: identity?.name || entry.id,
      catalog: catalogLabel(identity),
      reason,
    });
  }

  return touches;
}

function assessRisk({ entries, secrets, modelFiles, linkChecks, reservedCatalogTouches }) {
  const reasons = [];
  if (secrets.length > 0) reasons.push('a possible secret appears in the diff');
  if (modelFiles.length > 0) reasons.push('model files are committed instead of linked');
  if (entries.outside.length > 0) reasons.push('files outside an entry directory changed');
  if (reservedCatalogTouches.length > 0) reasons.push('an official or partner firmware entry changed');
  if (reasons.length > 0) return { level: 'needs a close look', reasons };

  if (entries.firmwares.length > 0) reasons.push('a Sticky workflow needs its delivery and usage conditions reviewed');
  if (linkChecks.some((check) => check.state === 'broken')) reasons.push('a link did not answer');
  if (reasons.length > 0) return { level: 'normal review', reasons };

  return { level: 'quick review', reasons: ['card metadata and one photo only'] };
}

// Escapes contributor text for a Markdown review comment.
// 将贡献者填写的文字转为审核评论中的普通文本。
function reviewText(value) {
  return String(value || '').replace(/[<>&|`\[\]\\"']/g, (character) => `&#${character.charCodeAt(0)};`).replace(/\r?\n/g, ' ');
}

export function collectReviewUrls(printableEntries, firmwareEntries) {
  const urls = new Map();
  for (const entry of printableEntries) {
    if (entry.printable?.download?.url) urls.set(entry.printable.download.url, true);
    if (entry.printable?.author?.url) urls.set(entry.printable.author.url, true);
  }
  for (const entry of firmwareEntries) {
    if (entry.firmware?.mode !== 'external') continue;
    for (const url of [entry.firmware.external?.url]) {
      if (typeof url === 'string' && url.startsWith('https://')) urls.set(url, false);
    }
  }
  return Array.from(urls, ([url, followRedirects]) => ({ url, followRedirects }));
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
    `| Download | [${printable.download?.platform || 'download page'}](${printable.download?.url}) |`,
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

function firmwareSection(entry, rawBaseUrl, linkChecks) {
  if (entry.removed) {
    return [`### Firmware \`${entry.id}\` (removed)`, ''];
  }

  const { firmware } = entry;
  const lines = [
    `### Firmware \`${entry.id}\``,
    '',
    '| Field | Value |',
    '| --- | --- |',
    `| Name | ${reviewText(firmware.name)} |`,
    `| Catalog | ${reviewText(firmware.catalogSection || 'community')}${firmware.category ? ` / ${reviewText(firmware.category)}` : ''} |`,
    `| Author | ${reviewText(firmware.author?.name) || '**not provided**'} |`,
    `| Licence | ${reviewText(firmware.source?.license) || '**not provided**'} |`,
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

  if (firmware.mode === 'external') {
    const submission = firmware.external?.submission || {};
    let destinationHost = 'invalid URL';
    try { destinationHost = new URL(firmware.external?.url).hostname; } catch {}
    lines.push(
      '#### External project application — maintainer review required',
      '',
      '| Field | Application |',
      '| --- | --- |',
      `| Destination | ${reviewText(firmware.external?.url)} |`,
      `| Domain | ${reviewText(destinationHost)} |`,
      `| Link check | ${reviewText(linkState(linkChecks, firmware.external?.url))} |`,
      `| Reason a .bin cannot be supplied | ${reviewText(submission.hostingReason)} |`,
      `| User workflow | ${reviewText(submission.userFlow)} |`,
      `| Accounts, payments, permissions, and data | ${reviewText(submission.siteRequirements)} |`,
      '',
      'A maintainer verifies the external delivery need, destination workflow, and usage conditions. Link availability is a connectivity check. Site content and downloaded software require manual review.',
      '',
    );
    if (rawBaseUrl && firmware.assets?.preview) {
      const imagePath = firmware.assets.preview.split('/').map(encodeURIComponent).join('/');
      lines.push(`<img src="${rawBaseUrl}/firmwares/${encodeURIComponent(entry.id)}/${imagePath}" alt="External project preview" width="320">`, '');
    }
  }

  return lines;
}

export function buildReviewCard({
  entries,
  printableEntries = [],
  firmwareEntries = [],
  baseFirmwareEntries = [],
  linkChecks = [],
  secrets = [],
  modelFiles = [],
  reservedCatalogTouches,
  rawBaseUrl = '',
}) {
  const reservedTouches = reservedCatalogTouches
    ?? collectReservedCatalogTouches(firmwareEntries, baseFirmwareEntries);
  const risk = assessRisk({
    entries,
    secrets,
    modelFiles,
    linkChecks,
    reservedCatalogTouches: reservedTouches,
  });
  const lines = [
    MARKER,
    '## Review card',
    '',
    `**${risk.level}** — ${risk.reasons.join('; ')}.`,
    '',
  ];

  if (reservedTouches.length > 0) {
    lines.push(
      '### Reserved catalog entries',
      '',
      'Community submissions belong in Community Firmwares. Official Sticky firmware and Partner Firmwares are maintained by Seeed or the partner platform.',
      '',
      ...reservedTouches.map((touch) => (
        `- \`${touch.id}\` (${touch.catalog}) — ${touch.reason}`
      )),
      '',
    );
  }

  for (const entry of printableEntries) {
    lines.push(...printableSection(entry, { linkChecks, rawBaseUrl }));
  }
  for (const entry of firmwareEntries) {
    lines.push(...firmwareSection(entry, rawBaseUrl, linkChecks));
    const base = baseFirmwareEntries.find((item) => item.id === entry.id);
    const previousUrl = base?.firmware?.external?.url;
    if (previousUrl && previousUrl !== entry.firmware?.external?.url) {
      lines.push(
        '#### External destination changed — renew maintainer review',
        '',
        `Previous destination: ${reviewText(previousUrl)}`,
        '',
        `Proposed destination: ${reviewText(entry.firmware?.external?.url) || 'external delivery removed'}`,
        '',
      );
    }
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
  const baseRootDir = process.env.BASE_REGISTRY_ROOT
    ? resolve(process.env.BASE_REGISTRY_ROOT)
    : '';
  const baseFirmwareEntries = baseRootDir
    ? entries.firmwares.map((id) => readFirmwareEntry(baseRootDir, id))
    : [];

  const urls = collectReviewUrls(printableEntries, firmwareEntries);
  const linkChecks = await Promise.all(urls.map(({ url, followRedirects }) => checkUrl(url, { followRedirects })));

  process.stdout.write(buildReviewCard({
    entries,
    printableEntries,
    firmwareEntries,
    baseFirmwareEntries,
    linkChecks,
    secrets: scanForSecrets(rootDir, changedFiles),
    modelFiles: findModelFiles(changedFiles),
    rawBaseUrl: process.env.RAW_BASE_URL || '',
  }));
}

if (process.argv[1] && resolve(process.argv[1]) === resolve(import.meta.filename)) {
  await main();
}
