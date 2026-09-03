# Example Printable Design

Copy this directory to `printables/<design-id>/`, then provide:

1. Complete metadata in `printable.json`.
2. A real photo of the printed design on reTerminal Sticky as `assets/preview.jpg`.
3. The HTTPS page where people download the files (Printables, MakerWorld,
   Thingiverse, GrabCAD, GitHub, or your own site).
4. Print settings and assembly notes in this README, using the outline below.

The Registry stores this card and the preview photo. The model files stay on
your download page.

Run `npm test` and `npm run validate` from the repository root before opening a
pull request.

- [English guide: contributing a 3D printable design](../../docs/contributing-printables.md)
- [中文指南：贡献 3D 打印设计](../../docs/contributing-printables.zh-CN.md)

---

## Suggested README outline for your design

Replace this file with the sections below.

### What it is

One paragraph: what the design does and which part of Sticky it protects,
holds, or mounts.

### Print settings

- Material: PLA / PETG / TPU
- Layer height: 0.2 mm
- Supports: none / where
- Orientation: which face on the bed

### Assembly

Numbered steps for fitting the print to Sticky and removing it.

### Files

Link to the same download page used in `printable.json`.
