<p align="center">
  <img src="assets/repository header image.png" alt="头图">
</p>  

欢迎使用 FSClicker  
一款免费开源的Windows连点器


------

你可以使用 FSClicker：

> * 以极高的CPS执行鼠标或键盘的单一事件
> * 在你想要的任意固定位置执行鼠标单一事件
> * 以任意周期执行鼠标事件集合 ( 计划开发中 )

------
应用预览:

<p align="center">
  <img src="assets/application preview A.png" alt="关于页面" style="width: 400px;"> 
  <img src="assets/application preview B.png" alt="应用页面" style="width: 400px;">
</p> 

------

开发环境:

> * CMake 3.30+
> * Qt 6.11.0, 需要 Core、Quick、Qml 与 Multimedia 模块
> * Windows 下推荐使用 Qt 6.11.0 自带 MinGW 13.1.0 + Ninja
> * 项目当前使用 C++23

命令行构建示例:

```powershell
D:\APP\DevEnv\Qt\Tools\CMake_64\bin\cmake.exe -S . -B build-qml -G Ninja `
  -DCMAKE_CXX_COMPILER=D:/APP/DevEnv/Qt/Tools/mingw1310_64/bin/g++.exe `
  -DCMAKE_MAKE_PROGRAM=D:/APP/DevEnv/Qt/Tools/Ninja/ninja.exe `
  -DCMAKE_PREFIX_PATH=D:/APP/DevEnv/Qt/6.11.0/mingw_64 `
  -DFS_CLICKER_STATIC_RUNTIME=ON `
  -DFS_CLICKER_STATIC_QT=OFF

D:\APP\DevEnv\Qt\Tools\CMake_64\bin\cmake.exe --build build-qml
```

项目默认启用 `FS_CLICKER_STATIC_RUNTIME=ON`，会尽量静态链接 MinGW 运行时。Qt 本身是否能静态链接取决于你安装的 Qt 是否为静态构建版；如果只有普通动态版 Qt，仍然需要用 `windeployqt` 部署 Qt DLL 与 QML/Quick 依赖。

如果已经安装 Qt 6.11.0 静态构建版，可以用静态 kit 路径并开启检查:

```powershell
D:\APP\DevEnv\Qt\Tools\CMake_64\bin\cmake.exe -S . -B build-qml-static -G Ninja `
  -DCMAKE_CXX_COMPILER=D:/APP/DevEnv/Qt/Tools/mingw1310_64/bin/g++.exe `
  -DCMAKE_MAKE_PROGRAM=D:/APP/DevEnv/Qt/Tools/Ninja/ninja.exe `
  -DCMAKE_PREFIX_PATH=D:/APP/DevEnv/Qt/6.11.0/mingw_64_static `
  -DFS_CLICKER_STATIC_RUNTIME=ON `
  -DFS_CLICKER_STATIC_QT=ON
```

打包示例:

```powershell
python scripts/package_app.py `
  --build-dir build-qml `
  --qt-bin D:/APP/DevEnv/Qt/6.11.0/mingw_64/bin `
  --mingw-bin D:/APP/DevEnv/Qt/Tools/mingw1310_64/bin `
  --qml-dir qml
```
