#!/usr/bin/env python3
"""
FSClicker 发布包生成脚本。

默认从项目根目录运行：
    python scripts/package_app.py

脚本会搜索项目目录下名称包含 build 的目录，使用已经编译好的 exe，
部署 Qt 运行依赖，生成可直接上传到 GitHub Release 的便携 zip 和 Velopack 安装器。
"""

import argparse
import hashlib
import os
import shutil
import subprocess
import sys
import zipfile
from pathlib import Path

APP_NAME = "FSClicker"
APP_AUTHOR = "Flowersauce"
WINDOWS_X64_SUFFIX = "windows-x64"
RUNTIME_DLLS = ("libstdc++-6.dll", "libgcc_s_seh-1.dll", "libwinpthread-1.dll", "libatomic-1.dll")
PORTABLE_MARKER = f"{APP_NAME}.portable"
VELOPACK_PACK_ID = f"{APP_AUTHOR}.{APP_NAME}"


def project_root() -> Path:
    return Path(__file__).resolve().parents[1]


def parse_args():
    root = project_root()
    parser = argparse.ArgumentParser(description="生成 FSClicker 的 Windows GitHub Release 发布包。")
    parser.add_argument("--build-dir", default=None, help="指定 CMake 构建目录；不传时自动搜索名称包含 build 的目录。")
    parser.add_argument("--output-dir", default=str(root / "output" / "release"), help="最终发布产物输出目录。")
    parser.add_argument("--work-dir", default=str(root / "output" / "_work"), help="打包临时工作目录。")
    parser.add_argument("--qt-bin", default=os.environ.get("QT_BIN_PATH"), help="包含 windeployqt.exe 的 Qt bin 目录。")
    parser.add_argument("--mingw-bin", default=os.environ.get("MINGW_BIN_PATH"),
                        help="包含 MinGW 运行时 DLL 的 bin 目录。")
    parser.add_argument("--keep-translations", action="store_true", help="保留 Qt 翻译文件。")
    parser.add_argument("--keep-compiler-runtime", action="store_true",
                        help="兼容旧参数；现在默认保留编译器运行时 DLL。")
    parser.add_argument("--no-compiler-runtime", action="store_true", help="不复制编译器运行时 DLL。")
    parser.add_argument("--keep-opengl-sw", action="store_true", help="保留 Qt 软件 OpenGL 兜底库。")
    parser.add_argument("--no-portable-marker", action="store_true", help="不写入便携模式标记文件。")
    parser.add_argument("--skip-zip", action="store_true", help="只生成目录，不生成 zip。")
    parser.add_argument("--name-suffix", default=WINDOWS_X64_SUFFIX, help="zip 文件名中的平台后缀。")
    parser.add_argument("--with-velopack", action="store_true", help="同时生成 Velopack 安装版。")
    parser.add_argument("--keep-velopack-feed", action="store_true",
                        help="保留 Velopack 生成的 nupkg、RELEASES 和 releases.win.json 等更新源文件。")
    parser.add_argument("--velopack-output-dir", default=None,
                        help="Velopack 更新源文件输出目录；默认保存在临时工作目录，除非传入 --keep-velopack-feed。")
    parser.add_argument("--vpk", default=os.environ.get("VPK_PATH", "vpk"), help="vpk 可执行文件路径。")
    parser.add_argument("--pack-id", default=VELOPACK_PACK_ID, help="Velopack 应用唯一 ID。")
    parser.add_argument("--pack-title", default=APP_NAME, help="Velopack 应用显示名称。")
    parser.add_argument("--pack-authors", default=APP_AUTHOR, help="Velopack 作者名称。")
    return parser.parse_args()


def confirm(prompt: str, default: bool = False) -> bool:
    default_text = "Y/n" if default else "y/N"
    answer = input(f"{prompt} [{default_text}]: ").strip().lower()
    if not answer:
        return default
    return answer in ("y", "yes")


def apply_interactive_defaults(args):
    if len(sys.argv) > 1:
        return

    print("FSClicker 发布打包")
    print("默认生成便携版 zip。")
    args.with_velopack = confirm("同时生成 Velopack 安装版吗？", default=False)
    if args.with_velopack and not shutil.which(args.vpk) and not Path(args.vpk).exists():
        explicit_path = input("未在 PATH 找到 vpk，请输入 vpk.exe 路径，或留空稍后报错: ").strip().strip('"')
        if explicit_path:
            args.vpk = explicit_path


def read_cmake_cache(build_dir: Path) -> dict[str, str]:
    cache_path = build_dir / "CMakeCache.txt"
    if not cache_path.exists():
        return {}

    values: dict[str, str] = {}
    for line in cache_path.read_text(encoding="utf-8", errors="ignore").splitlines():
        if not line or line.startswith("//") or line.startswith("#") or "=" not in line:
            continue
        key_type, value = line.split("=", 1)
        key = key_type.split(":", 1)[0]
        values[key] = value
    return values


def clean_directory(path: Path):
    path.mkdir(parents=True, exist_ok=True)
    for child in path.iterdir():
        if child.is_dir():
            shutil.rmtree(child)
        else:
            child.unlink()


def remove_directory(path: Path):
    if path.exists():
        shutil.rmtree(path)


def is_relative_to(path: Path, parent: Path) -> bool:
    try:
        path.relative_to(parent)
        return True
    except ValueError:
        return False


def infer_qt_bin(cache: dict[str, str], explicit_path: str | None) -> Path | None:
    if explicit_path:
        path = Path(explicit_path)
        if (path / "windeployqt.exe").exists():
            return path

    qt_dir = cache.get("Qt6_DIR")
    if not qt_dir:
        return None

    qt_dir_path = Path(qt_dir)
    try:
        qt_bin = qt_dir_path.parents[2] / "bin"
    except IndexError:
        return None

    return qt_bin if (qt_bin / "windeployqt.exe").exists() else None


def infer_mingw_bin(cache: dict[str, str], explicit_path: str | None) -> Path | None:
    if explicit_path:
        path = Path(explicit_path)
        if path.exists():
            return path

    compiler = cache.get("CMAKE_CXX_COMPILER")
    if compiler:
        compiler_bin = Path(compiler).parent
        if compiler_bin.exists():
            return compiler_bin

    return None


def find_on_path(name: str) -> Path | None:
    found = shutil.which(name)
    return Path(found) if found else None


def infer_objdump(cache: dict[str, str], mingw_bin: Path | None) -> Path | None:
    candidates: list[Path] = []
    if mingw_bin:
        candidates.append(mingw_bin / "objdump.exe")

    compiler = cache.get("CMAKE_CXX_COMPILER")
    if compiler:
        candidates.append(Path(compiler).with_name("objdump.exe"))

    path_objdump = find_on_path("objdump")
    if path_objdump:
        candidates.append(path_objdump)

    for candidate in candidates:
        if candidate.exists():
            return candidate
    return None


def find_exe(build_dir: Path) -> Path:
    direct_path = build_dir / f"{APP_NAME}.exe"
    if direct_path.exists():
        return direct_path

    candidates = sorted(build_dir.rglob(f"{APP_NAME}.exe"), key=lambda path: path.stat().st_mtime, reverse=True)
    if candidates:
        return candidates[0]

    raise FileNotFoundError(f"找不到可执行文件: {APP_NAME}.exe")


def find_build_dir(root: Path, explicit_path: str | None) -> Path:
    if explicit_path:
        build_dir = Path(explicit_path).resolve()
        if not build_dir.exists():
            raise FileNotFoundError(f"找不到指定构建目录: {build_dir}")
        find_exe(build_dir)
        return build_dir

    build_dirs = [path for path in root.iterdir() if path.is_dir() and "build" in path.name.lower()]
    candidates: list[tuple[float, Path]] = []
    for build_dir in build_dirs:
        try:
            exe_path = find_exe(build_dir)
        except FileNotFoundError:
            continue
        candidates.append((exe_path.stat().st_mtime, build_dir))

    if not candidates:
        raise FileNotFoundError("没有在名称包含 build 的目录中找到 FSClicker.exe，请先完成编译。")

    return max(candidates, key=lambda item: item[0])[1].resolve()


def imported_dlls(objdump: Path, binary_path: Path) -> set[str]:
    result = subprocess.run([str(objdump), "-p", str(binary_path)], check=False,
                            stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True)
    if result.returncode != 0:
        return set()

    dlls: set[str] = set()
    marker = "DLL Name:"
    for line in result.stdout.splitlines():
        if marker in line:
            dlls.add(line.split(marker, 1)[1].strip())
    return dlls


def discover_missing_imports(package_dir: Path, objdump: Path | None) -> set[str]:
    if not objdump:
        print("跳过导入表扫描: 找不到 objdump.exe。")
        return set()

    binaries = [path for path in package_dir.rglob("*") if path.suffix.lower() in (".exe", ".dll")]
    present = {path.name.lower() for path in binaries}
    missing: set[str] = set()
    for binary in binaries:
        for dll in imported_dlls(objdump, binary):
            if dll.lower() not in present:
                missing.add(dll)
    return missing


def copy_runtime_dlls(search_dirs: list[Path], output_dir: Path, missing_imports: set[str]):
    if not search_dirs:
        print("跳过 MinGW 运行时复制: 找不到 MinGW bin 目录。")
        return

    required_dlls = {dll.lower(): dll for dll in RUNTIME_DLLS}
    for dll in missing_imports:
        if dll.lower().startswith("lib"):
            required_dlls[dll.lower()] = dll

    for dll in required_dlls.values():
        destination = output_dir / dll
        if destination.exists():
            continue

        copied = False
        for search_dir in search_dirs:
            dll_path = search_dir / dll
            if dll_path.exists():
                shutil.copy2(dll_path, destination)
                print(f"已复制运行时库文件: {dll}")
                copied = True
                break

        if not copied and dll.lower() in {runtime.lower() for runtime in RUNTIME_DLLS}:
            print(f"提示: 当前工具链目录中没有找到可选运行时库文件: {dll}")


def deploy_qt(qt_bin: Path | None, qml_dir: Path, exe_path: Path, keep_translations: bool,
              no_compiler_runtime: bool, keep_opengl_sw: bool):
    if not qt_bin:
        raise RuntimeError("找不到 Qt bin 目录，请传入 --qt-bin 或设置 QT_BIN_PATH。")

    windeployqt_path = qt_bin / "windeployqt.exe"
    if not windeployqt_path.exists():
        raise RuntimeError(f"找不到 windeployqt: {windeployqt_path}")

    command = [str(windeployqt_path), "--release", "--qmldir", str(qml_dir)]
    if not keep_translations:
        command.append("--no-translations")
    if no_compiler_runtime:
        command.append("--no-compiler-runtime")
    if not keep_opengl_sw:
        command.append("--no-opengl-sw")
    command.append(str(exe_path))
    print("部署 Qt:", " ".join(command))

    env = os.environ.copy()
    env["PATH"] = str(qt_bin) + os.pathsep + env.get("PATH", "")
    subprocess.run(command, check=True, env=env)


def release_name(version: str, suffix: str) -> str:
    return f"{APP_NAME}-v{version}-{suffix}" if suffix else f"{APP_NAME}-v{version}"


def create_zip(package_dir: Path, output_dir: Path, artifact_name: str) -> Path:
    zip_path = output_dir / f"{artifact_name}.zip"
    if zip_path.exists():
        zip_path.unlink()

    with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as archive:
        for file_path in sorted(package_dir.rglob("*")):
            if file_path.is_file():
                archive.write(file_path, file_path.relative_to(package_dir.parent))

    print(f"发布包: {zip_path}")
    return zip_path


def write_sha256(file_path: Path, output_path: Path | None = None) -> Path:
    digest = hashlib.sha256(file_path.read_bytes()).hexdigest()
    sha_path = output_path if output_path else file_path.with_suffix(file_path.suffix + ".sha256")
    sha_path.write_text(f"{digest}  {file_path.name}\n", encoding="utf-8")
    print(f"校验文件: {sha_path}")
    return sha_path


def copy_directory(source: Path, destination: Path):
    if destination.exists():
        shutil.rmtree(destination)
    shutil.copytree(source, destination)


def copy_file(source: Path, destination: Path):
    destination.parent.mkdir(parents=True, exist_ok=True)
    if destination.exists():
        destination.unlink()
    shutil.copy2(source, destination)


def run_velopack(vpk_path: str, pack_dir: Path, output_dir: Path, version: str, pack_id: str, pack_title: str,
                 pack_authors: str, icon_path: Path):
    command = [
        vpk_path,
        "--legacyConsole",
        "pack",
        "--packId", pack_id,
        "--packVersion", version,
        "--packDir", str(pack_dir),
        "--mainExe", f"{APP_NAME}.exe",
        "--packTitle", pack_title,
        "--packAuthors", pack_authors,
        "--icon", str(icon_path),
        "--outputDir", str(output_dir),
    ]
    print("生成 Velopack 安装版:", " ".join(command))
    try:
        subprocess.run(command, check=True)
    except FileNotFoundError as error:
        raise RuntimeError("找不到 vpk，请先安装 Velopack CLI，或用 --vpk 指定 vpk.exe 路径。") from error


def find_velopack_setup(velopack_output_dir: Path) -> Path:
    candidates = sorted(velopack_output_dir.glob("*Setup.exe"), key=lambda path: path.stat().st_mtime, reverse=True)
    if not candidates:
        raise FileNotFoundError(f"没有在 Velopack 输出目录中找到 Setup.exe: {velopack_output_dir}")
    return candidates[0]


def main():
    args = parse_args()
    apply_interactive_defaults(args)
    root = project_root()
    output_dir = Path(args.output_dir).resolve()
    work_dir = Path(args.work_dir).resolve()
    velopack_output_dir = Path(args.velopack_output_dir).resolve() if args.velopack_output_dir else work_dir / "velopack"
    qml_dir = root / "qml"
    icon_path = root / "resources" / f"{APP_NAME}.ico"
    build_dir = find_build_dir(root, args.build_dir)
    cache = read_cmake_cache(build_dir)
    version = cache.get("CMAKE_PROJECT_VERSION", "0.0.0")
    base_artifact_name = release_name(version, args.name_suffix)
    portable_artifact_name = f"{base_artifact_name}-portable"
    installer_artifact_name = f"{base_artifact_name}-setup"
    portable_dir = work_dir / portable_artifact_name
    package_dir = work_dir / "app"

    if not qml_dir.exists():
        raise FileNotFoundError(f"找不到 QML 目录: {qml_dir}")
    if args.with_velopack and not icon_path.exists():
        raise FileNotFoundError(f"找不到 Velopack 图标文件: {icon_path}")

    exe_path = find_exe(build_dir)
    qt_bin = infer_qt_bin(cache, args.qt_bin)
    mingw_bin = infer_mingw_bin(cache, args.mingw_bin)
    objdump = infer_objdump(cache, mingw_bin)
    runtime_search_dirs = []
    for path in (mingw_bin, qt_bin):
        if path and path not in runtime_search_dirs:
            runtime_search_dirs.append(path)

    print(f"使用构建目录: {build_dir}")
    print(f"使用可执行文件: {exe_path}")

    default_output_root = root / "output"
    if is_relative_to(output_dir, default_output_root) and is_relative_to(work_dir, default_output_root) and is_relative_to(velopack_output_dir, default_output_root):
        clean_directory(default_output_root)
    else:
        clean_directory(output_dir)
        clean_directory(work_dir)
        if args.with_velopack and args.velopack_output_dir:
            clean_directory(velopack_output_dir)

    clean_directory(package_dir)
    packaged_exe_path = package_dir / exe_path.name
    shutil.copy2(exe_path, packaged_exe_path)
    print(f"已复制可执行文件: {packaged_exe_path}")

    deploy_qt(qt_bin, qml_dir, packaged_exe_path, args.keep_translations, args.no_compiler_runtime,
              args.keep_opengl_sw)
    copy_runtime_dlls(runtime_search_dirs, package_dir, discover_missing_imports(package_dir, objdump))

    if args.with_velopack:
        clean_directory(velopack_output_dir)
        run_velopack(args.vpk, package_dir, velopack_output_dir, version, args.pack_id, args.pack_title,
                     args.pack_authors, icon_path)

        setup_path = find_velopack_setup(velopack_output_dir)
        installer_path = output_dir / f"{installer_artifact_name}.exe"
        copy_file(setup_path, installer_path)
        print(f"安装包: {installer_path}")
        write_sha256(installer_path)

        if args.keep_velopack_feed:
            feed_dir = output_dir / "velopack-feed"
            copy_directory(velopack_output_dir, feed_dir)
            print(f"Velopack 更新源文件: {feed_dir}")

    copy_directory(package_dir, portable_dir)
    if not args.no_portable_marker:
        (portable_dir / PORTABLE_MARKER).write_text("FSClicker portable mode marker.\n", encoding="utf-8")
        print(f"已写入便携模式标记: {PORTABLE_MARKER}")

    if not args.skip_zip:
        output_dir.mkdir(parents=True, exist_ok=True)
        zip_path = create_zip(portable_dir, output_dir, portable_artifact_name)
        write_sha256(zip_path)
    else:
        portable_output_dir = output_dir / portable_artifact_name
        copy_directory(portable_dir, portable_output_dir)
        print(f"便携版目录: {portable_output_dir}")

    remove_directory(work_dir)


if __name__ == "__main__":
    main()
