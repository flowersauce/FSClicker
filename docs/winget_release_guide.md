# Winget 发布手册

本文记录 FSClicker 提交到 Windows Package Manager Community Repository 的发布流程。

## 前提

本机需要：

```powershell
winget
wingetcreate
git
```

FSClicker 的 winget 包 ID：

```text
Flowersauce.FSClicker
```

winget 使用 Velopack 安装器：

```text
FSClicker-v<version>-windows-x64-setup.exe
```

## 生成发布产物

更新 `CMakeLists.txt` 中的 `FS_CLICKER_VERSION` 后，先完成项目构建，再运行：

```powershell
python scripts/package_app.py --with-velopack
```

最终发布产物输出到：

```text
output/release/
```

应包含：

```text
FSClicker-v<version>-windows-x64-setup.exe
FSClicker-v<version>-windows-x64-setup.exe.sha256
FSClicker-v<version>-windows-x64-portable.zip
FSClicker-v<version>-windows-x64-portable.zip.sha256
```

本地测试安装器：

```powershell
.\output\release\FSClicker-v<version>-windows-x64-setup.exe --silent
winget list FSClicker
winget uninstall --product-code "Flowersauce.FSClicker"
```

## 创建 GitHub Release

在 FSClicker 仓库创建 tag，例如：

```text
v1.3.1
```

上传至少：

```text
FSClicker-v1.3.1-windows-x64-setup.exe
FSClicker-v1.3.1-windows-x64-setup.exe.sha256
```

建议同时上传便携版：

```text
FSClicker-v1.3.1-windows-x64-portable.zip
FSClicker-v1.3.1-windows-x64-portable.zip.sha256
```

不要覆盖已经发布的同名安装器。winget 依赖固定 URL 和固定 SHA256。

## 准备 Manifest

推荐复制上一版 manifest 后修改版本号、URL 和 hash。

目录结构：

```text
manifests/f/Flowersauce/FSClicker/<version>/
```

例如：

```text
manifests/f/Flowersauce/FSClicker/1.3.1/
```

目录内包含三份 YAML：

```text
Flowersauce.FSClicker.yaml
Flowersauce.FSClicker.installer.yaml
Flowersauce.FSClicker.locale.en-US.yaml
```

### Version Manifest

```yaml
PackageIdentifier: Flowersauce.FSClicker
PackageVersion: 1.3.1
DefaultLocale: en-US
ManifestType: version
ManifestVersion: 1.12.0
```

### Installer Manifest

```yaml
PackageIdentifier: Flowersauce.FSClicker
PackageVersion: 1.3.1
InstallerType: exe
Scope: user
InstallModes:
- silent
- silentWithProgress
InstallerSwitches:
  Silent: --silent
  SilentWithProgress: --silent
UpgradeBehavior: install
ProductCode: Flowersauce.FSClicker
AppsAndFeaturesEntries:
- DisplayName: FSClicker
  DisplayVersion: 1.3.1
  Publisher: Flowersauce
  ProductCode: Flowersauce.FSClicker
  InstallerType: exe
Installers:
- Architecture: x64
  InstallerUrl: https://github.com/flowersauce/FSClicker/releases/download/v1.3.1/FSClicker-v1.3.1-windows-x64-setup.exe
  InstallerSha256: <新安装器 SHA256>
  ProductCode: Flowersauce.FSClicker
ManifestType: installer
ManifestVersion: 1.12.0
```

### Locale Manifest

```yaml
PackageIdentifier: Flowersauce.FSClicker
PackageVersion: 1.3.1
PackageLocale: en-US
Publisher: Flowersauce
PublisherUrl: https://github.com/flowersauce
PublisherSupportUrl: https://github.com/flowersauce/FSClicker/issues
PackageName: FSClicker
PackageUrl: https://github.com/flowersauce/FSClicker
License: MIT
LicenseUrl: https://github.com/flowersauce/FSClicker/blob/main/LICENSE
ShortDescription: A lightweight auto clicker for Windows.
Tags:
- auto-clicker
- clicker
- input
- windows
ReleaseNotesUrl: https://github.com/flowersauce/FSClicker/releases/tag/v1.3.1
ManifestType: defaultLocale
ManifestVersion: 1.12.0
```

## 本地验证

验证 manifest：

```powershell
winget validate output\winget-manifests\manifests\f\Flowersauce\FSClicker\1.3.1
```

本地安装测试：

```powershell
winget install --manifest output\winget-manifests\manifests\f\Flowersauce\FSClicker\1.3.1
```

卸载测试：

```powershell
winget uninstall --product-code "Flowersauce.FSClicker"
```

本地 manifest 安装后，`winget uninstall --id Flowersauce.FSClicker` 不一定能找到包。使用 `--product-code` 测试即可。

## 提交到 winget-pkgs

可使用：

```powershell
wingetcreate submit output\winget-manifests\manifests\f\Flowersauce\FSClicker\1.3.1
```

也可以在 GitHub 网页编辑 fork 分支，添加新的版本目录和三份 YAML。

如果需要手动查找 PR：

```text
repo:microsoft/winget-pkgs is:pr author:flowersauce Flowersauce.FSClicker
```

## PR Checklist

可以勾选：

```text
Have you signed the Contributor License Agreement?
Have you checked that there aren't other open pull requests for the same manifest update/change?
This PR only modifies one (1) manifest
Have you validated your manifest locally with winget validate --manifest <path>?
Have you tested your manifest locally with winget install --manifest <path>?
Does your manifest conform to the 1.12 schema?
```

没有 issue 时不要勾选：

```text
Is there a linked Issue?
```

## CLA

如果 PR 要求签署 CLA，在 PR 评论：

```text
@microsoft-github-policy-service agree
```

如果状态没有更新：

```text
@microsoft-github-policy-service rerun
```

## 常见问题

SmartScreen 提示“无法识别的应用”通常是未签名或低信誉新安装器导致，不代表 manifest 错误。最稳的解决方案是代码签名；未签名时可先提交 PR，看自动验证是否接受。

裸 exe 安装后，winget 可能显示：

```text
ARP\User\X64\Flowersauce.FSClicker
```

可使用：

```powershell
winget uninstall --id "ARP\User\X64\Flowersauce.FSClicker" --exact
```

或：

```powershell
winget uninstall --product-code "Flowersauce.FSClicker"
```

