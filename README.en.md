<img src="assets/readme_header.svg" alt="FSClicker" width="100%">

<p align="center">
  <img alt="Windows" src="https://img.shields.io/badge/Windows-0078D4?style=flat-square&logo=windows&logoColor=white">
  <img alt="Qt 6.11" src="https://img.shields.io/badge/Qt-6.11-41CD52?style=flat-square&logo=qt&logoColor=white">
  <img alt="C++23" src="https://img.shields.io/badge/C%2B%2B-23-00599C?style=flat-square&logo=cplusplus&logoColor=white">
  <img alt="Vibe Coding" src="https://img.shields.io/badge/Vibe%20Coding-FF6B9A?style=flat-square">
  <img alt="License" src="https://img.shields.io/badge/License-MIT-2DA44E?style=flat-square">
</p>

<p align="center">
  <a href="README.md">中文</a> | English
</p>

<p align="center">
  <a href="#features">Features</a> ·
  <a href="#download">Download</a> ·
  <a href="#build">Build</a> ·
  <a href="#package">Package</a> ·
  <a href="#license">License</a>
</p>

---

## Features

- Mouse and custom key input.
- Repeat and hold input modes.
- Locked-position clicking with screen coordinate capture.
- Period-based input with `0% - 20%` dynamic jitter.

## Download

FSClicker is available from Windows Package Manager:

```powershell
winget install Flowersauce.FSClicker
```

You can also download the latest Windows packages from GitHub Releases:

- Installer: download `FSClicker-v<version>-windows-x64-setup.exe`, then run it to install automatically with a progress window.
- Portable: download `FSClicker-v<version>-windows-x64-portable.zip`, extract it, and run `FSClicker.exe`.

## Build

The app is built with Qt Quick.

Requirements:

- Windows
- CMake 3.30+
- Qt 6.11+
- MinGW
- Python 3 for release packaging

Configure:

```powershell
cmake -S . -B build -G Ninja `
  -DCMAKE_PREFIX_PATH="$env:QT_ROOT"
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
output/release/FSClicker-v<version>-windows-x64-portable.zip
output/release/FSClicker-v<version>-windows-x64-portable.zip.sha256
```

Portable packages and local development runs store configuration at `config/config.json` inside the app directory. Velopack builds store configuration in `config/config.json` under the installation root, so it survives updates and is removed with the installation.

By default, the package includes the required MinGW/compiler runtime DLLs, and excludes Qt translations and the
software OpenGL fallback library. Use this when you want the OpenGL fallback:

```powershell
python scripts/package_app.py --keep-opengl-sw
```

To generate the Velopack installer package at the same time, install the Velopack CLI first, then run:

```powershell
python scripts/package_app.py --with-velopack
```

This also creates:

```text
output/release/FSClicker-v<version>-windows-x64-setup.exe
output/release/FSClicker-v<version>-windows-x64-setup.exe.sha256
```

When running the script without arguments, you can also choose whether to generate the installer in the prompt.

Final GitHub Release artifacts are written to:

```text
output/release/
```

## License

Copyright © 2024 Flowersauce
