# 为 reTerminal Sticky Playground Registry 贡献内容

感谢你帮助 reTerminal Sticky 接入更多软件、平台、固件和社区项目。

这个仓库是 Sticky Playground 面向外部开发者的公开贡献入口。贡献者可以在这里说明：

- 你的平台或项目是什么；
- 它由谁开发和维护；
- 它如何支持 reTerminal Sticky；
- 用户应该通过什么方式使用它；
- Playground 可以展示哪些图片、模板、下载地址或固件版本。

这个仓库保存的是声明式数据（通俗解释：像填写一张结构固定的登记表），而不是
Sticky 网站本身的页面代码。网站页面、浏览器烧录功能、域名、部署配置和数据统计仍由
Seeed Studio 在内部仓库中维护。

英文贡献指南请查看 [CONTRIBUTING.md](CONTRIBUTING.md)。

## 从这里开始

一次完整贡献通常按下面的顺序进行：

1. 确认项目有公开的官网、代码仓库或文档页面。
2. 确认项目作者和用户支持入口。
3. 从四种接入模式中选择一种。
4. Fork 本仓库，并为项目建立一个独立目录。
5. 填写 `integration.json`，加入所需图片或模板。
6. 运行 `npm run validate`。
7. 提交 Pull Request，并说明测试过的硬件和流程。
8. 根据维护者的审核意见完善内容。

本仓库支持四种接入模式：

| 模式 | 适用情况 | 贡献者提供什么 | 用户最终得到什么 |
|---|---|---|---|
| `external` | 上游已经有安装器、浏览器工具箱或完整文档 | 平台资料和上游入口 | 用户进入上游维护的正式流程 |
| `template` | 每位用户都需要生成自己的 YAML、JSON 等配置文件 | 平台资料和可组合的文本片段 | 用户预览、复制或下载生成的配置文件 |
| `download` | 用户需要下载完整源码工程并在本地修改和编译 | 版本固定的工程压缩包地址和操作步骤 | 用户得到可在 PlatformIO 等工具中打开的工程 |
| `flash` | 已经有经过测试、无需修改即可运行的固件 | 固件版本、manifest 地址和完整性信息 | 固件经过额外审核后，可进入浏览器烧录流程 |

选择模式时，以“让用户完成一次完整流程所需的最简单方式”为准：

- 上游已经提供成熟安装器时，使用 `external`。
- 每位用户都需要改配置时，使用 `template`。
- 用户需要改源码或 UI 工程时，使用 `download`。
- 固件可以直接运行时，使用 `flash`。

一句话总结：先确定用户最终拿到的是链接、配置文件、源码工程还是可烧录固件，再选择对应模式。

## 可以贡献的内容

本仓库接受：

- 官方合作平台；
- 社区固件和应用；
- 上游安装器、工具箱和文档入口；
- 可重复使用的配置模板；
- 带版本号的源码工程下载；
- 用于浏览器烧录审核的固件 manifest；
- 已获得使用许可的平台 Logo 和实际效果图；
- 已有平台的版本、链接、文案、图片和兼容性更新。

网站页面怎么布局、烧录器怎么连接设备、域名怎么配置、网站怎么部署，由内部网站仓库统一负责。
外部贡献者只需要把一个平台的资料和资源准备完整，不需要访问 Sticky 私有仓库。

这个仓库不是普通的固件网盘。固件和源码压缩包通常由上游项目通过 GitHub Release
等正式渠道发布。本仓库登记经过审核的版本、地址和哈希值（通俗解释：文件的唯一数字指纹）。

一句话总结：第三方继续维护自己的软件，这个仓库负责用统一格式把它接入 Playground。

## 项目需要满足的基本条件

当一个平台或项目满足下面条件时，就适合提交 Pull Request：

- 有公开的项目主页、产品页面、代码仓库或使用文档；
- 能够确认作者或实际维护团队；
- 有公开的 Issue、支持页面或讨论入口；
- 已经测试 reTerminal Sticky，或者能够准确说明当前兼容情况；
- 可以使用四种模式中的一种形成完整用户流程；
- 提交到本仓库的图片和模板允许公开分发；
- Wi-Fi 密码、API Key 等用户数据使用占位符表示。

普通第三方项目使用：

```json
{
  "group": "community"
}
```

`official` 表示 Seeed Studio 与上游平台共同确认的官方工作流。如果你希望加入
`official`，建议先提交 Issue 或联系仓库维护者。双方会先确认平台定位、内容归属、
用户支持方式和后续版本维护责任。

一句话总结：社区项目可以直接按规范准备，官方平台先确认合作关系再投入较多开发工作。

## 仓库目录结构

正式平台直接放在 `integrations/` 下面：

```text
integrations/
  my-platform/
    integration.json
    assets/
      logo.svg
      preview.webp
    templates/
      header.yaml
      display.yaml
      sensor.yaml
```

目录名就是平台的稳定 ID，统一使用小写英文、数字和连字符：

```text
crosspoint-reader
my-dashboard
home-assistant-display
```

`integration.json` 中的 `id` 必须与目录名完全一致：

```json
{
  "id": "my-platform"
}
```

以下划线 `_` 开头的目录用于模板和示例，不会被当成正式平台发布。

一句话总结：一个平台一个目录，目录名和配置里的 ID 保持完全相同。

## 创建一个新平台目录

在仓库根目录执行：

```bash
cp -R integrations/_template integrations/my-platform
```

接下来按顺序处理：

1. 把 `my-platform` 改成最终平台 ID。
2. 替换 `integration.json` 中的所有示例内容。
3. 只保留当前模式需要的对象。
4. 把 Logo 和预览图放入 `assets/`。
5. 如果使用模板模式，把模板片段放入 `templates/`。
6. 运行自动检查。

```bash
npm run validate
```

校验器会扫描全部正式平台，因此也能发现不同目录之间 ID 重复等问题。

一句话总结：复制模板、填写资料、加入资源、运行检查，这四步就构成一次基础贡献。

## 所有模式共用的基础资料

每个 `integration.json` 都从下面这些公共字段开始：

```json
{
  "schemaVersion": 1,
  "id": "my-platform",
  "name": "My Platform",
  "group": "community",
  "mode": "external",
  "status": "experimental",
  "summary": "Turn Sticky into a focused information display.",
  "description": "My Platform provides a managed information-display workflow for reTerminal Sticky, including content scheduling and device setup.",
  "author": {
    "name": "Project author or team",
    "url": "https://github.com/example"
  },
  "source": {
    "url": "https://github.com/example/my-platform",
    "license": "MIT"
  },
  "support": {
    "url": "https://github.com/example/my-platform/issues"
  },
  "documentationUrl": "https://github.com/example/my-platform#readme",
  "compatibility": {
    "devices": [
      "reterminal-sticky"
    ],
    "notes": "Tested on the production reTerminal Sticky hardware."
  },
  "assets": {
    "logo": "assets/logo.svg",
    "preview": "assets/preview.webp",
    "previewAlt": "My Platform dashboard shown on reTerminal Sticky"
  },
  "tags": [
    "dashboard",
    "productivity"
  ]
}
```

### 公共字段说明

| 字段 | 是否必填 | 作用 |
|---|---:|---|
| `schemaVersion` | 是 | 数据规范版本，当前固定为 `1`。 |
| `id` | 是 | 平台的稳定 ID，使用小写连字符格式，并与目录名相同。 |
| `name` | 是 | 平台或项目对外显示的正式名称，最多 80 个字符。 |
| `group` | 是 | 普通第三方项目使用 `community`，确认合作关系的官方工作流使用 `official`。 |
| `mode` | 是 | `external`、`template`、`download` 或 `flash` 四选一。 |
| `status` | 是 | `experimental`、`beta` 或 `stable`。 |
| `summary` | 是 | 一句话简介，最多 140 个字符。 |
| `description` | 是 | 说明平台价值、主要用途和设置要求，最多 800 个字符。 |
| `author` | 是 | 项目作者或维护团队，以及公开的个人或组织页面。 |
| `source` | 是 | 项目官网或源码地址，并按实际情况填写许可证。 |
| `support` | 是 | 公开 Issue、支持中心或讨论页面。 |
| `documentationUrl` | 否 | 最直接的安装或使用文档。 |
| `compatibility` | 是 | 支持的设备和准确的硬件说明。 |
| `assets` | 是 | 本地 Logo、预览图和无障碍图片说明。 |
| `tags` | 否 | 最多六个简短标签，用于分类和搜索。 |

当前规范只登记 reTerminal Sticky，所以设备列表固定为：

```json
{
  "devices": [
    "reterminal-sticky"
  ]
}
```

`compatibility.notes` 可以填写：

- 实际测试的硬件版本；
- 必须连接的配件；
- 对电池、存储卡或网络的要求；
- 其他经过验证的使用条件。

一句话总结：公共字段回答“它是谁、谁维护、解决什么问题、是否支持 Sticky”。

## 模式一：external

当上游项目已经拥有成熟的安装和配置体验时，使用 `external`。

常见入口包括：

- 官方浏览器安装器；
- 固件 Toolbox；
- 设备配网应用；
- 官方开发板支持页面；
- 完整的上游安装文档。

加入 `external` 对象：

```json
{
  "mode": "external",
  "external": {
    "label": "Open official toolbox",
    "url": "https://example.com/toolbox",
    "description": "Continue in the official toolbox to install firmware, configure the device, and manage content."
  }
}
```

### External 字段说明

| 字段 | 是否必填 | 作用 |
|---|---:|---|
| `external.label` | 是 | 用户看到的按钮文字。 |
| `external.url` | 是 | 上游持续维护的 HTTPS 地址。 |
| `external.description` | 是 | 说明用户进入这个页面后要做什么、会得到什么。 |

上游页面应该能够让 Sticky 用户继续完成流程。平台专属的安装细节由上游文档负责，
Playground 负责给用户提供清楚、可信的入口。

一句话总结：上游已经有好用的工具时，Playground 负责把用户准确地带过去。

## 模式二：template

当每位用户都需要根据自己的环境生成配置文件时，使用 `template`。

适合的文件包括：

- YAML；
- JSON；
- TOML；
- 普通文本配置。

模板片段放在平台目录内部：

```text
integrations/my-platform/
  integration.json
  templates/
    header.yaml
    display.yaml
    sensor.yaml
    footer.yaml
```

加入 `template` 对象：

```json
{
  "mode": "template",
  "template": {
    "outputExtension": "yaml",
    "mimeType": "text/yaml",
    "fileNamePattern": "{integrationId}-{deviceId}",
    "headerPath": "templates/header.yaml",
    "footerPath": "templates/footer.yaml",
    "options": [
      {
        "id": "display",
        "label": "Sticky display",
        "description": "Adds the required reTerminal Sticky display configuration.",
        "path": "templates/display.yaml",
        "required": true,
        "defaultSelected": true
      },
      {
        "id": "sensor",
        "label": "Environmental sensor",
        "description": "Adds optional temperature and humidity sensor configuration.",
        "path": "templates/sensor.yaml",
        "required": false,
        "defaultSelected": true
      }
    ]
  }
}
```

文件名支持下面两个保留占位符：

| 占位符 | 生成结果 |
|---|---|
| `{integrationId}` | 当前平台的 `id`。 |
| `{deviceId}` | `reterminal-sticky`。 |

### Template 字段说明

| 字段 | 是否必填 | 作用 |
|---|---:|---|
| `template.outputExtension` | 是 | 输出文件后缀，使用小写字母且不带点号。 |
| `template.mimeType` | 是 | 浏览器下载文件时使用的 MIME 类型（通俗解释：告诉浏览器这是什么文件）。 |
| `template.fileNamePattern` | 是 | 不包含后缀的输出文件名规则。 |
| `template.headerPath` | 否 | 始终放在输出开头的本地模板片段。 |
| `template.footerPath` | 否 | 始终放在输出结尾的本地模板片段。 |
| `template.options` | 是 | 至少一个可选择的模板片段。 |
| `option.id` | 是 | 选项的稳定小写连字符 ID。 |
| `option.label` | 是 | 用户看到的选项名称。 |
| `option.description` | 是 | 说明这个片段会开启什么功能。 |
| `option.path` | 是 | 平台目录内的模板文件路径。 |
| `option.required` | 否 | 为 `true` 时，这个片段是生成有效配置的必需部分。 |
| `option.defaultSelected` | 否 | Playground 初次打开时是否默认选择。 |

模板内容应做到：

- Wi-Fi、Token、账户等个人数据使用明确占位符；
- 默认选项组合后能够形成语法正确的完整文件；
- 硬件参数与实际测试过的 Sticky 行为一致；
- 注释用于帮助用户修改配置；
- 文件保持为可以直接在 Pull Request 中阅读的纯文本。

提交 Pull Request 时，请提供默认选项组合后的完整输出。审核者既会检查每个片段，
也会检查最终拼接出来的文件。

一句话总结：模板模式像积木，贡献者提供可靠片段，Playground 根据用户选择拼成完整配置。

## 模式三：download

当用户需要下载完整源码工程，并在 PlatformIO、ESP-IDF、Arduino IDE 或其他开发工具中
修改、编译和烧录时，使用 `download`。

源码压缩包由上游项目发布。推荐使用带版本号的 GitHub Release Asset（通俗解释：
挂在某个正式发布版本下面的下载文件），这样可以清楚确认作者、版本和发布说明。

加入 `download` 对象：

```json
{
  "mode": "download",
  "download": {
    "url": "https://github.com/example/my-platform/releases/download/v1.2.0/my-platform-sticky.zip",
    "version": "1.2.0",
    "fileName": "my-platform-sticky.zip",
    "sha256": "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef",
    "steps": [
      {
        "title": "Download the project",
        "description": "Download and extract the versioned Sticky project archive."
      },
      {
        "title": "Customize the configuration",
        "description": "Open the project in PlatformIO and fill in the documented local configuration values."
      },
      {
        "title": "Build and upload",
        "description": "Select the reTerminal Sticky environment, build the project, and upload it over USB."
      }
    ]
  }
}
```

### Download 字段说明

| 字段 | 是否必填 | 作用 |
|---|---:|---|
| `download.url` | 是 | 带明确版本的源码工程 HTTPS 下载地址。 |
| `download.version` | 否 | 上游发布版本。 |
| `download.fileName` | 否 | 建议的下载文件名。 |
| `download.sha256` | 否 | 压缩包的小写 SHA-256，固定文件建议填写。 |
| `download.steps` | 是 | 一到十二个按顺序执行的操作步骤。 |

每个步骤包含：

- `title`：简短动作名称；
- `description`：用户可以直接照着执行的完整说明。

步骤应覆盖：

- 需要安装的开发工具；
- 如何打开工程；
- 用户应该在哪里填写自己的配置；
- 应该选择哪个 Sticky 构建环境；
- 如何连接设备并上传。

压缩包自身应包含 README、许可证、依赖版本和构建配置。Registry 里的步骤负责快速引导，
详细技术说明继续以上游项目为准。

一句话总结：Download 模式给用户一套能继续开发的完整工程，而不是直接运行的最终固件。

## 模式四：flash

当固件已经编译完成、在 reTerminal Sticky 上测试通过，并且无需修改源码就能运行时，
使用 `flash`。

浏览器烧录会直接修改用户设备，所以这个模式需要更严格的审核。Registry 负责记录：

- 这个固件来自哪个项目；
- 它对应哪个正式版本；
- manifest 文件存放在哪里；
- manifest 的 SHA-256 是多少；
- 用户可以在哪里查看完整发布说明。

加入 `flash` 对象：

```json
{
  "mode": "flash",
  "flash": {
    "versions": [
      {
        "version": "1.2.0",
        "channel": "stable",
        "manifestUrl": "https://raw.githubusercontent.com/example/my-platform/0123456789abcdef0123456789abcdef01234567/releases/sticky/1.2.0/manifest.json",
        "manifestSha256": "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef",
        "releaseUrl": "https://github.com/example/my-platform/releases/tag/v1.2.0"
      }
    ],
    "notes": [
      {
        "title": "Initial setup",
        "description": "After flashing, hold the power button for three seconds and follow the on-device Wi-Fi setup flow."
      }
    ]
  }
}
```

### Flash 字段说明

| 字段 | 是否必填 | 作用 |
|---|---:|---|
| `flash.versions` | 是 | 一个或多个经过登记的固件版本。 |
| `version.version` | 是 | 与上游发布一致的准确版本号。 |
| `version.channel` | 是 | `experimental`、`beta` 或 `stable`。 |
| `version.manifestUrl` | 是 | 内容固定不变的 HTTPS manifest 地址。 |
| `version.manifestSha256` | 是 | manifest 文件本身的小写 SHA-256。 |
| `version.releaseUrl` | 是 | 包含版本背景和发布说明的上游发布页面。 |
| `flash.notes` | 否 | 一到十二个设置或烧录后操作说明。 |

### 什么是固定不变的 Manifest 地址

Manifest（通俗解释：告诉烧录器“有哪些文件、分别写到哪里”的清单）必须对应一个确定版本。

推荐形式：

```text
https://github.com/owner/project/releases/download/v1.2.0/manifest.json
https://raw.githubusercontent.com/owner/project/<完整提交SHA>/path/manifest.json
```

完整 Git 提交 SHA、Release Tag 和固定 Release Asset 都能帮助审核者找到同一份文件。
分支会继续产生新提交，因此正式登记时需要能够定位到准确版本的地址。

### Manifest 应包含什么

Manifest 应清楚说明固件包名称、版本和设备上每一个写入区域：

```json
{
  "name": "My Platform for reTerminal Sticky",
  "version": "1.2.0",
  "builds": [
    {
      "chipFamily": "ESP32-S3",
      "parts": [
        {
          "path": "bootloader.bin",
          "offset": 0,
          "size": 24576,
          "sha256": "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
        },
        {
          "path": "partitions.bin",
          "offset": 32768,
          "size": 3072,
          "sha256": "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
        },
        {
          "path": "firmware.bin",
          "offset": 65536,
          "size": 1048576,
          "sha256": "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
        }
      ]
    }
  ]
}
```

示例中的文件名和地址只用于解释数据结构。每个项目应填写实际构建并验证过的 Flash Layout
（通俗解释：每个固件文件在芯片存储空间中的写入位置）。

每一个 `part` 都要包含：

- 能够根据 manifest 地址找到的相对文件路径；
- 准确的整数写入地址 `offset`；
- 文件准确字节数 `size`；
- 文件的小写 SHA-256。

上游发布说明还应写清楚：

- 使用哪一台 reTerminal Sticky 做了测试；
- Flash 容量和分区表；
- 是否需要先擦除整片 Flash；
- 第一次启动时会出现什么；
- 如何恢复官方固件；
- 已知限制和依赖的外部服务。

### 如何生成 SHA-256

macOS：

```bash
shasum -a 256 manifest.json
shasum -a 256 firmware.bin
```

Linux：

```bash
sha256sum manifest.json
sha256sum firmware.bin
```

请对最终上传到 Release 的文件计算哈希。只要重新构建、重新格式化或重新上传，文件内容就可能变化，
这时需要重新计算并更新登记值。

一句话总结：Flash 模式不仅登记下载链接，还要用版本和哈希证明“审核的是哪一份固件”。

## 图片资源

每个平台都提供：

```text
assets/
  logo.svg
  preview.webp
```

支持的格式：

- `.svg`
- `.png`
- `.jpg`
- `.jpeg`
- `.webp`

自动检查的大小限制：

| 资源 | 最大文件大小 |
|---|---:|
| Logo | 1 MB |
| 预览图 | 5 MB |

图片准备建议：

- 使用上游项目当前正式 Logo；
- 预览图展示真实平台界面、设备效果或实际使用结果；
- Logo 保持足够清晰和对比度；
- 文件名使用小写 ASCII 字符；
- SVG 使用静态、自包含内容；
- `previewAlt` 用一句话准确描述图片内容；
- 确认项目许可证或作者授权允许图片放入本仓库。

预览图的作用是帮助用户理解平台，而不是作为与功能无关的装饰背景。

一句话总结：Logo 帮助识别品牌，预览图帮助用户提前看懂这个平台实际能做什么。

## 链接和项目归属

Registry 中所有公开地址统一使用 HTTPS。

各链接的推荐来源：

- `source.url`：项目官网或正式源码仓库；
- `author.url`：作者或维护组织的公开页面；
- `support.url`：最适合普通用户反馈问题的入口；
- `documentationUrl`：最直接的 Sticky 安装或使用教程；
- Download 和 Flash：带明确版本、能够长期定位的发布地址。

自动检查会验证 URL 格式。维护者还会在审核时实际打开关键链接，因为项目归属、
页面内容和当前可用性需要人工判断。

如果项目更换维护者、迁移仓库或调整支持入口，请修改原有平台目录，使 Registry
继续指向当前真正负责项目的人和页面。

一句话总结：每个链接都应该把用户带到当前、正式、有人维护的上游位置。

## 用户凭据和个人数据

本仓库完全公开。下面这些内容统一使用占位符：

- Wi-Fi 名称和密码；
- API Key 和访问 Token；
- 用户账户 ID；
- 私有服务器地址；
- 证书和私钥；
- 个人邮箱或设备唯一编号。

推荐占位符：

```text
<YOUR_WIFI_SSID>
<YOUR_WIFI_PASSWORD>
<YOUR_API_KEY>
```

模板可以引导用户在本地填写这些值。用户填写的真实内容不会保存到 Registry。

一句话总结：公开仓库只保存配置结构，用户自己的秘密数据只留在用户本地。

## 本地自动检查

### 前置条件

安装 Node.js 20 或更新版本：

```bash
node --version
```

成功时输出应以 `v20` 或更大的主版本号开头。

### 运行检查

在仓库根目录执行：

```bash
npm test
npm run validate
```

`npm test` 会在临时目录中运行四种模式和常见错误的回归测试。

成功示例：

```text
Registry validation passed (1 integration(s)).
```

失败示例：

```text
Registry validation failed with 2 error(s):
- integrations/my-platform/integration.json.id: must match the directory name "my-platform"
- integrations/my-platform/integration.json.assets.preview: references a missing file: assets/preview.webp
```

校验器会尽量一次列出全部问题。根据提示修改对应路径，然后重新运行即可。

### 自动检查覆盖的内容

- `schemas/integration.schema.json` 是有效 JSON；
- 每个正式平台目录都有 `integration.json`；
- 公共字段和模式字段使用规范支持的名称；
- 目录名与 `id` 一致；
- 平台 ID、标签、模板选项和固件版本没有重复；
- `group`、`mode`、`status` 和固件通道使用支持的值；
- 公开链接是有效 HTTPS URL；
- 本地文件路径始终位于当前平台目录内；
- 引用的图片和模板文件真实存在；
- 图片内容与文件后缀一致；
- SVG 只包含静态图形，不包含脚本和外部资源；
- 图片格式和文件大小符合要求；
- 每个平台只包含当前模式对应的一个模式对象；
- Download 和 Flash 使用小写 SHA-256 格式。

本地校验器负责检查 Registry 数据。上游源码编译、固件实际运行效果和硬件测试结果，
由 Pull Request 中的测试证据与维护者审核共同确认。

一句话总结：自动检查负责发现格式和文件问题，实际硬件效果还需要真实测试记录。

## 提交 Pull Request 前的准备

建议一次 Pull Request 只增加一个新平台，或者只处理一个已有平台的明确更新。
这样作者、测试范围、审核意见和未来历史都更清楚。

提交前请完成：

- 运行 `npm run validate`；
- 检查完整 Diff；
- 打开提交的所有公开链接；
- 确认图片来源和公开使用许可；
- Template 模式拼接并解析默认输出；
- Download 模式下载并检查压缩包；
- Flash 模式测试准确版本和实际 Flash Layout；
- 清理构建目录、编辑器配置、本地凭据和与本次平台无关的文件。

Pull Request 模板会要求填写：

- 平台名称和 Integration ID；
- 选择的接入模式；
- 上游项目地址；
- 实际测试的 Sticky 硬件；
- 用户最终得到什么；
- 已经完成哪些验证；
- 当前模式对应的确认项。

一句话总结：Pull Request 不只提交文件，还要告诉审核者“测试了什么、为什么可以相信”。

## 维护者会审核什么

维护者主要判断：

- 是否能给 reTerminal Sticky 用户带来明确价值；
- 作者、项目和许可证是否清楚；
- 是否存在持续有效的支持入口；
- 选择的模式是否符合真实用户流程；
- 兼容性说明是否有测试依据；
- 用户文案是否清楚、准确；
- 图片是否可用并允许公开分发；
- 上游版本地址是否稳定；
- 用户凭据是否妥善使用占位符；
- Flash 模式是否提供可复核的完整性信息；
- 项目后续由谁负责维护。

Pull Request 合并代表 Registry 已经接受这份平台资料。平台什么时候出现在 Sticky Playground，
还会结合网站兼容、内容审核、发布安排以及 Flash 模式的设备验证单独决定。

一句话总结：Registry 审核资料和可信度，网站发布还会经过独立的上线流程。

## 更新已有平台

下面情况直接修改原有平台目录：

- 发布了新的上游版本；
- 官网、文档或支持地址变化；
- 平台介绍或预览图需要更新；
- 模板片段增加或调整；
- 兼容性说明变化；
- 状态从 `experimental` 升级到 `beta` 或 `stable`；
- 上游用户流程发生明显变化，需要切换接入模式。

Flash 模式更新时，只要旧版本文件仍然稳定可用并继续受支持，就保留旧的版本对象，
再加入一个新的版本对象。如果旧版本已经不可用或存在安全问题，请在 Pull Request
中说明移除原因。

平台改名或更新品牌时继续使用原有稳定 `id`。只有真正成为不同项目或完全不同用户流程时，
才建立新的 ID。

一句话总结：平台升级就在原目录继续维护，让用户和历史始终能找到同一个项目。

## 项目停止维护或退出 Registry

当项目停止维护、更换所有者、服务下线或无法继续提供 Sticky 工作流时，可以提交一个专门的
Pull Request 处理退出。

请提供：

- 上游停止维护或所有权转移的公开说明；
- 最后一个可用版本；
- 可以替代它的项目或迁移路径；
- 已经发布的固件是否需要保留，用于用户恢复或历史使用。

Registry 维护者会把目录变化与 Sticky Playground 中已经展示的入口一起协调。

一句话总结：项目退出也保留清楚依据，避免用户面对突然失效、无人说明的入口。

## 合并以后会发生什么

Pull Request 合并后：

1. 文件进入公开 Registry 的正式历史。
2. GitHub Actions 会在 `main` 分支再次运行校验。
3. 维护者可以把这个平台加入后续生成的 Playground Catalog。
4. Sticky 私有网站可以在独立构建或发布时读取某个已批准 Registry 版本。
5. 网站上线时可以补充展示文案和运行检查，但上游项目归属保持不变。

目前 Registry 还没有自动触发 Sticky 私有网站部署。第一阶段先把公开贡献格式和审核流程建立稳定，
网站同步会在后续单独接入。

一句话总结：合并先代表资料被正式接收，网站何时上线由独立发布流程决定。

## Registry 维护者与上游维护者的职责

Registry 维护者负责：

- 数据规范和自动检查；
- 平台资料审核和分类；
- 私有网站中的统一展示；
- 网站构建和部署；
- 浏览器烧录的最终启用；
- 处理已经不符合 Registry 要求的平台。

上游平台维护者负责：

- 项目源码和许可证；
- 固件、压缩包和发布说明；
- 平台专属技术文档；
- 用户支持和 Issue 处理；
- 当链接、兼容性、所有权或版本发生变化时更新 Registry。

这样的分工让第三方继续完整控制自己的软件，同时让 Sticky Playground 保持统一、可审核的用户体验。

一句话总结：上游负责把项目维护好，Registry 负责把项目稳定地介绍给 Sticky 用户。

## 什么时候先提交 Issue

下面情况建议先提交 GitHub Issue，再准备较大的 Pull Request：

- 还不确定应该选择哪种模式；
- 希望成为官方平台；
- 固件使用特殊 Flash Layout；
- 一个项目包含多个相互独立的用户流程；
- 当前 Schema 无法表达项目需要的字段；
- 对图片授权或固件完整性要求不确定。

Issue 中请附上上游项目地址，并用几句话说明希望 Sticky 用户如何使用它。维护者可以先确认
最合适、改动最小的贡献方式，再进入具体文件准备。

一句话总结：遇到模式或结构问题先讨论，可以避免贡献者做好大量文件后再重新设计。
