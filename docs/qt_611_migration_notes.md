# Qt 6.11 迁移记录

## 当前状态

- 项目源代码目录已经整理为 `src/`、`include/`、`resources/`、`qml/`、`scripts/`。
- 构建目标名和应用名统一为 `FSClicker`。
- 项目版本在 `CMakeLists.txt` 的 `FS_CLICKER_VERSION` 中维护。
- 当前默认使用 Qt 6.11 动态库构建。
- MinGW 运行时默认静态链接，见 `FS_CLICKER_STATIC_RUNTIME`。
- 发布包由 `scripts/package_app.py` 基于已编译好的构建目录生成。

## Qt 6.11 动态构建

示例配置命令：

```powershell
$env:QT_ROOT = "<Qt mingw_64 目录>"
$env:MINGW_ROOT = "<MinGW 目录>"

cmake -S . -B build-qt611 -G Ninja `
  -DCMAKE_CXX_COMPILER="$env:MINGW_ROOT/bin/g++.exe" `
  -DCMAKE_PREFIX_PATH="$env:QT_ROOT"
```

构建命令：

```powershell
cmake --build build-qt611
```

## 路径检查

如果构建异常，先确认命令实际指向的工具链：

```powershell
where.exe cmake ninja g++ qmake
cmake --version
ninja --version
g++ --version
```

优先使用 Qt 6.11 对应路径，可通过环境变量维护：

```text
QT_ROOT=<Qt mingw_64 目录>
MINGW_ROOT=<MinGW 目录>
```

## 发布打包

打包脚本不负责编译，只搜索名称包含 `build` 的目录，并使用其中最新的 `FSClicker.exe`。

默认命令：

```powershell
python scripts/package_app.py
```

常用参数：

```powershell
python scripts/package_app.py --build-dir build-release
python scripts/package_app.py --keep-opengl-sw
python scripts/package_app.py --keep-translations
```

默认发布包会排除 Qt 翻译文件、编译器运行时 DLL 和软件 OpenGL 兜底库。`--keep-opengl-sw` 可用于保留 `opengl32sw.dll`
，提升少数显卡驱动异常环境下的兼容性。

## 静态 Qt

当前项目不再保留静态 Qt 专用 CMake 选项。后续如果需要彻底减少动态 DLL，需要单独准备静态 Qt 工具链，并把它作为独立发布方案处理。
