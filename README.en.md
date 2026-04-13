<p align="center">
  <a href="README.md">中文</a> | English
</p>

# FSClicker

<p>
  <img alt="Windows" src="https://img.shields.io/badge/Windows-0078D4?style=flat-square&logo=windows&logoColor=white">
  <img alt="Qt 6.11" src="https://img.shields.io/badge/Qt-6.11-41CD52?style=flat-square&logo=qt&logoColor=white">
  <img alt="C++23" src="https://img.shields.io/badge/C%2B%2B-23-00599C?style=flat-square&logo=cplusplus&logoColor=white">
  <img alt="Vibe Coding" src="https://img.shields.io/badge/Vibe%20Coding-FF6B9A?style=flat-square">
  <img alt="License" src="https://img.shields.io/badge/License-MIT-2DA44E?style=flat-square">
</p>

FSClicker is a lightweight auto clicker for Windows, built with Qt Quick.

## Features

- Repeated left, middle, and right mouse clicks.
- Custom keyboard key and mouse wheel up/down input.
- Repeat and hold input modes.
- Locked-position clicking with screen coordinate capture.
- Period-based input with `0% - 20%` dynamic jitter.
- Chinese and English UI.
- Auto, dark, and light themes.
- Settings are saved when the app exits.

## Download

Download the latest Windows package from GitHub Releases, extract it, and run `FSClicker.exe`.

## Build

Requirements:

- Windows
- CMake 3.30+
- Qt 6.11+
- MinGW
- Python 3 for release packaging

Configure:

```powershell
cmake -S . -B build -G Ninja `
  -DCMAKE_PREFIX_PATH="$env:QT_ROOT" `
  -DFS_CLICKER_STATIC_RUNTIME=ON
```

Build:

```powershell
cmake --build build --target FSClicker
```

## Package

Build the app first, then run:

```powershell
python scripts/package_app.py
```

The script searches build directories whose names contain `build`, uses the newest `FSClicker.exe`, and creates:

```text
output/release/FSClicker-v<version>-windows-x64.zip
output/release/FSClicker-v<version>-windows-x64.zip.sha256
```

By default, the package excludes Qt translations, compiler runtime DLLs, and the software OpenGL fallback library. Use
this when you want the OpenGL fallback:

```powershell
python scripts/package_app.py --keep-opengl-sw
```

## License

This project is licensed under the MIT License.
