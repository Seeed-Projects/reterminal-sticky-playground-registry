# 为 reTerminal Sticky Playground 贡献固件

这个仓库是 reTerminal Sticky 固件合集面向外部开发者的公开贡献与审核入口。
一份完整的社区贡献通过审核后，会在 Sticky 官网 Playground 中生成一张固件卡片，
并进入由 Seeed 网站提供的浏览器烧录页面。

贡献者可以选择提交“完整源码 + 固件包”，也可以只提交经过验证的固件包并提供上游
源码链接。两种方式都需要项目信息、展示图片和实机测试记录。Sticky 私有网站只读取
经过审核并锁定版本的公开仓库内容，再生成卡片和烧录页。

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

每个项目在 `integrations/` 下建立一个独立目录。完整源码模式包含 `source/`，仅固件包
模式可以省略这个目录：

```text
integrations/
  my-firmware/
    integration.json
    README.md
    assets/
      logo.svg
      preview.jpg
    source/
      CMakeLists.txt
      sdkconfig.defaults
      main/
      components/
      LICENSE
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
| `assets/logo.*` | 在固件合集里显示项目标识 |
| `assets/preview.*` | 展示固件在 Sticky 上真实运行的照片或截图 |
| `source/` | 完整源码模式使用的可编译工程 |
| `source/LICENSE` | 本地提交源码对应的许可证 |
| `firmware/<version>/manifest.json` | 记录烧录地址、文件大小和 SHA-256 |
| `firmware/<version>/*.bin` | 可直接烧录的固件文件 |

目录名和 `integration.json` 中的 `id` 必须使用相同的小写连字符格式，例如
`weather-dashboard` 或 `sticky-2048`。

一句话总结：一个 PR 必须说明“代码在哪里”，并提供用户可以直接烧录的固件包。

## 创建一份贡献

在仓库根目录执行：

```bash
cp -R integrations/_template integrations/my-firmware
```

两种贡献方式都按下面的顺序准备：

1. 修改目录名和 `integration.json.id`。
2. 在项目 README 和 `integration.json` 中提供上游源码地址与许可证名称。
3. 在 `assets/` 中加入 Logo 和真实设备效果图。
4. 把经过测试的固件包放入 `firmware/<version>/`。
5. 选择完整源码模式时，再加入 `source/` 和对应的 `build` 配置。
6. 运行 Registry 测试和校验。
7. 把这套固件烧录到真实 reTerminal Sticky 上测试。
8. 提交 Pull Request，并写清测试结果。

一句话总结：源码信息、固件包、页面资料和实机结果齐全后，再进入审核。

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
    "logo": "assets/logo.svg",
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
        "manifestPath": "firmware/1.0.0/manifest.json"
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
| `source.url` | 两种贡献方式都填写上游源码仓库地址 |
| `source.license` | 源码许可证名称；仅固件包模式必须填写 |
| `source.path` | 完整源码模式填写本地源码目录，通常为 `source` |
| `build.*` | 完整源码模式与 `source.path` 一起提供的构建配置 |
| `flash.versions[].manifestPath` | 当前项目目录内的 manifest 路径 |

`official` 和 `platform` 区域由 Seeed 或合作平台共同维护。普通外部 PR 统一进入
`community` 区域。维护者可以把历史迁移但资料尚未齐全的条目标记为 `draft`；
草稿不会出现在 Sticky Playground。

一句话总结：社区条目需要有明确源码地址、有本地固件包，并且可以直接烧录。

## 两种贡献方式

### 完整源码和固件包

提交 `source/`，设置 `source.path`，并加入配套的 `build` 配置。源码和固件包属于同一
版本。对于已支持的构建系统，CI 会重新编译并把结果与提交的固件逐一比较。

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

一句话总结：有条件时可以提交完整工程，也可以直接提交经过验证的 bin 固件包。

## 完整源码模式要求

完整源码模式的 `source/` 必须能够仅依靠本次提交的文件完成编译。它应包含工程构建
文件、应用代码、本地组件、依赖清单或锁定文件、默认配置和许可证。

Wi-Fi 密码、API Key、Token 和密码等用户私密信息使用占位符或首次运行配置方式。
PR 中的源码和固件包必须属于同一个版本。

第一阶段的自动编译流程支持 ESP-IDF，常见目录如下：

```text
source/
  CMakeLists.txt
  sdkconfig.defaults  # 包含 CONFIG_APP_REPRODUCIBLE_BUILD=y
  main/
    CMakeLists.txt
    main.cpp
  components/
  LICENSE
```

需要其他编译系统的项目，先提交 Issue，让维护者先为该编译系统加入可重复执行的 CI
构建适配，再提交正式固件 PR。

ESP-IDF 社区项目必须在 `source/sdkconfig.defaults` 中启用
`CONFIG_APP_REPRODUCIBLE_BUILD=y`。这个设置会移除编译时间和本机路径差异，让 CI
能够确认 PR 中的源码确实可以生成同一套固件包。

一句话总结：源码必须让审核机器能够从零重新编译，而不是只留一个外部链接。

## 编译并整理 ESP-IDF 固件

安装 `integration.json` 中声明的 ESP-IDF 版本，然后进入项目的 `source/` 目录执行：

```bash
idf.py set-target esp32s3
idf.py build
```

编译完成后回到 Registry 仓库根目录，执行：

```bash
npm run package:esp-idf -- my-firmware 1.0.0
```

这个命令会读取 `source/build/flasher_args.json`，复制烧录所需的全部 `.bin`，并生成
`firmware/1.0.0/manifest.json`。manifest 会准确记录每个文件的烧录地址、字节大小和
SHA-256（通俗解释：用于确认文件没有被替换或损坏的数字指纹）。

生成的 manifest 和 `.bin` 与源码一起提交。审核者因此既能重新编译，也能直接测试
最终用户将要烧录的同一套文件。

一句话总结：作者负责完成编译，仓库工具负责把编译结果整理成网站能识别的固件包。

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
- 每个固件文件存在，大小和 SHA-256 一致；
- manifest 包含 ESP-IDF 烧录表中的全部文件和地址；
- 不同固件分区的烧录地址没有重叠；
- 社区条目满足本站直接烧录的全部要求。

一句话总结：这一步负责提前发现“少文件、固件不匹配、烧录地址错误”等问题。

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

PR 需要包含所选贡献方式对应的内容、固件包、项目资料、图片和实机测试结果。
GitHub Actions 会依次执行：

1. 检查 Registry 结构和本地固件包。
2. 在干净环境中重新编译采用完整源码模式的 ESP-IDF 项目。
3. 对这些项目，把重新编译的结果与 PR 中的每个 `.bin` 和 SHA-256 逐一比较。

当这些检查通过，并且维护者确认项目用途、许可证、兼容性和实机结果后，PR 才进入合并。

一句话总结：作者提供成品，自动流程证明“源码确实能生成这套成品”。

## 合并后如何进入 Sticky 官网

Registry PR 合并后不会立刻进入正式生产站。维护者按照下面的顺序发布：

1. 合并资料完整且检查通过的 Registry PR。
2. 在 Sticky 固定测试分支更新所锁定的 Registry 提交。
3. 本地构建 Sticky，确认新卡片和烧录页面正确生成。
4. 从本地 Sticky 页面把固件烧录到真实设备并验收。
5. 把测试通过的 Sticky 分支合并到 Sticky `main`。
6. 由公司服务器构建网站，并通过 Kubernetes 发布。

这条流程把“外部贡献审核”“实机验收”和“正式上线”分成三个明确阶段，同时保持
Sticky 网站仓库闭源。

一句话总结：合并公开 PR 是进入测试，Sticky 实机验收通过后才进入官网。

## 更新已有固件

发布新版本时：

1. 在 `flash.versions` 最前面加入新版本。
2. 把经过测试的固件包放入 `firmware/<version>/`。
3. 完整源码模式同时更新 `source/`，并执行 `npm run package:esp-idf -- <id> <version>`。
4. 保留仍被 `integration.json` 引用的旧版本目录。
5. 重新运行自动检查和真实设备测试。
6. 在 PR 中说明用户能看到的变化和升级后的行为。

一句话总结：每个版本都保留固件包和测试结果；完整源码模式同时保留对应源码。

## PR 提交前清单

- [ ] 一个项目目录包含本次完整贡献。
- [ ] `integration.json` 使用 `community` + `community` + `flash`。
- [ ] 已选择“完整源码 + 构建配置”或“仅固件包 + 上游源码地址与许可证”。
- [ ] `firmware/<version>/` 包含 manifest 和全部必需 `.bin`。
- [ ] README 和 PR 写明经过测试的固件来源和固件版本。
- [ ] `npm test` 和 `npm run validate` 全部通过。
- [ ] 这套固件已在真实 reTerminal Sticky 上完成测试。
- [ ] PR 写明测试硬件、固件版本、测试结果，以及适用时的编译版本。
