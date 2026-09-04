"""Records the flash layout of a PlatformIO build for the Registry packaging step.

Writes ``$BUILD_DIR/flasher_args.json`` with every flash offset and the file that
belongs at it, using the same shape ESP-IDF produces. Paths are stored relative to
the build directory so files that live inside framework packages stay reachable.

把 PlatformIO 本次编译的烧录布局写入 ``$BUILD_DIR/flasher_args.json``,
格式与 ESP-IDF 生成的同名文件一致。路径以构建目录为基准保存,
因此位于框架包内的文件同样可以定位。

Usage / 用法::

    PLATFORMIO_EXTRA_SCRIPTS=post:/path/to/pio-dump-flash-args.py pio run -e <env>
"""

import json
import os

Import("env")  # noqa: F821

board = env.BoardConfig()  # noqa: F821
build_dir = env.subst("$BUILD_DIR")  # noqa: F821

images = [
    (offset, env.subst(path))  # noqa: F821
    for offset, path in env.get("FLASH_EXTRA_IMAGES", [])  # noqa: F821
]
images.append((
    env.subst("$ESP32_APP_OFFSET"),  # noqa: F821
    env.subst(os.path.join("$BUILD_DIR", "${PROGNAME}.bin")),  # noqa: F821
))

flash_files = {}
for offset, path in images:
    relative_path = os.path.relpath(path, build_dir).replace(os.sep, "/")
    flash_files[str(offset)] = relative_path

os.makedirs(build_dir, exist_ok=True)
with open(os.path.join(build_dir, "flasher_args.json"), "w", encoding="utf8") as handle:
    json.dump(
        {
            "flash_files": flash_files,
            "flash_settings": {
                "flash_size": board.get("upload.flash_size", "4MB"),
            },
        },
        handle,
        indent=2,
    )
    handle.write("\n")

print("Recorded %d flash file(s) in %s/flasher_args.json" % (len(flash_files), build_dir))
