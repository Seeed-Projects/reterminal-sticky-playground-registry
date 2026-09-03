# 为 reTerminal Sticky Playground 贡献内容

这个仓库是 reTerminal Sticky Playground 面向外部开发者和创客的公开贡献与审核入口。
它接受两类贡献，各有独立的目录、元数据文件和指南。

英文版请查看 [CONTRIBUTING.md](CONTRIBUTING.md)。

## 选择你的指南

| 我想分享… | 目录 | 元数据文件 | 指南 |
|---|---|---|---|
| 用户可以在 Sticky 网站直接烧录的固件 | `firmwares/<firmware-id>/` | `firmware.json` | [贡献固件](docs/contributing-firmware.zh-CN.md) |
| 3D 打印的外壳、支架、安装件或配件 | `printables/<design-id>/` | `printable.json` | [贡献 3D 打印设计](docs/contributing-printables.zh-CN.md) |

固件贡献包含可编译源码或经过验证的固件包，审核前需要在真机上测试。3D 打印贡献
只是一张带预览照片的卡片；模型文件留在作者自己的下载页。

一句话总结：做固件看固件指南，做外壳看打印指南，两条路互不干扰。

## 仓库结构

```text
firmwares/
  _template/                 新固件从这里复制
  <firmware-id>/
    firmware.json
    README.md
    assets/
    source/                  源码模式
    firmware/<version>/      仅固件包模式
printables/
  _template/                 新打印设计从这里复制
  <design-id>/
    printable.json
    README.md
    assets/preview.jpg
schemas/
  firmware.schema.json
  printable.schema.json
scripts/
  validate-registry.mjs      同时检查两个目录
```

## 共同规则

- 每份贡献一个目录。目录名与元数据文件里的 `id` 使用同一个小写连字符标识。
- 所有链接使用 HTTPS。
- 图片放在 `assets/` 下，使用 PNG、JPG、WebP，固件条目还可以使用静态 SVG。
- 提交的文件中不包含个人 Wi-Fi 凭据、API key、token 或密码。
- 卡片文字使用英文，因为 Sticky 网站面向全球访客。

## 本地自检

安装 Node.js 20 或更高版本，在仓库根目录执行：

```bash
npm test
npm run validate
```

校验脚本会检查全部固件和打印设计目录，仓库一致时打印
`Registry validation passed (N firmware(s), M printable(s)).`。
GitHub Actions 会对每个 pull request 运行同样的命令。

## PR 审核

1. 提交 pull request，并在模板中勾选贡献类型。
2. GitHub Actions 校验 Registry；对源码模式固件，还会编译项目并把固件作为 PR 产物附上。
3. 维护者审核元数据、图片、链接、许可证，固件贡献还会核对真机测试记录。
4. 检查通过且审核完成后合并。

## PR 合并之后

合并 Registry 的 pull request 不会直接发布到正式网站。Sticky 网站锁定一个经过审核
的 Registry 提交，维护者按下面的顺序发布：

1. 合并 Registry pull request。
2. 源码模式固件需等待 Registry `main` 的工作流发布固件 Release。
3. 在 Sticky 网站仓库更新锁定的 Registry 提交。
4. 本地构建网站，确认新卡片（固件还包括烧录页）正确生成。
5. 固件从本地页面烧录到真机验收。
6. 合并到 Sticky 网站 `main` 并部署。

第 6 步完成后，新卡片出现在 <https://www.seeedstudio.com/sticky/playground/>。

一句话总结：合并公开 PR 是进入待发布队列，网站更新锁定版本后才正式上线。

## 更新已有贡献

直接修改你目录下的文件，再提交新的 pull request。固件更新把新版本放在
`flash.versions` 最前面，详见固件指南；打印设计更新可以改卡片文字、照片、分类或下载页。
