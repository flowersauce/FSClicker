# 项目演进记录

## 2026-04-13 至 2026-04-14

本阶段把 FSClicker 从早期 Qt Widgets 版本推进到 Qt 6.11 + QML 的可发布状态。旧实现保留在 `old_code/`，当前主线代码位于
`src/`、`include/`、`qml/`、`resources/` 和 `scripts/`。

## 当前架构

- `ClickerController` 负责 QML 状态、配置读写入口、全局热键、坐标捕获和启动/停止控制。
- `EventInjector` 运行在线程中，使用 Win32 `SendInput` 发送鼠标和键盘事件。
- `KeyboardHook` 安装键盘和鼠标低级钩子，支持普通按键、鼠标按键和滚轮上/下捕获。
- `AppConfig` 负责 `config.json` 读取、边界检查、无效配置重置和退出时统一保存。
- QML 已拆分：主界面为 `qml/main.qml`，主题为 `qml/theme/atom_theme.qml`，通用控件位于 `qml/components/`。

## 已完成能力

- 鼠标左键、中键、右键连点。
- 自定义键盘按键，以及滚轮上/下动作。
- 连击和长按两种输入行为。滚轮动作固定走连击，避免长按语义不成立。
- 自由光标和固定坐标两种模式，坐标捕获使用全屏 QML 覆盖层。
- 周期输入支持 `0% - 20%` 动态误差。当前实现是“单次间隔抖动”，长期均值围绕设定周期。
- 主题支持自动、深色、浅色。自动模式跟随 Windows 深浅色偏好。
- 语言、缩放、主题、连点功能配置都持久化到 exe 同目录的 `config.json`。
- 运行中会禁用侧栏和设置页入口，避免启动后修改关键配置。
- 标题栏按钮改为三段式轻圆角按钮，绿色最小化，黄色置顶，红色关闭。
- 关于页图标使用 `resources/FSClicker_transparent.svg`。
- 程序图标使用 `resources/FSClicker.ico`，由 `resources/app_icon.rc` 引入。

## 性能与调度

- 当前连点调度基于 `QElapsedTimer` 的 deadline 循环。
- 高频场景保留忙等/yield 组合，避免纯 `msleep(1)` 带来的明显精度损失。
- 误差逻辑不再抖动目标时间点，而是直接抖动每次等待间隔：

```text
下次等待间隔 = 周期 + 本次随机误差
```

- 例如周期 `0.01s`、误差 `20%` 时，理论间隔约为 `8ms - 12ms`。

## 打包与发布

- 打包脚本为 `scripts/package_app.py`。
- 脚本不负责编译，只搜索名称包含 `build` 的目录，并使用其中最新的 `FSClicker.exe`。
- 默认执行：

```powershell
python scripts/package_app.py
```

- 默认发布包包含必需的 MinGW/编译器运行时 DLL，排除 Qt 翻译文件和软件 OpenGL 兜底库。
- 如需保留软件 OpenGL 兜底库，可使用：

```powershell
python scripts/package_app.py --keep-opengl-sw
```

- 音频播放已从 Qt Multimedia 改为 WinAPI `PlaySoundA`，避免引入 FFmpeg 和 `Qt6Multimedia.dll`。
- 当前仍使用动态 Qt，不再保留静态 Qt 专用 CMake 选项。若后续要做静态 Qt，应作为独立发布方案重新评估。

## 文档与资源

- `README.md` 为默认中文说明。
- `README.en.md` 为英文说明。
- README 不再引用本地图片资源。
- README 和文档中的构建示例使用环境变量，不写入本机绝对路径。

## 后续可考虑

- 在 Release 构建下复测 CPS 和打包体积。
- 在普通 Windows 环境下验证默认不带 `opengl32sw.dll` 的发布包。
- 继续压缩 Qt Quick Controls 部署体积，但这会牵涉控件实现方式，优先级低于功能稳定性。
