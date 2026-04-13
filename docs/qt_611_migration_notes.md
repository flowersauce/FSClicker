# Qt 6.11 Migration Notes

## Current Status

- Project source layout has been reorganized:
  - `src/` for implementation files.
  - `include/` for headers.
  - `ui/` for Qt Designer files.
  - `resources/` for qrc/rc and runtime assets.
  - `scripts/` for helper scripts.
- Build target has been renamed to `FSClicker`.
- User-facing app name is `FSClicker`.
- Project version is defined in `CMakeLists.txt` as `FS_CLICKER_VERSION`.
- `MainWindow` has been partially split into smaller initialization and injection-state methods.
- Global keyboard hook logic has been moved into `KeyboardHook`.
- Packaging script is now argument-based: `scripts/package_app.py`.

## Qt 6.11 Dynamic Build Verified

The project has been verified with the moved Qt 6.11 dynamic installation:

```powershell
D:\APP\DevEnv\Qt\Tools\CMake_64\bin\cmake.exe -S . -B build-qt611 -G Ninja `
  -DCMAKE_CXX_COMPILER=D:/APP/DevEnv/Qt/Tools/mingw1310_64/bin/g++.exe `
  -DCMAKE_MAKE_PROGRAM=D:/APP/DevEnv/Qt/Tools/Ninja/ninja.exe `
  -DCMAKE_PREFIX_PATH=D:/APP/DevEnv/Qt/6.11.0/mingw_64

D:\APP\DevEnv\Qt\Tools\CMake_64\bin\cmake.exe --build build-qt611
```

Result:

```text
Linking CXX executable FSClicker.exe
```

## Important Path Notes

During this session, the current shell still had old Qt paths in `PATH`, including:

```text
D:\Qt\6.7.2\mingw_64\bin
D:\Qt\Tools\mingw1120_64\bin
```

After reboot, verify the effective paths again. Prefer the new Qt 6.11 paths:

```text
D:\APP\DevEnv\Qt\6.11.0\mingw_64\bin
D:\APP\DevEnv\Qt\Tools\mingw1310_64\bin
D:\APP\DevEnv\Qt\Tools\CMake_64\bin
D:\APP\DevEnv\Qt\Tools\Ninja
```

Useful checks:

```powershell
where.exe cmake ninja g++ qmake
cmake --version
ninja --version
g++ --version
```

## Static Qt Build Plan

Qt Installer did not provide a ready-made static Qt kit. Static Qt should be built from source.

Use this after reboot, once environment variables are clean:

```powershell
$env:Path = "D:\APP\DevEnv\Qt\Tools\mingw1310_64\bin;D:\APP\DevEnv\Qt\Tools\CMake_64\bin;D:\APP\DevEnv\Qt\Tools\Ninja;" + $env:Path

New-Item -ItemType Directory -Force D:\APP\DevEnv\Qt\Build\qt-6.11.0-static | Out-Null
Set-Location D:\APP\DevEnv\Qt\Build\qt-6.11.0-static

D:\APP\DevEnv\Qt\6.11.0\Src\configure.bat `
  -prefix D:\APP\DevEnv\Qt\6.11.0\mingw_64_static `
  -static `
  -release `
  -opensource `
  -confirm-license `
  -nomake examples `
  -nomake tests `
  -skip qtwebengine
```

If configure succeeds:

```powershell
D:\APP\DevEnv\Qt\Tools\CMake_64\bin\cmake.exe --build . --parallel
D:\APP\DevEnv\Qt\Tools\CMake_64\bin\cmake.exe --install .
```

Then build this project against static Qt:

```powershell
D:\APP\DevEnv\Qt\Tools\CMake_64\bin\cmake.exe -S . -B build-qt611-static -G Ninja `
  -DCMAKE_CXX_COMPILER=D:/APP/DevEnv/Qt/Tools/mingw1310_64/bin/g++.exe `
  -DCMAKE_MAKE_PROGRAM=D:/APP/DevEnv/Qt/Tools/Ninja/ninja.exe `
  -DCMAKE_PREFIX_PATH=D:/APP/DevEnv/Qt/6.11.0/mingw_64_static `
  -DFS_CLICKER_STATIC_QT=ON `
  -DFS_CLICKER_STATIC_RUNTIME=ON

D:\APP\DevEnv\Qt\Tools\CMake_64\bin\cmake.exe --build build-qt611-static
```

## CMake Static Options

The project defines:

```cmake
option(FS_CLICKER_STATIC_RUNTIME "Link MinGW runtime libraries statically." ON)
option(FS_CLICKER_STATIC_QT "Require a static Qt build and link Qt libraries statically." OFF)
```

`FS_CLICKER_STATIC_QT=ON` intentionally fails when `CMAKE_PREFIX_PATH` points to a shared Qt installation. This prevents accidentally believing Qt was statically linked when it was not.

## Packaging Notes

Dynamic Qt packaging command example:

```powershell
python scripts/package_app.py `
  --build-dir build-qt611 `
  --qt-bin D:/APP/DevEnv/Qt/6.11.0/mingw_64/bin `
  --mingw-bin D:/APP/DevEnv/Qt/Tools/mingw1310_64/bin
```

`windeployqt` deploys Qt libraries and plugins, but may not copy MinGW runtime DLLs. The packaging script still handles:

```text
libstdc++-6.dll
libgcc_s_seh-1.dll
libwinpthread-1.dll
```

When using a true static Qt build with static runtime linkage, this packaging step should become much simpler, but it should still be verified on a clean machine.

## Next Steps After Reboot

1. Verify `PATH` no longer points to old `D:\Qt\6.7.2` or `mingw1120` paths.
2. Re-run the Qt 6.11 dynamic build if needed.
3. Start the static Qt configure step.
4. If configure fails, save the full output and adjust skipped modules or dependencies.
