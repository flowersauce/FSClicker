import argparse
import os
import shutil
import subprocess
import zipfile
from pathlib import Path


APP_NAME = "FSClicker"
RUNTIME_DLLS = ("libstdc++-6.dll", "libgcc_s_seh-1.dll", "libwinpthread-1.dll")


def parse_args():
    parser = argparse.ArgumentParser(description="Package FS Clicker for Windows distribution.")
    parser.add_argument("--build-dir", default="build", help="CMake build directory.")
    parser.add_argument("--qt-bin", default=os.environ.get("QT_BIN_PATH"), help="Qt bin directory containing windeployqt.exe.")
    parser.add_argument("--mingw-bin", default=os.environ.get("MINGW_BIN_PATH"), help="MinGW bin directory containing runtime DLLs.")
    parser.add_argument("--output-dir", default=f"output/{APP_NAME}", help="Directory for packaged files.")
    parser.add_argument("--exe-name", default=f"{APP_NAME}.exe", help="Executable name produced by CMake.")
    parser.add_argument("--zip-name", default=f"{APP_NAME}.zip", help="Zip file name created under output/.")
    return parser.parse_args()


def clean_directory(path):
    path.mkdir(parents=True, exist_ok=True)
    for child in path.iterdir():
        if child.is_dir():
            shutil.rmtree(child)
        else:
            child.unlink()


def copy_runtime_dlls(mingw_bin, output_dir):
    if not mingw_bin:
        print("跳过 MinGW 运行时复制: 未设置 --mingw-bin 或 MINGW_BIN_PATH")
        return

    for dll in RUNTIME_DLLS:
        dll_path = mingw_bin / dll
        if dll_path.exists():
            shutil.copy2(dll_path, output_dir)
            print(f"已复制运行时库文件: {dll}")
        else:
            print(f"警告: 找不到运行时库文件: {dll_path}")


def deploy_qt(qt_bin, exe_path):
    if not qt_bin:
        raise RuntimeError("未设置 Qt bin 路径，请传入 --qt-bin 或设置 QT_BIN_PATH")

    windeployqt_path = qt_bin / "windeployqt.exe"
    if not windeployqt_path.exists():
        raise RuntimeError(f"找不到 windeployqt 工具: {windeployqt_path}")

    cmd = [str(windeployqt_path), str(exe_path)]
    print(f"运行命令: {' '.join(cmd)}")
    subprocess.run(cmd, check=True)
    print("Qt 依赖库已部署完成")


def create_zip(output_dir, zip_name):
    zip_file_path = Path("output") / zip_name
    zip_file_path.parent.mkdir(parents=True, exist_ok=True)

    with zipfile.ZipFile(zip_file_path, "w", zipfile.ZIP_DEFLATED) as zipf:
        for file_path in output_dir.rglob("*"):
            if file_path.is_file():
                arcname = file_path.relative_to(output_dir.parent)
                zipf.write(file_path, arcname)

    print(f"打包完成: {zip_file_path}")


def main():
    args = parse_args()
    build_dir = Path(args.build_dir)
    output_dir = Path(args.output_dir)
    qt_bin = Path(args.qt_bin) if args.qt_bin else None
    mingw_bin = Path(args.mingw_bin) if args.mingw_bin else qt_bin
    exe_path = build_dir / args.exe_name

    if not exe_path.exists():
        raise FileNotFoundError(f"找不到可执行文件: {exe_path}")

    clean_directory(output_dir)
    shutil.copy2(exe_path, output_dir)
    packaged_exe_path = output_dir / args.exe_name
    print(f"已复制可执行文件到输出目录: {packaged_exe_path}")

    copy_runtime_dlls(mingw_bin, output_dir)
    deploy_qt(qt_bin, packaged_exe_path)
    create_zip(output_dir, args.zip_name)


if __name__ == "__main__":
    main()
