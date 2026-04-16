// FSClicker 主窗口。

import QtQuick
import QtQuick.Controls
import QtQuick.Window
import "theme"
import "components"

ApplicationWindow {
    id: root
    property int baseWidth: 500
    property int baseHeight: 360
    property real uiScale: appConfig.uiScale
    readonly property int scaledWidth: Math.round(baseWidth * uiScale)
    readonly property int scaledHeight: Math.round(baseHeight * uiScale)
    property string language: appConfig.language
    readonly property string latinFontFamily: "Jura"
    readonly property string uiFontFamily: "Sarasa UI SC"
    readonly property string monoFontFamily: "Jura"

    width: scaledWidth
    height: scaledHeight
    minimumWidth: scaledWidth
    minimumHeight: scaledHeight
    maximumWidth: scaledWidth
    maximumHeight: scaledHeight
    visible: true
    title: "FSClicker"
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.Window

    property bool pinned: false
    property int pageIndex: 0
    readonly property color startColor: clicker.running ? theme.danger : (clicker.canStart ? theme.accent : theme.accentDisabled)
    readonly property color startTextColor: clicker.running || clicker.canStart ? "white" : theme.mutedText

    function updateWindowFlags() {
        root.flags = Qt.FramelessWindowHint | Qt.Window | (root.pinned ? Qt.WindowStaysOnTopHint : 0)
        root.show()
    }

    function trText(zh, en) {
        return language === "zh" ? zh : en
    }

    function displayKeyName(name) {
        if (language === "zh") {
            return name
        }

        const keyNames = {
            "鼠标左键": "Left",
            "鼠标中键": "Middle",
            "鼠标右键": "Right",
            "滚轮上"  : "Whl Up",
            "滚轮下"  : "Whl Dn",
            "退格"    : "Back",
            "回车"    : "Enter",
            "空格"    : "Space",
            "截屏"    : "Print",
            "未设置"  : "Unset",
            "未知"    : "Unknown"
        }
        return keyNames[name] || name
    }

    function displayDiyKeyName() {
        if (clicker.diyKeyName === "未设置") {
            return trText("自定义", "Custom")
        }
        return displayKeyName(clicker.diyKeyName)
    }

    function displayStatusText(text) {
        if (language === "zh") {
            return text
        }

        const statusTexts = {
            "按下全局开关启动" : "Press hotkey to start",
            "按下全局开关终止" : "Press hotkey to stop",
            "按下新的全局开关" : "Press new hotkey",
            "按下要连点的键"   : "Press target key",
            "点击屏幕捕获坐标" : "Click screen to pick point",
            "按键不可用，请重试": "Key unavailable",
            "等待配置完成"     : "Complete setup"
        }
        return statusTexts[text] || text
    }

    function setApplicationScale(value) {
        appConfig.uiScale = value
    }

    function setTimingJitterPercent(value) {
        const normalized            = Math.max(0, Math.min(20, Number(value) || 0))
        clicker.timingJitterPercent = normalized
    }

    AtomTheme {
        id: theme; dark: appConfig.darkTheme
    }

    Connections {
        target: clicker

        function onRunningChanged() {
            if (clicker.running) {
                root.pageIndex = 0
            }
        }
    }

    Rectangle {
        width: root.baseWidth
        height: root.baseHeight
        scale: root.uiScale
        transformOrigin: Item.TopLeft
        radius: theme.windowRadius
        color: theme.window
        border.color: root.pinned ? (theme.dark ? Qt.lighter(theme.windowBorder, 1.45) : Qt.darker(theme.windowBorder, 1.25)) : theme.windowBorder
        border.width: theme.borderWidth
        clip: true

        Item {
            anchors.fill: parent

            Rectangle {
                id: titleBar
                x: 12; y: 12; width: 476; height: 32
                radius: theme.panelRadius
                color: theme.titleBar

                MouseArea {
                    anchors.fill: parent; acceptedButtons: Qt.LeftButton; onPressed: root.startSystemMove()
                }

                Text {
                    x: 16; y: 0; width: 250; height: 32; text: "FSClicker"; color: theme.titleText; font.family: root.latinFontFamily; font.pixelSize: 14; font.weight: Font.DemiBold; verticalAlignment: Text.AlignVCenter; elide: Text.ElideRight
                }

                Item {
                    id: trafficGroup
                    x: 394; y: 9; width: 73; height: 14
                    TrafficButton {
                        x: 0; y: 0; leftRounded: true; normalColor: theme.green; onClicked: root.showMinimized()
                    }
                    TrafficButton {
                        x: 25; y: 0; checked: root.pinned; normalColor: theme.yellow; onClicked:
                        { root.pinned = !root.pinned; root.updateWindowFlags() }
                    }
                    TrafficButton {
                        x: 50; y: 0; rightRounded: true; normalColor: theme.danger; onClicked: Qt.quit()
                    }
                }
            }

            Rectangle {
                id: functionPanel
                x: 12; y: 56; width: 476; height: 292
                radius: 20
                color: theme.panel

                Rectangle {
                    x: 120; y: 16; width: 4; height: 260; radius: 2; color: theme.window
                }
                NavButton {
                    x: 16; y: 16; text: root.trText("功能", "Func"); enabled: !clicker.running; selected: root.pageIndex === 0; onClicked: root.pageIndex = 0
                }
                NavButton {
                    x: 16; y: 54; text: root.trText("设置", "Set"); enabled: !clicker.running; selected: root.pageIndex === 1; onClicked: root.pageIndex = 1
                }
                NavButton {
                    x: 16; y: 92; text: root.trText("关于", "Info"); enabled: !clicker.running; selected: root.pageIndex === 2; onClicked: root.pageIndex = 2
                }

                Item {
                    id: ioPage
                    x: 140; y: 0; width: 320; height: 292
                    visible: root.pageIndex === 0

                    ConfigRow {
                        x: 0; y: 16; width: 157; label: root.trText("全局开关", "Hotkey"); rowEnabled: !clicker.running
                        SmallActionButton {
                            x: 80; y: 0; width: 77; height: 32; text: clicker.capturingGlobalSwitchKey ? root.trText("捕获中", "Capture") : root.displayKeyName(clicker.globalSwitchKeyName); onClicked: clicker.beginGlobalSwitchKeyCapture()
                        }
                    }
                    ConfigRow {
                        x: 163; y: 16; width: 157; label: root.trText("坐标设置", "Position"); rowEnabled: clicker.cursorMoveMode === 1 && !clicker.running
                        SmallActionButton {
                            x: 80; y: 0; width: 77; height: 32; text: clicker.capturingCoordinate ? root.trText("捕获中", "Capture") : clicker.coordinateText; onClicked: clicker.beginCoordinateCapture()
                        }
                    }
                    ConfigRow {
                        y: 54; label: root.trText("输入按键", "Button"); rowEnabled: !clicker.running
                        SegmentButton {
                            x: 80; y: 0; width: 60; height: 32; text: root.trText("左键", "Left"); selected: clicker.inputKey === 0; onClicked: clicker.inputKey = 0
                        }
                        SegmentButton {
                            x: 140; y: 0; width: 60; height: 32; text: root.trText("中键", "Middle"); selected: clicker.inputKey === 1; onClicked: clicker.inputKey = 1
                        }
                        SegmentButton {
                            x: 200; y: 0; width: 60; height: 32; text: root.trText("右键", "Right"); selected: clicker.inputKey === 2; onClicked: clicker.inputKey = 2
                        }
                        SegmentButton {
                            x: 260; y: 0; width: 60; height: 32; text: clicker.capturingDiyKey ? root.trText("捕获", "Pick") : root.displayDiyKeyName(); rightRounded: true; selected: clicker.inputKey === 3; onClicked: clicker.beginDiyKeyCapture()
                        }
                    }
                    ConfigRow {
                        y: 92; label: root.trText("输入行为", "Action"); rowEnabled: !clicker.running
                        SegmentButton {
                            x: 80; y: 0; width: 120; height: 32; text: root.trText("连击", "Repeat"); selected: clicker.inputActionMode === 0; onClicked: clicker.inputActionMode = 0
                        }
                        SegmentButton {
                            x: 200; y: 0; width: 120; height: 32; text: root.trText("长按", "Hold"); rightRounded: true; selected: clicker.inputActionMode === 1; onClicked: clicker.inputActionMode = 1
                        }
                    }
                    ConfigRow {
                        y: 130; label: root.trText("光标移动", "Cursor"); rowEnabled: !clicker.running
                        SegmentButton {
                            x: 80; y: 0; width: 120; height: 32; text: root.trText("自由", "Free"); selected: clicker.cursorMoveMode === 0; onClicked: clicker.cursorMoveMode = 0
                        }
                        SegmentButton {
                            x: 200; y: 0; width: 120; height: 32; text: root.trText("固定", "Lock"); rightRounded: true; selected: clicker.cursorMoveMode === 1; onClicked: clicker.cursorMoveMode = 1
                        }
                    }
                    ConfigRow {
                        y: 168; width: 209; label: root.trText("输入周期", "Period"); rowEnabled: clicker.inputActionMode === 0 && !clicker.running
                        TextField {
                            x: 80; y: 0; width: 101; height: 32
                            text: Number(clicker.cycleSeconds).toFixed(3)
                            color: enabled ? theme.activeText : theme.mutedText
                            horizontalAlignment: TextInput.AlignHCenter
                            verticalAlignment: TextInput.AlignVCenter
                            font.family: root.monoFontFamily
                            font.pixelSize: 12
                            selectByMouse: true
                            validator: DoubleValidator {
                                bottom: 0.001; top: 99999; decimals: 3
                            }
                            background: Rectangle {
                                color: "transparent"; topRightRadius: theme.controlRadius; bottomRightRadius: theme.controlRadius
                            }
                            onTextEdited: if (acceptableInput) clicker.cycleSeconds = Number(text)
                            onEditingFinished: clicker.cycleSeconds = Number(text)
                        }
                        Text {
                            x: 181; y: 0; width: 28; height: 32; text: root.trText("秒", "s"); color: parent.enabled ? theme.activeText : theme.mutedText; font.family: root.trText(root.uiFontFamily, root.latinFontFamily); font.pixelSize: 12; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                        }
                    }
                    Rectangle {
                        x: 215; y: 168; width: 105; height: 32
                        radius: theme.controlRadius
                        color: theme.configBar
                        enabled: clicker.inputActionMode === 0 && !clicker.running
                        Text {
                            x: 0; y: 0; width: 28; height: 32; text: "±"; color: parent.enabled ? theme.activeText : theme.mutedText; font.family: root.monoFontFamily; font.pixelSize: 12; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                        }
                        TextField {
                            x: 28; y: 0; width: 49; height: 32
                            text: Number(clicker.timingJitterPercent).toFixed(0)
                            color: enabled ? theme.activeText : theme.mutedText
                            horizontalAlignment: TextInput.AlignHCenter
                            verticalAlignment: TextInput.AlignVCenter
                            font.family: root.monoFontFamily
                            font.pixelSize: 12
                            selectByMouse: true
                            validator: IntValidator {
                                bottom: 0; top: 20
                            }
                            background: Rectangle {
                                color: "transparent"
                            }
                            onTextEdited: root.setTimingJitterPercent(text)
                            onEditingFinished: root.setTimingJitterPercent(text)
                        }
                        Text {
                            x: 77; y: 0; width: 28; height: 32; text: "%"; color: parent.enabled ? theme.activeText : theme.mutedText; font.family: root.monoFontFamily; font.pixelSize: 12; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                        }
                    }
                    Rectangle {
                        x: 0; y: 244; width: 320; height: 32
                        radius: theme.controlRadius
                        color: root.startColor
                        Text {
                            anchors.centerIn: parent; text: root.displayStatusText(clicker.statusText); color: root.startTextColor; font.pixelSize: 13
                        }
                    }
                }

                Item {
                    id: settingsPage
                    x: 140; y: 0; width: 320; height: 292
                    visible: root.pageIndex === 1

                    ConfigRow {
                        y: 16; label: root.trText("主题模式", "Theme"); rowEnabled: !clicker.running
                        SegmentButton {
                            x: 80; y: 0; width: 80; height: 32; text: root.trText("自动", "Auto"); selected: appConfig.themeMode === 0; onClicked: appConfig.themeMode = 0
                        }
                        SegmentButton {
                            x: 160; y: 0; width: 80; height: 32; text: root.trText("深色", "Dark"); selected: appConfig.themeMode === 1; onClicked: appConfig.themeMode = 1
                        }
                        SegmentButton {
                            x: 240; y: 0; width: 80; height: 32; text: root.trText("浅色", "Light"); rightRounded: true; selected: appConfig.themeMode === 2; onClicked: appConfig.themeMode = 2
                        }
                    }

                    ConfigRow {
                        y: 54; label: root.trText("应用缩放", "Scale"); rowEnabled: !clicker.running
                        SegmentButton {
                            x: 80; y: 0; width: 80; height: 32; text: "100%"; selected: appConfig.uiScaleIndex === 0; onClicked: appConfig.uiScaleIndex = 0
                        }
                        SegmentButton {
                            x: 160; y: 0; width: 80; height: 32; text: "125%"; selected: appConfig.uiScaleIndex === 1; onClicked: appConfig.uiScaleIndex = 1
                        }
                        SegmentButton {
                            x: 240; y: 0; width: 80; height: 32; text: "150%"; rightRounded: true; selected: appConfig.uiScaleIndex === 2; onClicked: appConfig.uiScaleIndex = 2
                        }
                    }

                    ConfigRow {
                        y: 92; label: root.trText("界面语言", "Language"); rowEnabled: !clicker.running
                        SegmentButton {
                            x: 80; y: 0; width: 80; height: 32; text: root.trText("自动", "Auto"); selected: appConfig.languageMode === 0; onClicked: appConfig.languageMode = 0
                        }
                        SegmentButton {
                            x: 160; y: 0; width: 80; height: 32; text: "简体中文"; selected: appConfig.languageMode === 1; onClicked: appConfig.languageMode = 1
                        }
                        SegmentButton {
                            x: 240; y: 0; width: 80; height: 32; text: "English"; rightRounded: true; selected: appConfig.languageMode === 2; onClicked: appConfig.languageMode = 2
                        }
                    }

                }

                Item {
                    id: aboutPage
                    x: 140; y: 0; width: 320; height: 292
                    visible: root.pageIndex === 2

                    Column {
                        x: 0; y: 28; width: 320
                        spacing: 13

                        Image {
                            width: 104; height: 104
                            anchors.horizontalCenter: parent.horizontalCenter
                            source: "qrc:/resources/FSClicker_transparent.svg"
                            fillMode: Image.PreserveAspectFit
                        }
                        Text {
                            width: 320; height: 30
                            text: "FSClicker"
                            color: theme.atomRed
                            font.family: root.latinFontFamily
                            font.pixelSize: 22
                            font.weight: Font.DemiBold
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        Text {
                            width: 320; height: 46
                            text: root.trText("版本 " + appVersion + "\n作者 " + appAuthor, "Version " + appVersion + "\nAuthor " + appAuthor)
                            color: theme.activeText
                            font.pixelSize: 13
                            lineHeight: 1.45
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        Text {
                            id: repositoryLink
                            readonly property color defaultColor: theme.mutedText
                            readonly property color hoverColor: theme.dark ? Qt.lighter(defaultColor, 1.28) : Qt.darker(defaultColor, 1.12)
                            readonly property color pressedColor: theme.dark ? Qt.darker(defaultColor, 1.18) : Qt.lighter(defaultColor, 1.12)

                            width: implicitWidth; height: 24
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: root.trText("打开项目仓库", "Open repository")
                            color: repositoryMouseArea.pressed ? pressedColor : (repositoryMouseArea.containsMouse ? hoverColor : defaultColor)
                            font.pixelSize: 13
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            MouseArea {
                                id: repositoryMouseArea
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked: Qt.openUrlExternally(appRepositoryUrl)
                            }
                        }
                    }
                }
            }
        }
    }

    Window {
        id: coordinateCaptureWindow
        visible: false
        flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool
        color: "transparent"
        title: "捕获坐标"
        property real cursorX: width / 2
        property real cursorY: height / 2
        property real scaleFactor: screen ? screen.devicePixelRatio : 1
        property int actualX: Math.round(cursorX * scaleFactor)
        property int actualY: Math.round(cursorY * scaleFactor)
        property string coordinateLabel: "X: " + actualX + ", Y: " + actualY

        function openCaptureWindow() {
            showFullScreen();
            coordinateCaptureWindow.requestActivate();
            captureMouseArea.forceActiveFocus()
        }

        function closeCaptureWindow() {
            hide()
        }

        Connections {
            target: clicker

            function onCaptureStateChanged() {
                if (clicker.capturingCoordinate) coordinateCaptureWindow.openCaptureWindow()
                else coordinateCaptureWindow.closeCaptureWindow()
            }
        }
        Rectangle {
            anchors.fill: parent; color: "#000000"; opacity: 0.4
        }
        Rectangle {
            x: coordinateCaptureWindow.cursorX - 1; y: 0; width: 2; height: parent.height; color: "white"
        }
        Rectangle {
            x: 0; y: coordinateCaptureWindow.cursorY - 1; width: parent.width; height: 2; color: "white"
        }
        Text {
            text: coordinateCaptureWindow.coordinateLabel
            color: "white"
            font.family: root.monoFontFamily
            font.pixelSize: 14
            x: coordinateCaptureWindow.cursorX + width + 20 > coordinateCaptureWindow.width ? coordinateCaptureWindow.cursorX - width - 10 : coordinateCaptureWindow.cursorX + 10
            y: coordinateCaptureWindow.cursorY + height + 20 > coordinateCaptureWindow.height ? coordinateCaptureWindow.cursorY - height - 10 : coordinateCaptureWindow.cursorY + height + 10
        }
        MouseArea {
            id: captureMouseArea
            anchors.fill: parent
            hoverEnabled: true
            focus: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onPositionChanged: function (mouse)
                {
                    coordinateCaptureWindow.cursorX = mouse.x;
                    coordinateCaptureWindow.cursorY = mouse.y
                }
            onClicked: function (mouse)
                {
                    coordinateCaptureWindow.cursorX = mouse.x
                    coordinateCaptureWindow.cursorY = mouse.y
                    if (mouse.button === Qt.LeftButton) clicker.finishCoordinateCapture(coordinateCaptureWindow.actualX, coordinateCaptureWindow.actualY)
                    else if (mouse.button === Qt.RightButton) clicker.cancelCoordinateCapture()
                }
            Keys.onEscapePressed: clicker.cancelCoordinateCapture()
        }
    }
}
