# 贡献 3D 打印设计

本指南面向 Sticky Playground [**3D Printables**](https://www.seeedstudio.com/sticky/playground/3d-printables/)
页面的外壳、支架、安装件和配件贡献，写给在 Printables、MakerWorld、Thingiverse、
GrabCAD 或 GitHub 上发布模型的创客。你不需要了解任何固件或 ESP-IDF 知识。

英文指南请查看 [contributing-printables.md](contributing-printables.md)。
与固件贡献共用的规则、三种提交 PR 的方式以及发布流程，见
[CONTRIBUTING.zh-CN.md](../CONTRIBUTING.zh-CN.md)。

## 五分钟通道：官网表单

[3D Printables 页面](https://www.seeedstudio.com/sticky/playground/3d-printables/)
上的 **Share your design** 按钮会打开一个简短表单：设计名称、分类、一句话简介、
一张照片、下载页面和作者名。提交之后，服务会替你
写好 `printable.json`、`README.md` 和 `assets/preview.<扩展名>`，并在本仓库开好
pull request，页面上会直接给出这个 PR 的链接，随时可以查看审核进度。整个过程不需要
GitHub 账号，不需要 git，也不需要手写 JSON。

如果你更习惯自己准备文件、要更新一个已经上线的设计，或者想弄清楚表单到底生成了什么，
继续往下读。两条路径的产物完全一样，校验也完全一样。

一句话总结：想快就用官网表单，想自己掌控就照下面的步骤来。

## 目录

1. [你需要提交什么](#1-你需要提交什么)
2. [访客会看到什么](#2-访客会看到什么)
3. [目录里的文件](#3-目录里的文件)
4. [printable.json 字段说明](#4-printablejson-字段说明)
5. [分类](#5-分类)
6. [预览照片](#6-预览照片)
7. [README](#7-readme)
8. [分步操作](#8-分步操作)
9. [校验报错与修法](#9-校验报错与修法)
10. [更新已有设计](#10-更新已有设计)
11. [真实示例](#11-真实示例)
12. [提交前清单](#12-提交前清单)

## 1. 你需要提交什么

你提交的是一张**卡片**，不是模型文件：

| 文件 | 作用 | 必需 |
|---|---|---|
| `printables/<design-id>/printable.json` | 卡片上显示的文字和链接 | 是 |
| `printables/<design-id>/assets/preview.jpg` | 打印成品装在真机 reTerminal Sticky 上的照片 | 是 |
| `printables/<design-id>/README.md` | 打印参数、组装说明，以及下载链接 | 是 |

STL / 3MF / STEP 文件留在你自己的下载页。Sticky 网站展示你的卡片，并通过
**View on …** 按钮把访客送到那个页面。文件的控制权始终在你手里：更新文件、增加
remix、更换许可证都不需要再到这里提 PR。

一句话总结：你交一张名片和一张照片，文件放在你自己那里。

## 2. 访客会看到什么

3D Printables 页面上，每个设计是网格里的一张卡片，左侧有分类筛选。一张卡片从上到下是：

1. 你的 `preview.image` 照片（裁成约 4:3）；
2. 分类标签（例如 **Cases**）和 `tags` 里的标签；
3. `name` 作为标题；
4. `summary` 显示在标题下方一到两行；
5. 一行 **Author**，显示 `author.name`，如果填了 `author.url` 会带链接；
6. 一个通栏按钮 **View on `<download.platform>`**，在新标签页打开 `download.url`；不填 `download.platform` 时按钮显示 **View download page**。

`description` 是选填的，只保存在条目里供 GitHub 上的读者查看，卡片显示的是
`summary`，所以最关键的信息要写在 `summary` 里。

## 3. 目录里的文件

```text
printables/
  my-case/                 <- 目录名 = id
    printable.json         <- 卡片元数据（本指南第 4 节）
    README.md              <- 打印参数和组装说明（第 7 节）
    assets/
      preview.jpg          <- preview.image 引用的照片（第 6 节）
```

目录名（`<design-id>`）的规则：

- 只用小写字母、数字和单个连字符，2–64 个字符；
- 必须匹配正则 `^[a-z0-9]+(?:-[a-z0-9]+)*$`；
- 描述设计本身而不是你：`wallet-case`、`sticky-desk-stand`、`fridge-magnet-mount`；
- 必须与 `printable.json` 里的 `id` 完全一致。

目录里放其他东西（额外照片、`LICENSE` 文件）是允许的，网站会忽略。请不要放模型文件；
提交了 STL/3MF/STEP 的 PR 会被要求删掉。

## 4. printable.json 字段说明

`printable.json` 是一个严格的 JSON 对象。下面每个键要么必填要么可选；
**出现任何其他键都会导致校验失败**。正式定义见
[`schemas/printable.schema.json`](../schemas/printable.schema.json)。

### 顶层字段

| 字段 | 类型 | 必填 | 限制 | 填什么 |
|---|---|---|---|---|
| `schemaVersion` | 整数 | 是 | 必须是 `1` | 固定写 `1` |
| `id` | 字符串 | 是 | 2–64 字符，`^[a-z0-9]+(?:-[a-z0-9]+)*$` | 与目录名相同 |
| `name` | 字符串 | 是 | 1–80 字符 | 卡片标题，例如 `Sticky Wallet Case` |
| `category` | 字符串 | 是 | `case`、`stand`、`mount`、`accessory`、`reference`、`other` 之一 | 决定卡片出现在哪个筛选项下，见[第 5 节](#5-分类) |
| `summary` | 字符串 | 是 | 1–140 字符 | 标题下的一句话：它是什么、最有用的一点是什么 |
| `description` | 字符串 | 否 | 1–800 字符 | 两三句话：如何贴合 Sticky、推荐材料、打印前要知道的事 |
| `author` | 对象 | 是 | 见下 | 设计者 |
| `download` | 对象 | 是 | 见下 | 文件在哪里 |
| `preview` | 对象 | 是 | 见下 | 卡片上的照片 |
| `tags` | 字符串数组 | 否 | 最多 6 项，每项 1–32 字符，不重复 | 显示在分类旁的短标签，例如 `snap-on`、`usb-c`、`tpu` |

### `author`

| 字段 | 类型 | 必填 | 限制 | 填什么 |
|---|---|---|---|---|
| `author.name` | 字符串 | 是 | 1–80 字符 | 显示在卡片上的个人或团队名 |
| `author.url` | 字符串 | 否 | HTTPS 网址，最长 2048 | 点击名字时打开的主页：Printables/MakerWorld 个人页、GitHub 或个人网站 |

### `download`

| 字段 | 类型 | 必填 | 限制 | 填什么 |
|---|---|---|---|---|
| `download.platform` | 字符串 | 否 | 1–40 字符 | 托管文件的网站名，会变成按钮文字 **View on `<platform>`**；不填时按钮显示 **View download page**。按网站自己的写法：`Printables`、`MakerWorld`、`Thingiverse`、`GrabCAD`、`GitHub`、`Cults3D`，或你自己的站名 |
| `download.url` | 字符串 | 是 | HTTPS 网址，最长 2048 | 访客下载模型的公开页面（或直接的文件链接） |
| `download.license` | 字符串 | 否 | 1–80 字符 | 下载页标注的许可证，按平台的写法：`CC BY 4.0`、`CC BY-SA 4.0`、`CC BY-NC 4.0`、`MIT`、`GPL-3.0` |

### `preview`

| 字段 | 类型 | 必填 | 限制 | 填什么 |
|---|---|---|---|---|
| `preview.image` | 字符串 | 是 | 必须匹配 `^assets/[A-Za-z0-9._-]+\.(png\|jpg\|jpeg\|webp)$`；文件 ≤ 5 MB | 照片在目录内的路径，通常是 `assets/preview.jpg` |
| `preview.alt` | 字符串 | 是 | 1–180 字符 | 一句话描述照片内容，供读屏软件和图片搜索使用，例如 `Black PETG wallet case fitted on reTerminal Sticky, front view` |

### 完整示例

```json
{
  "schemaVersion": 1,
  "id": "my-case",
  "name": "My Case",
  "category": "case",
  "summary": "A slim snap-on case that keeps the USB-C port and side button open.",
  "description": "My Case wraps the edges of reTerminal Sticky and leaves the screen, side button, and USB-C port uncovered. Print it in PETG for a firm fit or TPU for a softer bumper. The lip is 1.2 mm thick and clears the e-paper bezel.",
  "author": {
    "name": "Your name",
    "url": "https://www.printables.com/@your-profile"
  },
  "download": {
    "platform": "Printables",
    "url": "https://www.printables.com/model/123456-my-case",
    "license": "CC BY-SA 4.0"
  },
  "preview": {
    "image": "assets/preview.jpg",
    "alt": "My Case printed in black PETG fitted on reTerminal Sticky"
  },
  "tags": ["snap-on", "usb-c", "petg"]
}
```

写作建议：

- `name`：首字母大写的标题，结尾不加标点，不用写 "for reTerminal Sticky"（每张卡都是给 Sticky 的）。
- `summary`：一个完整句子，句号结尾，不堆形容词。
- `description`：选填；要写就说明贴合方式、材料，以及需要的五金件（磁铁、螺丝、毛毡垫）。
- 所有文字使用英文，网站面向全球访客。

## 5. 分类

`category` 决定你的卡片出现在页面左侧哪个筛选项下。按设计的主要用途选一个。

| 值 | 筛选项名称 | 适用范围 | 例子 |
|---|---|---|---|
| `case` | Cases | 包裹设备本体的外壳和保护套 | 钱包式外壳、卡扣式保护套、带盖全包外壳 |
| `stand` | Stands | 让 Sticky 立在桌面或斜放的支架 | 桌面支架、充电支架、画架式支架 |
| `mount` | Mounts | 把 Sticky 固定到其他物体上的部件 | 冰箱磁吸座、墙面支架、显示器夹、车把夹 |
| `accessory` | Accessories | 以上都不属于的附加件 | 触控笔座、线夹、挂绳环、收纳盒 |
| `reference` | Reference Models | Sticky 本体的模型，用于设计其他部件 | 外壳 CAD 模型、尺寸模板、试装夹具 |
| `other` | Others | 以上分类都不适用的设计 | 打印夹具、包装内衬、一次性试验件 |

一个设计兼具两种用途（既是外壳又是支架）时，选访客最可能先搜的那个，另一个写进 `tags`。

## 6. 预览照片

照片是访客第一眼看到的东西，也是他们打开你页面前判断贴合度的唯一依据。

| 要求 | 说明 |
|---|---|
| 真实照片 | 打印成品装在真机 reTerminal Sticky 上。渲染图、切片软件截图、没有设备的照片都不接受。 |
| 构图 | 横向，约 4:3。卡片会按这个比例裁切，主体放在中间。 |
| 格式 | JPG、PNG 或 WebP。JPG 通常最小。 |
| 大小 | 最大 5 MB。长边 1200–1600 px 足够。 |
| 路径 | 放在 `assets/` 下，与 `preview.image` 完全一致。 |
| 内容 | 不加水印、不叠文字、不出现其他产品。 |

好的做法：自然光、纯色背景、墨水屏亮着且内容可读、一个能看出打印件与设备如何贴合的角度。

## 7. README

README 是给已经决定打印你设计的人看的。把模板替换成下面这些小节：

```markdown
# My Case

One paragraph: what it is, which part of Sticky it protects, holds, or mounts,
and who it is for.

## Print settings

- Material: PETG (firm) or TPU 95A (soft)
- Layer height: 0.2 mm
- Walls: 3
- Infill: 15 %
- Supports: none
- Orientation: back face on the bed

## Assembly

1. Remove any brim.
2. Slide the case on from the USB-C side first.
3. Press the top edge until the lip clicks over the bezel.
4. To remove, push the bottom corner out with a thumb.

## Hardware

None. (Or: 2 × 6 mm neodymium magnets, glued into the recesses.)

## Files

<https://www.printables.com/model/123456-my-case> — same page as `download.url`.

## License

CC BY-SA 4.0
```

保持简短；更长的内容放在你的下载页。README 同样使用英文。

## 8. 分步操作

### 第 1 步 —— 先发布模型

把文件上传到 Printables、MakerWorld、Thingiverse、GrabCAD、GitHub 或你自己的网站，
复制公开页面的地址。地址必须以 `https://` 开头，并且在浏览器隐私窗口里不登录也能看到文件。

### 第 2 步 —— 拍照

打印出来装在你的 Sticky 上，按[第 6 节](#6-预览照片)拍照，保存为 `preview.jpg`。

### 第 3 步 —— 拿到仓库副本

从 [CONTRIBUTING.zh-CN.md → 三种提交方式](../CONTRIBUTING.zh-CN.md#5-三种提交方式) 里选一种：

- **方式 A**（git）：Fork、克隆、新建分支。
- **方式 B**（网页）：Fork 后直接在 GitHub 网页里创建文件。

下面按方式 A 讲；方式 B 需要创建的文件内容完全相同。

用方式 A 时，先把本地 `main` 同步到上游最新，这样设计使用的目录结构与 Registry 当前读取的一致：

```bash
# 每个克隆只需执行一次
git remote add upstream https://github.com/Seeed-Projects/reterminal-sticky-playground-registry.git
git pull --no-ff upstream main
```

### 第 4 步 —— 复制模板

在仓库根目录执行：

```bash
cp -R printables/_template printables/my-case
```

现在你有了 `printables/my-case/printable.json`、`README.md` 和 `assets/README.md`。

### 第 5 步 —— 填写 printable.json

打开 `printables/my-case/printable.json`，按[第 4 节](#4-printablejson-字段说明)替换每一个值。
把 `id` 改成 `my-case`。

### 第 6 步 —— 放入照片

把照片复制到 `printables/my-case/assets/preview.jpg`，删掉 `assets/README.md`
（它只是说明这里该放什么）。确认 `printable.json` 里的 `preview.image` 是 `assets/preview.jpg`。

### 第 7 步 —— 写 README

用[第 7 节](#7-readme)的结构替换 `printables/my-case/README.md`。

### 第 8 步 —— 本地自检

安装 Node.js 20 或更高版本，在仓库根目录执行：

```bash
npm test
npm run validate
```

预期最后一行：

```text
Registry validation passed (12 firmware(s), 6 printable(s)).
```

（数字是仓库里全部目录的数量，加上你的设计后打印件会多 1。）失败请看
[第 9 节](#9-校验报错与修法)。

### 第 9 步 —— 提交并推送

```bash
git add printables/my-case
git commit -m "feat: add My Case printable"
git push -u origin add-my-case
```

### 第 10 步 —— 提交 pull request

在 GitHub 上从你的分支向 `Seeed-Projects/reterminal-sticky-playground-registry` 的
`main` 发起 pull request。在模板里：

- **Contribution type** 勾选 **3D printable design: new design or update**；
- 完成 **Common verification**；
- 完成 **3D printable design** 一节：设计名、目录、下载页、平台、分类和五个勾选项；
- **Firmware** 一节保持不动。

标题建议：`Add <Design Name> printable`。

### 第 11 步 —— 审核

GitHub Actions 会在几分钟内运行 `npm test` 和 `npm run validate`。之后维护者核对署名、
下载页和照片。在 PR 里回复评论，或直接往同一分支推送修改。合并后，卡片会随下一次
Sticky 网站发布上线，见
[CONTRIBUTING.zh-CN.md → PR 合并之后](../CONTRIBUTING.zh-CN.md#9-pr-合并之后)。

一句话总结：发布模型 → 拍照 → 复制模板填三份文件 → 本地校验 → 提 PR。

## 9. 校验报错与修法

每条报错依次是：文件、字段路径、问题。以 `printables/my-case/printable.json` 为例：

| 报错文字 | 原因 | 修法 |
|---|---|---|
| `printables/my-case: is missing printable.json` | 文件没创建，或名字不对 | 创建 `printables/my-case/printable.json` |
| `...printable.json: contains invalid JSON (...)` | 少逗号、多逗号、键没加引号 | 把文件贴进 JSON 检查工具，修正提示的那一行 |
| `...printable.json: missing required field "download"` | 缺少必填的顶层对象 | 按[第 4 节](#4-printablejson-字段说明)补上 |
| `...printable.json: contains unsupported field "mode"` | 出现了字段说明里没有的键 | 删掉它（`mode`、`catalogSection`、`source`、`external` 这类固件字段不属于这里） |
| `...printable.json.id: must match the directory name "my-case"` | `id` 与目录名不一致 | 改成一样 |
| `...printable.json.category: must be one of: case, stand, mount, accessory, reference, other` | 拼错，或填了固件分类 | 用六个值之一 |
| `...printable.json.summary: must contain between 1 and 140 characters` | 太长 | 缩成一句话，细节移到 `description` |
| `...printable.json.download.url: must use HTTPS` | 以 `http://` 开头 | 换成 `https://` 版本 |
| `...printable.json.download.url: must be a valid absolute URL` | 缺协议头或含空格 | 从浏览器地址栏完整复制 |
| `...printable.json.preview.image: must be inside the assets/ directory` | 路径不以 `assets/` 开头 | 移动文件并写成 `assets/preview.jpg` |
| `...printable.json.preview.image: references a missing file: assets/preview.jpg` | 文件名或后缀不一致，或没加进 git | 检查拼写和大小写；执行 `git add printables/my-case/assets` |
| `...printable.json.preview.image: must use one of these file extensions: .png, .jpg, .jpeg, .webp` | HEIC、SVG、GIF 等格式 | 导出为 JPG 或 PNG |
| `...printable.json.preview.image: does not contain a valid JPEG file signature` | 把 PNG（或别的文件）直接改名成 `.jpg` | 用正确格式重新导出，或改回真实后缀并更新 `preview.image` |
| `...printable.json.preview.image: must not exceed 5 MB` | 照片过大 | 缩到长边约 1600 px |
| `...printable.json.tags: must be an array containing no more than 6 tags` | 标签太多 | 保留最有用的 6 个 |
| `...printable.json.tags[2]: duplicates tag "case"` | 同一标签出现两次 | 删掉重复项 |
| `...printable.json.README.md: references a missing file: README.md` | README 被删或没提交 | 加上 `printables/my-case/README.md` |

## 10. 更新已有设计

修改你目录下的文件，再提一个新的 pull request（CONTRIBUTING 里的方式 A、B 或 C 均可）。
模板里勾选 **3D printable design: new design or update**，并说明改了什么。常见更新：

| 变化 | 改哪里 |
|---|---|
| 换到别的平台 | `download.platform`、`download.url`、README 里的链接 |
| 更好的照片 | 替换 `assets/preview.jpg`；视角变了就更新 `preview.alt` |
| 模型出了新版本 | `description`（提一下版本）、README 的打印参数 |
| 更换许可证 | `download.license`、README |
| 重新分类 | `category` |

目录名和 `id` 保持不变，卡片才能保留身份。

## 11. 真实示例

Dropthetenors 的 Sticky Wallet Case 发布在 Thingiverse。它在本仓库里的卡片是：

`printables/sticky-wallet-case/printable.json`

```json
{
  "schemaVersion": 1,
  "id": "sticky-wallet-case",
  "name": "Sticky Wallet Case",
  "category": "case",
  "summary": "A slim printed wallet-style case designed around the Sticky card form factor.",
  "description": "A slim wallet-style case that wraps around reTerminal Sticky so it can be carried like a card. Files are published on Thingiverse.",
  "author": {
    "name": "Dropthetenors",
    "url": "https://www.thingiverse.com/thing:7396154"
  },
  "download": {
    "platform": "Thingiverse",
    "url": "https://www.thingiverse.com/thing:7396154/files"
  },
  "preview": {
    "image": "assets/preview.jpg",
    "alt": "Sticky Wallet Case printed and fitted on reTerminal Sticky"
  },
  "tags": [
    "case"
  ]
}
```

在网站上，它显示为 **Cases** 分类下一张标题为 "Sticky Wallet Case" 的卡片，作者
"Dropthetenors" 链接到 Thingiverse 页面，按钮 **View on Thingiverse** 打开文件标签页。
`printables/` 下的其他目录都是真实示例，可以随时参考。

## 12. 提交前清单

- [ ] 目录名与 `printable.json` 的 `id` 完全一致，只含小写字母、数字和连字符。
- [ ] `category` 是 `case`、`stand`、`mount`、`accessory`、`reference`、`other` 之一。
- [ ] `summary` 是一句话（≤ 140 字符）；填了 `description` 的话是两三句话（≤ 800）。
- [ ] `author.name` 写明设计者；`author.url`（如有）是 HTTPS。
- [ ] `download.url` 是公开 HTTPS 页面；填了 `download.platform` 的话是托管网站名；下载页标注了许可证时 `download.license` 与之一致。
- [ ] `assets/preview.jpg` 是打印成品装在 reTerminal Sticky 上的真实照片，≤ 5 MB，与 `preview.image` 完全一致，并有描述性的 `preview.alt`。
- [ ] `README.md` 写明打印参数、组装步骤、五金件、文件链接和许可证。
- [ ] 没有提交任何模型文件。
- [ ] `npm test` 和 `npm run validate` 通过。
- [ ] PR 模板勾选了 **3D printable design** 并完成了对应小节。
