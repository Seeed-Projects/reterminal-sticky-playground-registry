# 贡献 3D 打印设计

本指南面向 Sticky Playground **3D Printables** 页面的外壳、支架、安装件和配件贡献，
写给在 Printables、MakerWorld、Thingiverse、GrabCAD 或 GitHub 上发布模型的创客。
你不需要了解任何固件知识。

英文指南请查看 [contributing-printables.md](contributing-printables.md)。
两类贡献共用的审核与发布流程见 [CONTRIBUTING.zh-CN.md](../CONTRIBUTING.zh-CN.md)。

## 你需要提交什么

你提交的是一张「卡片」，不是模型文件：

- 一个很小的 `printable.json`，写卡片上显示的文字；
- 一张打印成品装在真机 reTerminal Sticky 上的照片；
- 一份简短的 README，写打印参数和组装说明。

模型文件继续留在你自己的下载页。Sticky 网站展示你的卡片，并通过 **View on …**
按钮把访客送到你的页面。

```text
printables/
  my-case/
    printable.json
    README.md
    assets/
      preview.jpg
```

一句话总结：你交一张名片和一张照片，文件放在你自己那里。

## 访客会看到什么

1. 访客打开 Sticky Playground，切到 **3D Printables**。
2. 访客按分类筛选，点开你的卡片。
3. 访客点击 **View on Printables**（或你填写的平台名）。
4. 访客在你的页面下载文件。

## 分步操作

### 1. 先发布模型

把文件上传到 Printables、MakerWorld、Thingiverse、GrabCAD、GitHub 或你自己的网站，
记下公开页面的地址；地址必须以 `https://` 开头。

### 2. 复制模板

在仓库根目录执行：

```bash
cp -R printables/_template printables/my-case
```

目录名用小写英文加连字符，例如 `sticky-desk-stand`、`wallet-case`。目录名必须和
`printable.json` 里的 `id` 完全一致。

### 3. 填写 printable.json

```json
{
  "schemaVersion": 1,
  "id": "my-case",
  "name": "My Case",
  "category": "case",
  "summary": "A slim snap-on case that keeps the USB-C port open.",
  "description": "My Case wraps the edges of reTerminal Sticky and leaves the screen, side button, and USB-C port uncovered. Print it in PETG for a firm fit or TPU for a softer bumper.",
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
  "tags": ["snap-on", "usb-c"]
}
```

| 字段 | 填什么 |
|---|---|
| `id` | 与目录名相同 |
| `name` | 卡片标题 |
| `category` | 下方五个分类之一 |
| `summary` | 标题下方的一句话（不超过 140 字符） |
| `description` | 两三句话，说明贴合度、材料或使用注意（不超过 800 字符） |
| `author.name` | 你的名字或团队名，显示在卡片上 |
| `author.url` | 可选。你的主页，点击名字时打开 |
| `download.platform` | 托管文件的网站名，例如 `Printables`、`MakerWorld`、`Thingiverse`、`GrabCAD`、`GitHub` |
| `download.url` | 下载文件的公开页面 |
| `download.license` | 可选。该页面标注的许可证，例如 `CC BY-SA 4.0` |
| `preview.image` | 照片在 `assets/` 内的路径，例如 `assets/preview.jpg` |
| `preview.alt` | 一句话描述照片内容，供读屏软件使用 |
| `tags` | 可选。最多 6 个短标签，显示在分类旁边 |

卡片上的文字面向全球访客，请使用英文填写。

### 4. 选择分类

| 分类 | 适用范围 | 例子 |
|---|---|---|
| `case` | 包裹设备的外壳和保护套 | 钱包式外壳、卡扣式保护套 |
| `stand` | 让 Sticky 立在桌面或斜放的支架 | 桌面支架、充电支架 |
| `mount` | 把 Sticky 固定到其他物体上的部件 | 冰箱磁吸座、墙面支架、车把夹 |
| `accessory` | 以上都不属于的附加件 | 触控笔座、线夹、挂绳环 |
| `reference` | Sticky 本体的模型，用于设计其他部件 | 外壳 CAD 模型、尺寸模板 |

分类决定你的卡片出现在网站左侧哪个筛选项下。

### 5. 放入照片

把打印成品装在 reTerminal Sticky 上的真实照片保存为 `assets/preview.jpg`
（PNG、WebP 也可以）。横向、约 4:3 的构图在卡片上效果最好。文件不超过 5 MB。

### 6. 写 README

用下面的内容替换模板 README：

- 这个设计是什么，保护、支撑或固定 Sticky 的哪个部位；
- 打印参数：材料、层高、支撑、摆放方向；
- 组装步骤，以及如何再取下来；
- 与 `printable.json` 相同的下载页链接。

### 7. 本地自检

安装 Node.js 20 或更高版本，在仓库根目录执行：

```bash
npm test
npm run validate
```

`npm run validate` 会确认目录名与 `id` 一致、必填字段齐全、下载地址使用 HTTPS、
分类在五个值之内、照片存在且格式正确。全部通过时会打印
`Registry validation passed`。

### 8. 提交 pull request

在 PR 模板中勾选 **3D printable design**，完成对应的简短清单。维护者核对署名、
下载页和照片后合并。卡片会随下一次网站发布出现在 Sticky 官网，
见 [CONTRIBUTING.zh-CN.md](../CONTRIBUTING.zh-CN.md#pr-合并之后)。

## 更新已有设计

直接修改你目录下的文件，再提交一个新的 pull request。常见更新：

- 换了平台后的新下载地址；
- 更好的照片；
- 调整分类或描述。

## 提交前清单

- [ ] 目录名与 `printable.json` 的 `id` 完全一致。
- [ ] `category` 是 `case`、`stand`、`mount`、`accessory`、`reference` 之一。
- [ ] `download.url` 是托管文件的公开 HTTPS 页面。
- [ ] `author.name` 写明设计者；下载页标注了许可证时，`download.license` 与之一致。
- [ ] `assets/preview.jpg` 是打印成品装在 reTerminal Sticky 上的真实照片。
- [ ] README 写明打印参数和组装说明。
- [ ] `npm test` 和 `npm run validate` 通过。
