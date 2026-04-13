# Work Log

## 2026-04-13

- Rebuilt FSClicker as a Qt 6.11 QML application while keeping the core clicker logic in C++.
- Moved the old Qt Widgets implementation under `old_code/` for reference.
- Standardized the application name to `FSClicker` in CMake, QML, docs, and packaging script.
- Current build target is `FSClicker.exe`; CMake uses C++23 with Qt 6.11.0.
- Static Qt is not installed in the current environment, so the active build is the dynamic Qt 6.11 MinGW build.
- `WrapVulkanHeaders` is still reported as missing during CMake configure; this is a non-blocking Qt package detection warning and is being ignored for now.
- The UI is now a frameless QML recreation of the old rounded custom window style, using the Atom One Dark-inspired theme.
- Sidebar pages are `功能`, `设置`, and `关于`; English labels are also wired.
- Fonts are currently `Jura-Medium.ttf` for Latin text and `SarasaUiSC-Regular.ttf` for Chinese text.
- Settings page currently controls theme, app scale, and language.
- App scale is persisted and now bound directly to the window size, so `125%` and `150%` should resize both canvas and window.
- Portable JSON persistence has been added at `<FSClicker.exe directory>/config.json`.
- Persisted config covers clicker options, coordinate, hotkeys, cycle seconds, theme, scale, and language.
- The custom key button now displays the captured key name when a DIY key is selected.
- Coordinate capture is implemented as a fullscreen QML overlay; left click confirms, right click or Escape cancels.
- The high-performance jitter experiment was reverted because it performed worse in CPS testing on this machine.
- Event injection is currently close to the original `QElapsedTimer + QThread::msleep(1)` loop, with only minor thread/atomic cleanup retained.
- Known issue to revisit later: actual CPS accuracy should be retested on another machine before touching the injection loop again.
- PowerShell still prints Terminal-Icons `Export-Clixml Access denied` noise in command output; it is unrelated to the project and is being ignored for now.

Recommended next steps:

1. Test `build-qml-dynamic-elevated/FSClicker.exe` on the home machine.
2. Verify CPS accuracy against the old version before changing `src/event_injector.cpp` again.
3. If CPS is still poor only on this machine, keep the backend unchanged and continue UI/settings work.
4. If persistence needs cleanup, inspect the generated `config.json` beside the executable.
