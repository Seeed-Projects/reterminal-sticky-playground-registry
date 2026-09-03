# 贡献固件

本指南面向 Sticky Playground **Firmware** 页面的固件贡献。一份完整的社区固件贡献
通过审核后，会在该页面生成一张固件卡片，并进入由 Seeed 网站提供的浏览器烧录页面。
如果你想分享 3D 打印外壳或支架，请阅读[贡献 3D 打印设计](contributing-printables.zh-CN.md)。

贡献者可以选择提交完整可编译源码，也可以只提交经过验证的固件包并提供上游源码
链接。源码模式由 GitHub Actions 自动编译并整理固件。两种方式都需要项目信息、展示
图片和实机测试记录。Sticky 私有网站只读取经过审核并锁定版本的公开仓库内容，再生成
卡片和烧录页。

英文指南请查看 [contributing-firmware.md](contributing-firmware.md)。
两类贡献共用的审核与发布流程见 [CONTRIBUTING.zh-CN.md](../CONTRIBUTING.zh-CN.md)。

## 目录

- [贡献完成后，用户会得到什么](#贡献完成后用户会得到什么)
- [PR 必须提供的文件](#pr-必须提供的文件)
- [创建一份贡献](#创建一份贡献)
- [firmware.json](#firmwarejson) 及其[字段说明](#字段说明)
- [两种贡献方式](#两种贡献方式)：[源码模式](#源码模式) 或 [仅固件包](#仅固件包)（含 [manifest.json 说明](#仅固件包的-manifestjson)）
- [Sticky 官方固件更新](#sticky-官方固件更新)
- [源码模式要求](#源码模式要求)
- [可选的本地 ESP-IDF 编译](#可选的本地-esp-idf-编译)
- [本地自动检查](#本地自动检查) 及[常见校验报错](#常见校验报错)
- [提交 pull request](#提交-pull-request)
- [真实设备测试](#真实设备测试)
- [PR 审核和自动编译](#pr-审核和自动编译)
- [合并后如何进入 Sticky 官网](#合并后如何进入-sticky-官网)
- [更新已有固件](#更新已有固件)
- [PR 提交前清单](#pr-提交前清单)

## 贡献完成后，用户会得到什么

一份正式发布的社区固件会形成下面这条用户流程：

1. 用户打开 Sticky Playground。
2. 用户点击社区固件卡片。
3. 用户在 Sticky 网站查看固件版本和安装说明。
4. 用户通过 USB 连接 reTerminal Sticky。
5. 用户点击 **Flash Now**，直接在浏览器中完成烧录。

公开 Registry 保存可安装固件包，以及完整源码或上游源码地址；Sticky 网站负责页面
样式、USB 串口连接、烧录界面、域名和正式部署。

一句话总结：贡献者交付完整固件，用户留在 Sticky 网站里直接烧录。

## PR 必须提供的文件

每个项目在 `firmwares/` 下建立一个独立目录，并从下面两种目录结构中选择一种。

源码模式：

```text
firmwares/
  my-firmware/
    firmware.json
    README.md
    assets/
      preview.jpg
      logo.svg  # 可选
    source/
      CMakeLists.txt
      sdkconfig.defaults
      main/
      components/
      LICENSE
```

仅固件包模式：

```text
firmwares/
  my-firmware/
    firmware.json
    README.md
    assets/
      preview.jpg
      logo.svg  # 可选
    firmware/
      1.0.0/
        manifest.json
        bootloader.bin
        partition-table.bin
        my-firmware.bin
```

| 文件或目录 | 作用 |
|---|---|
| `firmware.json` | 保存卡片文案、作者、兼容性、编译方式和固件版本 |
| `README.md` | 说明固件功能、操作方式、环境要求和实机测试结果 |
| `assets/logo.*` | 合作伙伴条目使用的官方标识；社区条目可按需提供项目标识 |
| `assets/preview.*` | 社区条目使用 Sticky 实机照片或截图；合作伙伴条目使用官方 Logo |
| `source/` | 源码模式使用的可编译工程 |
| `source/LICENSE` | 本地提交源码对应的许可证 |
| `firmware/<version>/manifest.json` | 仅固件包模式使用的烧录清单 |
| `firmware/<version>/*.bin` | 仅固件包模式提交的可烧录文件 |

目录名和 `firmware.json` 中的 `id` 必须使用相同的小写连字符格式，例如
`weather-dashboard` 或 `sticky-2048`。

`firmwares/<firmware-id>/assets/` 下的图片支持 `.png`、`.jpg`、
`.jpeg`、`.webp` 和静态 `.svg` 格式。社区条目使用 Sticky 实机照片或截图作为
预览图。经过协调的合作伙伴条目可以让 `assets.logo` 与 `assets.preview` 指向同一份
官方 Logo。

一句话总结：源码模式交源码给 Action 生成固件；仅固件模式直接提交可烧录文件。

## 创建一份贡献

在仓库根目录执行：

```bash
cp -R firmwares/_template firmwares/my-firmware
```

按下面的顺序准备：

1. 修改目录名和 `firmware.json.id`。
2. 社区条目填写作者署名；合作伙伴条目填写经过确认的官方项目链接。
3. 在项目 README 和 `firmware.json` 中提供上游源码地址与许可证名称。
4. 社区条目加入 Sticky 实机效果图；合作伙伴条目加入官方 Logo。
5. 选择源码模式时，加入 `source/`、`build` 配置和 `sourceBuild: true`。
6. 选择仅固件包模式时，把经过测试的固件包放入 `firmware/<version>/`。
7. 运行 Registry 测试和校验。
8. 把这套固件烧录到真实 reTerminal Sticky 上测试。
9. 提交 Pull Request，并写清测试结果。

一句话总结：准备项目资料和所选交付方式需要的文件后，再进入审核。

## firmware.json

普通第三方贡献统一使用：`"group": "community"`、
`"catalogSection": "community"`、`"mode": "flash"`。

```json
{
  "schemaVersion": 1,
  "id": "my-firmware",
  "name": "My Firmware",
  "group": "community",
  "catalogSection": "community",
  "category": "productivity",
  "mode": "flash",
  "status": "experimental",
  "summary": "Turn Sticky into a focused information display.",
  "description": "My Firmware provides a local information display with touch controls and an offline data source.",
  "author": {
    "name": "Project author or team",
    "url": "https://github.com/example"
  },
  "origin": {
    "name": "Project repository",
    "url": "https://github.com/example/my-firmware"
  },
  "source": {
    "url": "https://github.com/example/my-firmware",
    "license": "MIT",
    "path": "source"
  },
  "support": {
    "url": "https://github.com/example/my-firmware/issues"
  },
  "documentationUrl": "https://github.com/example/my-firmware#readme",
  "compatibility": {
    "devices": ["reterminal-sticky"],
    "notes": "Tested on reTerminal Sticky production hardware."
  },
  "assets": {
    "preview": "assets/preview.jpg",
    "previewAlt": "My Firmware running on reTerminal Sticky"
  },
  "tags": ["dashboard", "offline"],
  "build": {
    "system": "esp-idf",
    "version": "v5.4",
    "target": "esp32s3",
    "projectPath": "source"
  },
  "flash": {
    "versions": [
      {
        "version": "1.0.0",
        "channel": "experimental",
        "sourceBuild": true
      }
    ],
    "notes": [
      {
        "title": "Device connection",
        "description": "Connect reTerminal Sticky with a USB data cable and use desktop Chrome or Edge."
      }
    ]
  }
}
```

### 字段说明

`firmware.json` 是一个严格的 JSON 对象：下面每个键要么必填要么可选，
**出现任何其他键都会导致校验失败**。正式定义见
[`schemas/firmware.schema.json`](../schemas/firmware.schema.json)。社区贡献按
“社区填法”一列填写；合作伙伴和官方条目由 Seeed 协调维护。

#### 身份与目录位置

| 字段 | 类型 | 必填 | 限制 | 社区填法 / 填什么 |
|---|---|---|---|---|
| `schemaVersion` | 整数 | 是 | 必须是 `1` | `1` |
| `id` | 字符串 | 是 | 2–64 字符，`^[a-z0-9]+(?:-[a-z0-9]+)*$` | 与目录名相同，例如 `sticky-2048` |
| `name` | 字符串 | 是 | 1–80 字符 | 网站上的卡片标题 |
| `group` | 字符串 | 是 | `official`、`partner`、`community` | `community` |
| `catalogSection` | 字符串 | 是 | `official`、`platform`、`community`、`draft` | `community`。`draft` 的卡片不会发布 |
| `category` | 字符串 | `community` 必填 | `ereader`、`productivity`、`personal`、`weather`、`finance`、`tools`、`fun`、`smart-home` | 决定卡片出现在哪个筛选项下，见下表 |
| `mode` | 字符串 | 是 | `flash`、`external`、`template`、`download` | `flash`（浏览器烧录）。其余模式保留给协调维护的条目 |
| `status` | 字符串 | 是 | `experimental`、`beta`、`stable` | 项目成熟度，显示为徽标 |
| `summary` | 字符串 | 是 | 1–140 字符 | 标题下方的一句话 |
| `description` | 字符串 | 是 | 1–800 字符 | 两三句话：做什么、需要哪些服务、用户会得到什么 |
| `tags` | 字符串数组 | 否 | 最多 6 项，每项 1–32 字符，不重复 | 短标签，例如 `offline`、`wifi`、`touch` |

社区固件分类：

| 值 | 筛选项名称 | 适用范围 |
|---|---|---|
| `ereader` | eReader | 图书、文章、文档阅读器 |
| `productivity` | Productivity | 待办、日历、笔记、计时器、工作看板 |
| `personal` | Personal | 习惯打卡、礼拜时间、日记、个人提醒 |
| `weather` | Weather | 天气、空气质量、潮汐、预报 |
| `finance` | Finance | 行情、持仓、预算 |
| `tools` | Tools | 实用工具、计算器、换算、设备诊断 |
| `fun` | Fun | 游戏、艺术、玩具、趣味显示 |
| `smart-home` | Smart Home | Home Assistant、传感器、控制、在家检测 |

#### 署名与链接

| 字段 | 类型 | 必填 | 限制 | 填什么 |
|---|---|---|---|---|
| `author.name` | 字符串 | `community` 必填 | 1–80 字符 | 卡片上显示的作者或团队名 |
| `author.url` | 字符串 | 否 | HTTPS 网址 | 点击名字时打开的主页或项目页 |
| `origin.name` | 字符串 | 否 | 1–80 字符 | 固件来源的显示名，例如 `Project repository` 或某个社区名 |
| `origin.url` | 字符串 | 否 | HTTPS 网址 | `origin.name` 的链接 |
| `source.url` | 字符串 | 是 | HTTPS 网址 | 上游源码仓库，两种贡献方式都要填 |
| `source.license` | 字符串 | 仅固件包必填 | 1–80 字符 | SPDX 风格的许可证标识，如 `MIT`、`GPL-3.0`、`Apache-2.0` |
| `source.path` | 字符串 | 源码模式必填 | 目录内的相对路径，通常是 `source` | 存放 ESP-IDF 工程的目录 |
| `support.url` | 字符串 | 是 | HTTPS 网址 | 用户反馈问题的地方，通常是 issue 页面 |
| `documentationUrl` | 字符串 | 否 | HTTPS 网址 | 用户文档，通常是上游 README |

#### 兼容性与图片

| 字段 | 类型 | 必填 | 限制 | 填什么 |
|---|---|---|---|---|
| `compatibility.devices` | 数组 | 是 | 必须恰好是 `["reterminal-sticky"]` | 固定写 `["reterminal-sticky"]` |
| `compatibility.notes` | 字符串 | 否 | 最长 400 字符 | 测试过的硬件版本、需要的配件、已知限制 |
| `assets.preview` | 字符串 | 是 | `assets/<文件>.(png\|jpg\|jpeg\|webp\|svg)`，≤ 5 MB | 固件运行中的真实 Sticky 截图或照片 |
| `assets.previewAlt` | 字符串 | 是 | 1–180 字符 | 一句话描述预览图 |
| `assets.logo` | 字符串 | 否 | 同上格式，≤ 1 MB | 可选的项目标识。合作伙伴条目在这里和 `preview` 都使用官方 logo |

#### 编译设置（仅源码模式）

| 字段 | 类型 | 必填 | 限制 | 填什么 |
|---|---|---|---|---|
| `build.system` | 字符串 | 是 | 必须是 `esp-idf` | `esp-idf` |
| `build.version` | 字符串 | 是 | 最长 64 字符，`^[A-Za-z0-9][A-Za-z0-9._-]*$` | 项目使用的 ESP-IDF 版本：`v5.4`、`v5.3.2`、`latest` |
| `build.target` | 字符串 | 是 | 最长 40 字符 | `esp32s3`（reTerminal Sticky 使用 ESP32-S3） |
| `build.projectPath` | 字符串 | 是 | 相对路径 | 与 `source.path` 相同 |

仅固件包模式请整个省略 `build` 对象。

#### 固件版本

| 字段 | 类型 | 必填 | 限制 | 填什么 |
|---|---|---|---|---|
| `flash.versions` | 数组 | 是 | 至少 1 项，最新版本在最前 | 每个已发布版本一项 |
| `flash.versions[].version` | 字符串 | 是 | 1–40 字符，不重复 | 展示给用户的版本号，例如 `1.0.0` |
| `flash.versions[].channel` | 字符串 | 是 | `experimental`、`beta`、`stable` | 该版本的成熟度 |
| `flash.versions[].sourceBuild` | 布尔 | 源码模式：最新版本填 `true` | — | 由 GitHub Actions 从 `source/` 编译这个版本 |
| `flash.versions[].manifestPath` | 字符串 | 仅固件包：每个版本必填 | `firmware/<version>/manifest.json` | 已提交的 manifest 路径 |
| `flash.versions[].manifestUrl`、`manifestSha256`、`releaseUrl` | 字符串 | 维护者使用 | — | 用于已经存放在 Registry GitHub Release 里的版本 |
| `flash.notes` | 数组 | 否 | 1–12 项 | 烧录页上显示的安装提示；每项含 `title`（≤ 100）和 `description`（≤ 500） |

每个版本只能用一种交付方式：`sourceBuild: true`，或 `manifestPath`，或 Release 三元组。

`author` 和 `origin` 用于网站署名展示；`source` 保存审核与固件打包使用的源码仓库、
许可证和本地编译路径。

`official` 和 `platform` 区域由 Seeed 或合作平台共同维护。合作伙伴条目使用
`"group": "partner"`、`"catalogSection": "platform"`、`"mode": "flash"`、
官方项目链接和官方标识。平台身份已经由条目名称和官方链接明确表达，因此作者署名
为可选项。普通外部 PR 统一进入 `community` 区域。维护者可以把历史迁移但资料尚未
齐全的条目标记为 `draft`；草稿不会出现在 Sticky Playground。

一句话总结：社区条目需要有明确源码地址，并通过源码或固件包形成可烧录版本。

## 两种贡献方式

### 源码模式

提交 `source/`，设置 `source.path`，加入配套的 `build` 配置，并把最新版本设置为
`"sourceBuild": true`。GitHub Actions 会在干净环境中编译源码，自动生成 manifest 和
全部 `.bin`。PR 阶段会生成临时构建产物供审核；PR 合并后会发布成固定版本的 GitHub
Release，供 Sticky 测试分支读取。
每个项目版本只对应一个固定 Release，因此源码更新时需要使用新的
`flash.versions[].version` 版本号。

### 仅固件包

填写 `source.url` 和 `source.license`，省略 `source.path` 与 `build`，并把完整固件放在
`firmware/<version>/`。manifest 必须记录每个 bin 的烧录地址、字节大小和 SHA-256。
项目 README 和 PR 需要记录实机型号、固件版本、固件来源和实机测试结果。

```json
"source": {
  "url": "https://github.com/example/my-firmware",
  "license": "MIT"
}
```

#### 仅固件包的 manifest.json

每个版本目录里有一个 `manifest.json` 和它列出的 `.bin` 文件。网站读取这份 manifest
在浏览器里烧录设备。

```json
{
  "name": "My Firmware",
  "version": "1.0.0",
  "flashSize": "16MB",
  "flashMode": "dio",
  "flashFreq": "80m",
  "baudRate": 460800,
  "new_install_prompt_erase": true,
  "builds": [
    {
      "chipFamily": "ESP32-S3",
      "parts": [
        { "path": "bootloader.bin",      "offset": 0,      "size": 21344,   "sha256": "<sha256>" },
        { "path": "partition-table.bin", "offset": 32768,  "size": 3072,    "sha256": "<sha256>" },
        { "path": "my-firmware.bin",     "offset": 65536,  "size": 1523712, "sha256": "<sha256>" }
      ]
    }
  ]
}
```

| 字段 | 必填 | 填什么 |
|---|---|---|
| `name` | 是 | 与 `firmware.json` 的 `name` 相同 |
| `version` | 是 | 与指向这份 manifest 的 `flash.versions[].version` 相同 |
| `flashSize` | 否 | 设备闪存大小，reTerminal Sticky 为 `16MB`；用来拒绝超出范围的分区 |
| `flashMode`、`flashFreq`、`baudRate`、`new_install_prompt_erase` | 否 | 传给浏览器烧录器；上面的值适用于 Sticky |
| `builds[].chipFamily` | 是 | `ESP32-S3` |
| `builds[].parts[].path` | 是 | 只写文件名，与 manifest 同目录，以 `.bin` 结尾 |
| `builds[].parts[].offset` | 是 | 烧录地址（十进制字节），来自编译产物的 `flasher_args.json` |
| `builds[].parts[].size` | 是 | 文件的精确字节数 |
| `builds[].parts[].sha256` | 是 | 文件的小写 SHA-256 |

各分区不能重叠，单个 `.bin` 不超过 32 MB。数值可以这样获取：

```bash
cd firmwares/my-firmware/firmware/1.0.0
wc -c *.bin                 # 大小
shasum -a 256 *.bin         # sha256（macOS/Linux）；Windows 用 certutil -hashfile 文件名 SHA256
```

烧录地址来自 ESP-IDF 的 `build/flasher_args.json`（`flash_files`）或上游项目的发布说明。
单个合并镜像从地址 `0` 开始。

一句话总结：提交源码时由 Action 产出固件；直接提交固件时由作者提供完整烧录包。

## Sticky 官方固件更新

由 Seeed 维护的 `sticky-factory` 更新使用仅固件包方式，并保持
`"group": "official"`、`"catalogSection": "official"` 和
`"mode": "flash"`。每个新官方版本存放在
`firmwares/sticky-factory/firmware/<version>/`，同时把
`flash.versions` 中的最新版本指向准确的本地路径
`firmware/<version>/manifest.json`。

版本目录保存经过测试的完整固件包。manifest 记录固件版本、芯片型号、烧录参数、
分区地址、字节大小、SHA-256，以及可选的 MD5。固件 README 和 PR 同步记录官方固件
产物来源及相同的包信息。

已经使用 Registry GitHub Release 的官方历史版本继续保留原有 `manifestUrl`、
`manifestSha256` 和 `releaseUrl`。这样，新版本使用仓库内存档，所有历史下载仍然保持
原来的固定地址。新官方版本提交后，其版本目录就是该版本唯一的交付记录。

提交时在 PR 模板中选择 **Official Sticky firmware update maintained by Seeed**，并完整
填写固件包来源和实机测试结果。

一句话总结：新官方固件进入版本目录，现有历史版本继续从原来的 Release 提供。

## 源码模式要求

源码模式的 `source/` 必须能够仅依靠本次提交的文件完成编译。它应包含工程构建
文件、应用代码、本地组件、依赖清单或锁定文件、默认配置和许可证。

Wi-Fi 密码、API Key、Token 和密码等用户私密信息使用占位符或首次运行配置方式。

第一阶段的自动编译流程支持 ESP-IDF，常见目录如下：

```text
source/
  CMakeLists.txt
  sdkconfig.defaults
  main/
    CMakeLists.txt
    main.cpp
  components/
  LICENSE
```

需要其他编译系统的项目，先提交 Issue，让维护者先为该编译系统加入可重复执行的 CI
构建适配，再提交正式固件 PR。

`build.version` 决定 GitHub Actions 使用哪个 ESP-IDF 版本。贡献者填写项目实际使用
的版本，例如 `v5.0.5`、`v5.3.2` 或 `latest`，流程不会统一锁定到某个固定版本。

一句话总结：源码必须让审核机器能够从零重新编译，而不是只留一个外部链接。

## 可选的本地 ESP-IDF 编译

作者可以在提交 PR 前先做一次本地编译。安装 `firmware.json` 中声明的 ESP-IDF
版本，然后进入项目的 `source/` 目录执行：

```bash
idf.py set-target esp32s3
idf.py -D PROJECT_VER=1.0.0 build
```

这一步用于作者在提交前确认工程能正常编译。Registry 会忽略本地生成的 `build/`、
`sdkconfig`、依赖缓存和编辑器配置。PR 提交后，正式固件由 GitHub Actions 重新编译
并整理，无需把本机生成的文件放进 PR。

一句话总结：本地编译是可选自测，正式固件统一由 Action 生成。

## 本地自动检查

安装 Node.js 20 或更高版本，然后执行：

```bash
npm test
npm run validate
```

自动检查会确认：

- 目录名与项目 ID 一致；
- 必填资料和 HTTPS 链接完整；
- 项目 README 和声明的源码许可证存在；
- 设置 `source.path` 时，本地源码目录和 ESP-IDF 工程文件存在；
- 图片格式正确，SVG 只包含静态内容；
- manifest 结构正确；
- 每个已提交固件文件存在，大小和 SHA-256 一致；
- 不同固件分区的烧录地址没有重叠；
- 社区条目满足本站直接烧录的全部要求。
- Sticky 最新官方固件使用仓库内的标准版本目录。

一句话总结：这一步负责提前发现“少文件、固件不匹配、烧录地址错误”等问题。

源码模式的 manifest 和固件文件检查会在 Action 编译后执行；仅固件包模式直接检查
PR 中提交的文件。

### 常见校验报错

| 报错文字 | 原因 | 修法 |
|---|---|---|
| `firmwares/my-firmware: is missing firmware.json` | 文件没创建或名字不对 | 创建 `firmwares/my-firmware/firmware.json` |
| `...firmware.json: contains unsupported field "..."` | 出现了字段说明里没有的键 | 删掉或修正拼写 |
| `...firmware.json.id: must match the directory name "..."` | `id` 与目录名不一致 | 改成一样 |
| `...firmware.json.category: is required for community firmware entries` | 缺 `category` | 从八个值里选一个 |
| `...firmware.json.author: is required for community firmware` | 缺 `author` | 补上 `author.name` |
| `...firmware.json.source.license: is required for firmware-only packages` | 仅固件包没写许可证 | 补上 `source.license` |
| `...firmware.json.build: is required when source.path is provided` | 源码模式缺 `build` | 补上 `build` 对象 |
| `...firmware.json.build.projectPath: must contain CMakeLists.txt for an ESP-IDF project` | `source/` 为空或指错目录 | 把 ESP-IDF 工程根目录提交到 `source/` |
| `...flash.versions[0].sourceBuild: must be true for a source contribution` | 最新版本没有 `sourceBuild` | 第一项加上 `"sourceBuild": true` |
| `...flash.versions[0]: must use exactly one firmware delivery method` | `sourceBuild` 和 `manifestPath` 同时出现，或都没有 | 只保留一种 |
| `...manifestPath: must be inside the firmware/ directory` | manifest 放错位置 | 移到 `firmware/<version>/manifest.json` |
| `...manifestPath: version must match firmware version "1.0.0"` | manifest 的 `version` 与 `flash.versions[].version` 不一致 | 改成一样 |
| `...parts[0].path: must be one .bin filename beside the manifest` | 路径带目录或后缀不对 | 文件放在 manifest 旁边，只写文件名 |
| `...parts[0].size: expected 1523712 bytes but found 1523700` | `size` 写错 | 重新 `wc -c` 后更新 |
| `...parts[0].sha256: does not match the firmware file` | 哈希写错，或算完哈希后文件又变了 | 重新 `shasum -a 256` 后更新 |
| `...parts[1]: overlaps the previous firmware part` | 某个分区的 `offset + size` 压到了下一个分区 | 对照 `flasher_args.json` 检查地址 |
| `...assets.preview: does not contain a valid PNG file signature` | JPG 改名成了 `.png`（或类似情况） | 使用真实后缀 |
| `...source.LICENSE: references a missing file: source/LICENSE` | 源码目录缺许可证文件 | 加上 `source/LICENSE` |

## 提交 pull request

[CONTRIBUTING.zh-CN.md → 三种提交方式](../CONTRIBUTING.zh-CN.md#5-三种提交方式)
里的任一方式都可以；固件文件较多，用 git 最方便。

```bash
git checkout -b add-my-firmware
git add firmwares/my-firmware
git commit -m "feat: add My Firmware 1.0.0"
git push -u origin add-my-firmware
```

然后向 `Seeed-Projects/reterminal-sticky-playground-registry` 的 `main` 发起 pull
request，并填写模板：

1. **Contribution type** 勾选一条 **Firmware:** 开头的类型。
2. 完成 **Common verification**。
3. 在 **Firmware** 一节填写名称、目录、版本、上游项目、许可证，仅固件包还要填产物来源和 SHA-256。
4. 勾选 **Package type**，只完成对应的子清单（源码模式或仅固件包）。
5. 完成 **Physical-device test**：设备版本、安装方式、版本号、测试过的主流程、重启/USB 结果。

标题建议：`Add <Firmware Name> <version>`。

源码模式：PR 打开后 GitHub Actions 会编译项目。**Build <id>** 任务完成后，其页面上有
名为 `firmware-<id>-<version>` 的产物，里面是 `manifest.json` 和 `.bin` 文件。你可以
下载后用 `esptool` 或浏览器烧录器烧录，用审核者将要发布的同一份字节完成真机测试。

一句话总结：推分支、开 PR、按模板填固件信息和真机测试记录。

## 真实设备测试

按照 `manifest.json` 中相同的地址，把固件烧录到一台 reTerminal Sticky。至少验证：

- 上电和第一次启动；
- 项目的主要使用流程；
- 项目使用到的触摸和硬件按键；
- 使用存储功能时的重启和数据恢复；
- USB 重新连接后再次完整安装。

在 PR 中记录测试硬件、ESP-IDF 版本、固件版本和结果。建议附上主流程的照片或短视频，
方便审核者核对预览图和实际行为。

一句话总结：能编译只是第一关，真实 Sticky 能正常运行才具备发布条件。

## PR 审核和自动编译

PR 需要包含所选贡献方式对应的内容、项目资料、图片和实机测试结果。
GitHub Actions 会依次执行：

1. 检查 Registry 结构和仅固件包模式提交的文件。
2. 按每个源码项目声明的 ESP-IDF 版本进行编译。
3. 根据 ESP-IDF 烧录表自动生成 manifest 和完整固件包。
4. 把生成的固件作为 PR 临时构建产物，供维护者下载审核。

当这些检查通过，并且维护者确认项目用途、许可证、兼容性和实机结果后，PR 才进入合并。

一句话总结：作者提交源码，自动流程负责生成可审核的固件成品。

## 合并后如何进入 Sticky 官网

Registry PR 合并后不会立刻进入正式生产站。维护者按照下面的顺序发布：

1. 合并资料完整且检查通过的 Registry PR。
2. 等待 Registry `main` 的 Action 发布源码编译固件 Release。
3. 在 Sticky 固定测试分支更新所锁定的 Registry 提交。
4. 本地构建 Sticky，确认新卡片和烧录页面正确生成。
5. 从本地 Sticky 页面把固件烧录到真实设备并验收。
6. 把测试通过的 Sticky 分支合并到 Sticky `main`。
7. 由公司服务器构建网站，并通过 Kubernetes 发布。

这条流程把“外部贡献审核”“实机验收”和“正式上线”分成三个明确阶段，同时保持
Sticky 网站仓库闭源。

一句话总结：合并公开 PR 是进入测试，Sticky 实机验收通过后才进入官网。

## 更新已有固件

发布新版本时：

1. 在 `flash.versions` 最前面加入新版本。
2. 源码模式更新 `source/`，并把新版本设置为 `sourceBuild: true`。
3. 仅固件包模式把经过测试的固件包放入 `firmware/<version>/`。
4. 保留仍被 `firmware.json` 引用的旧版本目录。
5. 重新运行自动检查和真实设备测试。
6. 在 PR 中说明用户能看到的变化和升级后的行为。

一句话总结：每个版本保留测试结果；源码模式的历史固件保存在对应 GitHub Release。

## PR 提交前清单

- [ ] 一个项目目录包含本次完整贡献。
- [ ] `firmware.json` 使用所选贡献类型规定的分组、目录区域和模式。
- [ ] 社区固件条目已填写文档列出的 `category`。
- [ ] 已选择“源码 + 构建配置”或“仅固件包 + 对应贡献类型要求的产物来源信息”。
- [ ] 官方固件更新使用仓库内版本目录，并记录官方固件产物来源。
- [ ] 源码模式使用 `sourceBuild: true`，或仅固件包模式包含 manifest 和全部必需 `.bin`。
- [ ] README 和 PR 写明经过测试的固件来源和固件版本。
- [ ] `npm test` 和 `npm run validate` 全部通过。
- [ ] 这套固件已在真实 reTerminal Sticky 上完成测试。
- [ ] PR 写明测试硬件、固件版本、测试结果，以及适用时的编译版本。
