# 为 reTerminal Sticky Playground 贡献固件

这个仓库是 reTerminal Sticky 固件合集面向外部开发者的公开贡献与审核入口。
一份完整的社区贡献通过审核后，会在 Sticky 官网 Playground 中生成一张固件卡片，
并进入由 Seeed 网站提供的浏览器烧录页面。

贡献者可以选择提交完整可编译源码，也可以只提交经过验证的固件包并提供上游源码
链接。源码模式由 GitHub Actions 自动编译并整理固件。两种方式都需要项目信息、展示
图片和实机测试记录。Sticky 私有网站只读取经过审核并锁定版本的公开仓库内容，再生成
卡片和烧录页。

英文指南请查看 [CONTRIBUTING.md](CONTRIBUTING.md)。

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

每个项目在 `integrations/` 下建立一个独立目录，并从下面两种目录结构中选择一种。

源码模式：

```text
integrations/
  my-firmware/
    integration.json
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
integrations/
  my-firmware/
    integration.json
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
| `integration.json` | 保存卡片文案、作者、兼容性、编译方式和固件版本 |
| `README.md` | 说明固件功能、操作方式、环境要求和实机测试结果 |
| `assets/logo.*` | 可选的项目标识图片 |
| `assets/preview.*` | 展示固件在 Sticky 上真实运行的照片或截图 |
| `source/` | 源码模式使用的可编译工程 |
| `source/LICENSE` | 本地提交源码对应的许可证 |
| `firmware/<version>/manifest.json` | 仅固件包模式使用的烧录清单 |
| `firmware/<version>/*.bin` | 仅固件包模式提交的可烧录文件 |

目录名和 `integration.json` 中的 `id` 必须使用相同的小写连字符格式，例如
`weather-dashboard` 或 `sticky-2048`。

`integrations/<integration-id>/assets/` 下的图片支持 `.png`、`.jpg`、
`.jpeg`、`.webp` 和静态 `.svg` 格式。真实设备效果图为必需项，项目 Logo
为可选项。

一句话总结：源码模式交源码给 Action 生成固件；仅固件模式直接提交可烧录文件。

## 创建一份贡献

在仓库根目录执行：

```bash
cp -R integrations/_template integrations/my-firmware
```

按下面的顺序准备：

1. 修改目录名和 `integration.json.id`。
2. 填写必需的作者名称，并按需填写作者链接和展示来源。
3. 在项目 README 和 `integration.json` 中提供上游源码地址与许可证名称。
4. 在 `assets/` 中加入真实设备效果图；项目需要 Logo 时一并加入。
5. 选择源码模式时，加入 `source/`、`build` 配置和 `sourceBuild: true`。
6. 选择仅固件包模式时，把经过测试的固件包放入 `firmware/<version>/`。
7. 运行 Registry 测试和校验。
8. 把这套固件烧录到真实 reTerminal Sticky 上测试。
9. 提交 Pull Request，并写清测试结果。

一句话总结：准备项目资料和所选交付方式需要的文件后，再进入审核。

## integration.json

普通第三方贡献统一使用：`"group": "community"`、
`"catalogSection": "community"`、`"mode": "flash"`。

```json
{
  "schemaVersion": 1,
  "id": "my-firmware",
  "name": "My Firmware",
  "group": "community",
  "catalogSection": "community",
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

### 关键字段

| 字段 | 社区贡献填写方式 |
|---|---|
| `group` | 固定为 `community` |
| `catalogSection` | 固定为 `community` |
| `mode` | 固定为 `flash` |
| `status` | 按成熟度填写 `experimental`、`beta` 或 `stable` |
| `author.name` | 必填，网站显示的作者或团队名称 |
| `author.url` | 选填，点击作者名称时打开的 HTTPS 链接 |
| `origin.name` | 选填，网站显示的来源名称 |
| `origin.url` | 选填，点击来源名称时打开的 HTTPS 链接 |
| `source.url` | 两种贡献方式都填写上游源码仓库地址 |
| `source.license` | 源码许可证名称；仅固件包模式必须填写 |
| `source.path` | 源码模式填写本地源码目录，通常为 `source` |
| `build.*` | 源码模式与 `source.path` 一起提供的构建配置 |
| `flash.versions[].sourceBuild` | 源码由 GitHub Actions 编译时设置为 `true` |
| `flash.versions[].manifestPath` | 仅固件包模式填写项目内的 manifest 路径 |

`author` 和 `origin` 用于网站署名展示；`source` 保存审核与固件打包使用的源码仓库、
许可证和本地编译路径。

`official` 和 `platform` 区域由 Seeed 或合作平台共同维护。合作伙伴条目使用
`"group": "partner"`、`"catalogSection": "platform"` 和项目官方链接。合作伙伴
与官方卡片可以省略作者署名，因为平台身份已经由条目名称和官方链接明确表达。
普通外部 PR 统一进入 `community` 区域。维护者可以把历史迁移但资料尚未齐全的
条目标记为 `draft`；草稿不会出现在 Sticky Playground。

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

一句话总结：提交源码时由 Action 产出固件；直接提交固件时由作者提供完整烧录包。

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

作者可以在提交 PR 前先做一次本地编译。安装 `integration.json` 中声明的 ESP-IDF
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

一句话总结：这一步负责提前发现“少文件、固件不匹配、烧录地址错误”等问题。

源码模式的 manifest 和固件文件检查会在 Action 编译后执行；仅固件包模式直接检查
PR 中提交的文件。

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
4. 保留仍被 `integration.json` 引用的旧版本目录。
5. 重新运行自动检查和真实设备测试。
6. 在 PR 中说明用户能看到的变化和升级后的行为。

一句话总结：每个版本保留测试结果；源码模式的历史固件保存在对应 GitHub Release。

## PR 提交前清单

- [ ] 一个项目目录包含本次完整贡献。
- [ ] `integration.json` 使用 `community` + `community` + `flash`。
- [ ] 已选择“源码 + 构建配置”或“仅固件包 + 上游源码地址与许可证”。
- [ ] 源码模式使用 `sourceBuild: true`，或仅固件包模式包含 manifest 和全部必需 `.bin`。
- [ ] README 和 PR 写明经过测试的固件来源和固件版本。
- [ ] `npm test` 和 `npm run validate` 全部通过。
- [ ] 这套固件已在真实 reTerminal Sticky 上完成测试。
- [ ] PR 写明测试硬件、固件版本、测试结果，以及适用时的编译版本。
