# 为 reTerminal Sticky Playground 贡献内容

这个仓库是 [reTerminal Sticky Playground](https://www.seeedstudio.com/sticky/playground/)
面向外部开发者和创客的公开贡献与审核入口。访客在 Playground 的 **Firmware** 和
**3D Printables** 两个页面看到的所有内容，都来自这个仓库里的文件。你在这里新增或
修改一个目录，提交 pull request，维护者审核合并，Sticky 网站在下一次发布时把它带上线。

英文版请查看 [CONTRIBUTING.md](CONTRIBUTING.md)。

## 目录

1. [选择你的指南](#1-选择你的指南)
2. [需要准备什么](#2-需要准备什么)
3. [仓库结构](#3-仓库结构)
4. [一份贡献如何到达网站](#4-一份贡献如何到达网站)
5. [三种提交方式](#5-三种提交方式)
6. [所有贡献共同遵守的规则](#6-所有贡献共同遵守的规则)
7. [本地自检](#7-本地自检)
8. [PR 审核](#8-pr-审核)
9. [PR 合并之后](#9-pr-合并之后)
10. [更新或移除已有贡献](#10-更新或移除已有贡献)
11. [常见问题](#11-常见问题)

## 1. 选择你的指南

贡献分两类，各有独立的目录、元数据文件、模板和详细指南。

| 我想分享… | 目录 | 元数据文件 | 复制哪个模板 | 详细指南 |
|---|---|---|---|---|
| 用户可以在 Sticky 网站直接烧录的固件 | `firmwares/<firmware-id>/` | `firmware.json` | `firmwares/_template/` | [贡献固件](docs/contributing-firmware.zh-CN.md) |
| 3D 打印的外壳、支架、安装件或配件 | `printables/<design-id>/` | `printable.json` | `printables/_template/` | [贡献 3D 打印设计](docs/contributing-printables.zh-CN.md) |

- **固件**贡献需要提交可编译的 ESP-IDF 源码（由 GitHub Actions 编译），或者一个
  带 manifest 的、经过验证的 `.bin` 固件包，并在真机上测试后才能审核。第一次提交
  预计一到两小时。
- **打印件**贡献只是一张卡片：一个 JSON 文件、一张照片、一份简短 README。模型文件
  留在你自己的 Printables / MakerWorld / Thingiverse / GrabCAD / GitHub 页面。
  预计 15–30 分钟。

**打印件还有一条快速通道。**
[3D Printables 页面](https://www.seeedstudio.com/sticky/playground/3d-printables/)
上的 **Share your design** 表单，会用网页表单问一份精简版的同类信息，照片直接上传，然后替你在
本仓库开好 pull request。不需要 GitHub 账号，不需要 git，也不需要写 JSON，大约五分钟
就能完成。提交后页面会给出 PR 链接，方便你跟进审核。如果你更习惯用 git，或者要更新
一个已经上线的设计，就走下面的手动流程。

本页只讲两类贡献共同的部分。读完一遍，再去看你那一类的详细指南。

一句话总结：做固件看固件指南，做外壳看打印指南，本页是两条路共用的入口。

## 2. 需要准备什么

| 条件 | 固件 | 打印件 | 说明 |
|---|---|---|---|
| GitHub 账号 | 需要 | 只有手动流程需要 | 免费账号即可；走官网表单不需要账号 |
| 电脑上安装 Git | 推荐 | 可选 | 打印件可以完全在 GitHub 网页里完成，见[方式 B](#方式-bgithub-网页操作不需要-git) |
| Node.js 20 或更高 | 推荐 | 可选 | 用来在本地跑和 GitHub Actions 相同的检查 |
| 一台 reTerminal Sticky | 需要 | 需要 | 固件必须在真机上烧录测试；打印件需要一张装在真机上的照片 |
| ESP-IDF | 可选 | 不需要 | 只有想在提交前本地编译固件时才需要 |

## 3. 仓库结构

```text
firmwares/                     每个固件一个目录
  _template/                   新固件从这里复制
  <firmware-id>/
    firmware.json              卡片文字、链接、编译设置、版本列表
    README.md                  功能说明、操作方式、配置、真机测试记录
    assets/
      preview.jpg              真实的 Sticky 截图或照片
      logo.svg                 可选的标识图
    source/                    源码模式：ESP-IDF 工程
    firmware/<version>/        仅固件包模式：manifest.json + *.bin

printables/                    每个打印设计一个目录
  _template/                   新设计从这里复制
  <design-id>/
    printable.json             卡片文字、作者、分类、下载页
    README.md                  打印参数和组装说明
    assets/
      preview.jpg              打印成品装在 Sticky 上的照片

schemas/
  firmware.schema.json         firmware.json 的正式定义
  printable.schema.json        printable.json 的正式定义

scripts/
  validate-registry.mjs        检查全部目录；通过 npm run validate 运行
  create-flash-manifest.mjs    生成固件 manifest；通过 npm run create:manifest 运行
  list-build-targets.mjs       为 GitHub Actions 找出需要编译的源码固件
  package-esp-idf.mjs          把 ESP-IDF 编译结果整理成 manifest.json + .bin

docs/
  contributing-firmware.md     固件详细指南（另有 .zh-CN.md）
  contributing-printables.md   打印件详细指南（另有 .zh-CN.md）

.github/
  workflows/validate-registry.yml   每个 PR 和 main 分支都会运行
  pull_request_template.md          你提交 PR 时要填写的清单
```

目录名就是这份贡献的公开标识。固件的目录名会出现在网站地址里
（`/sticky/playground/<firmware-id>/`），两类贡献的目录名都会出现在统计数据里。
请认真取名，之后改名需要再提一个 PR。

## 4. 一份贡献如何到达网站

```text
1. 你提交一个 pull request，新增或修改一个目录。
2. GitHub Actions 校验仓库；源码模式固件还会被编译。
3. 维护者审核 PR 并合并到 main。
4. 源码模式固件：main 分支的工作流发布带 .bin 文件的 GitHub Release。
5. Sticky 网站仓库锁定这个新的 Registry 提交。
6. 网站构建、本地验证、真机验收，然后部署。
7. 你的卡片出现在 https://www.seeedstudio.com/sticky/playground/。
```

第 1–2 步由你完成，第 3–7 步由维护者完成，详见[第 9 节](#9-pr-合并之后)。

## 5. 三种提交方式

按你对 git 的熟悉程度选一种。三种方式最后都会向
`Seeed-Projects/reterminal-sticky-playground-registry` 的 `main` 分支提交 pull request。

### 方式 A：Fork 后用 git 操作（固件推荐）

1. 在 GitHub 上 Fork 仓库：打开
   <https://github.com/Seeed-Projects/reterminal-sticky-playground-registry>，点击 **Fork**。
2. 克隆你的 Fork 并新建分支：

   ```bash
   git clone https://github.com/<你的账号>/reterminal-sticky-playground-registry.git
   cd reterminal-sticky-playground-registry
   git checkout -b add-my-design
   ```

3. 复制对应类型的模板并填写（细节见各自的指南）：

   ```bash
   cp -R printables/_template printables/my-design     # 打印件
   cp -R firmwares/_template  firmwares/my-firmware    # 固件
   ```

4. 运行本地检查：

   ```bash
   npm test
   npm run validate
   ```

5. 提交并推送：

   ```bash
   git add printables/my-design
   git commit -m "feat: add My Design printable"
   git push -u origin add-my-design
   ```

6. 打开 pull request。GitHub 会在你的 Fork 页面显示 **Compare & pull request** 按钮，
   点击后确认目标是 `Seeed-Projects/reterminal-sticky-playground-registry` 的 `main`，
   然后填写描述框里自动出现的模板。

### 方式 B：GitHub 网页操作（不需要 git）

适合打印件，以及固件元数据的小修改。

1. Fork 仓库（同方式 A 第 1 步）。
2. 在你的 Fork 里打开 `printables/` 目录，点击 **Add file → Create new file**。
3. 在文件名框里输入 `my-design/printable.json`。输入 `/` 会自动创建目录。把
   [`printables/_template/printable.json`](printables/_template/printable.json)
   的内容粘贴进来并修改。
4. 点击 **Commit changes…**，选择 **Create a new branch**，命名为 `add-my-design`，提交。
5. 仍在这个分支上，打开 `printables/my-design/`，点击 **Add file → Upload files**
   上传照片：把文件拖进去，然后把上传区域上方显示的路径改成
   `printables/my-design/assets/preview.jpg`，提交。
6. 用 **Create new file** 重复第 5 步，创建 `printables/my-design/README.md`。
7. GitHub 会显示一条横幅，提示从你的分支发起 pull request。点击它，确认目标是
   Seeed 仓库的 `main`，填写模板。

GitHub Actions 会运行与 `npm run validate` 相同的检查；如果有问题，PR 会显示红叉，
日志会告诉你要改哪个字段。在你的分支上修改文件，检查会自动重新运行。

### 方式 C：在浏览器里直接改已有文件（只用于更新）

修正错别字、更换链接、给已有卡片加版本时：

1. 在 GitHub 上打开那个文件（例如 `printables/sticky-wallet-case/printable.json`）。
2. 点击铅笔图标（**Edit this file**）。如果需要，GitHub 会自动帮你 Fork。
3. 修改内容，点击 **Commit changes…**，选择 **Propose changes**。
4. GitHub 打开 pull request 表单，填写模板。

## 6. 所有贡献共同遵守的规则

以下规则由 `npm run validate` 和人工审核共同保证。

| 规则 | 说明 |
|---|---|
| 一个目录一份贡献 | 一个固件或一个设计的全部文件都放在自己的目录里。同一个 PR 不要改动其他目录。 |
| 目录名 = `id` | 只用小写字母、数字和单个连字符：`^[a-z0-9]+(?:-[a-z0-9]+)*$`，2–64 个字符。例如 `sticky-2048`、`wallet-case`。元数据文件里的 `id` 必须与目录名完全一致。 |
| 只用 HTTPS 链接 | 所有网址字段必须以 `https://` 开头。 |
| 图片放在 `assets/` 下 | 引用写成 `assets/<文件名>`。固件支持 PNG、JPG、WebP、静态 SVG；打印件支持 PNG、JPG、WebP。预览图最大 5 MB，标识图最大 1 MB。 |
| 真实图片 | 预览图必须是 reTerminal Sticky 真机上的截图或照片，不接受渲染图或素材图。 |
| 卡片文字用英文 | `name`、`summary`、`description`、`alt` 和 README 使用英文，因为网站面向全球访客。 |
| 不含密钥 | 源码、配置和 README 中不包含 Wi-Fi 密码、API key、token 或私钥。用占位符代替，并说明运行时如何配置。 |
| 自己的作品或明确署名 | `author` 写创作者本人或团队。替他人提交开源设计或固件时，`author` 写原作者，`download.url` / `source.url` 指向原始页面。 |
| 元数据文件是严格的 | 出现未知字段会导致校验失败。请复制模板后修改值，不要自行添加新键。 |

## 7. 本地自检

安装 [Node.js](https://nodejs.org/) 20 或更高版本，在仓库根目录执行：

```bash
npm test          # 校验脚本自身的单元测试
npm run validate  # 检查 firmwares/ 和 printables/ 下的全部目录
```

成功时输出：

```text
Registry validation passed (12 firmware(s), 5 printable(s)).
```

失败时会逐条列出问题，精确到文件和字段：

```text
Registry validation failed with 2 error(s):
- printables/my-design/printable.json.download.url: must use HTTPS
- printables/my-design/printable.json.preview.image: references a missing file: assets/preview.jpg
```

文件名后面的部分是 JSON 里的路径：`download.url` 表示 `download` 对象里的 `url`
键。改好字段，再跑一次，直到通过。

校验脚本对两类贡献都会检查：

- 目录名与 `id` 一致；
- 必填字段齐全，且没有未知字段；
- 字符串在长度限制内，枚举字段使用允许的值；
- 网址使用 HTTPS；
- 引用的图片存在、后缀允许、内容是真实图片数据、大小不超限；
- `README.md` 存在。

固件还会额外检查源码目录、编译设置、manifest、`.bin` 大小、SHA-256 和烧录地址，
见固件指南。

一句话总结：先在自己电脑上让校验变绿，PR 上就不会红。

## 8. PR 审核

1. 提交 pull request 并填写模板。**Contribution type** 只勾一项，只需完成对应的那一节。
2. GitHub Actions 运行 `npm test` 和 `npm run validate`。源码模式固件还会被编译，
   固件作为 PR 产物附在检查结果里。出现红叉说明有失败，点 **Details** 看日志。
3. 一两分钟后，PR 上会出现一条**审核卡**评论：它把卡片按审核者看到的样子展示出来，
   包括预览照片、链接是否可以访问，以及需要留意的地方，例如缺少许可证、diff 里残留
   了密码。每次推送都会重写这条评论，所以它始终反映分支的最新状态。
4. 维护者审核元数据、图片、链接、许可证，固件贡献还会核对真机测试记录。审核意见会
   以评论形式出现在 PR 上；回复或直接往同一分支推送修改即可。
5. 检查全绿且审核完成后，维护者合并。

一般几个工作日内会有回应。如果 PR 超过两周没有动静，请在 PR 里留言提醒。

## 9. PR 合并之后

合并不会立刻上线。Sticky 网站只读取一个锁定的 Registry 提交，维护者按批次发布：

1. 合并 Registry pull request。
2. 源码模式固件需等待 Registry `main` 的工作流发布固件 Release。
3. 在 Sticky 网站仓库更新锁定的 Registry 提交。
4. 本地构建网站，确认新卡片（固件还包括烧录页）正确生成。
5. 固件从本地页面烧录到真机验收。
6. 合并到 Sticky 网站 `main` 并部署。

第 6 步完成后，你的卡片出现在 <https://www.seeedstudio.com/sticky/playground/>。

一句话总结：合并公开 PR 是进入待发布队列，网站更新锁定版本后才正式上线。

## 10. 更新或移除已有贡献

**更新。** 修改你目录下的文件，用上面任一方式再提一个 pull request。模板里勾选
**Firmware: update to an existing firmware** 或 **3D printable design**。固件更新把
新版本放在 `flash.versions` 最前面，细节见固件指南。

**移除。** 提交一个删除该目录的 pull request，并在描述里说明原因。如果仍有用户依赖，
维护者可能保留目录并标记为不再维护。

**转交。** 由他人接手维护时，提交一个修改 `author` 和链接的 PR，并在描述里提到
原作者，方便对方确认。

## 11. 常见问题

**一个 PR 能提交多个设计或固件吗？**
请一个目录一个 PR。这样审核更聚焦，每张卡片也能独立发布。

**我的设计发在多个平台，`download.url` 填哪个？**
填你维护得最勤的那个页面，其余的在 README 里提一下。

**我发现别人做的设计很适合 Sticky，可以帮忙加上吗？**
可以，前提是其许可证允许转发链接并注明出处。`author` 写原设计者，`download.url`
指向对方的页面，并在 PR 里说明你是代为提交。

**必须在本地跑 `npm run validate` 吗？**
不是必须。GitHub Actions 会在 PR 上运行它。本地跑只是几秒钟就能拿到结果，而不用等几分钟。

**校验说 `contains unsupported field "..."`。**
元数据文件只接受各自指南里列出的字段。删掉多出来的键，或检查合法键名是否拼错。

**提交前怎么在自己的设备上测试固件？**
用 ESP-IDF 本地编译并 `idf.py flash`，或者提交 PR 后使用 GitHub Actions 生成的 PR
产物。固件指南对两种方式都有说明。

**有不清楚的地方问谁？**
在本仓库开一个 issue，或直接在 pull request 里提问。
