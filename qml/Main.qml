import QtQuick
import QtQuick.Controls
import QtQuick.Window
import "theme"

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
            "退格": "Back",
            "回车": "Enter",
            "空格": "Space",
            "截屏": "Print",
            "未设置": "Unset",
            "未知": "Unknown"
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
            "按下全局开关启动": "Press hotkey to start",
            "按下全局开关终止": "Press hotkey to stop",
            "按下新的全局开关": "Press new hotkey",
            "按下要连点的键": "Press target key",
            "点击屏幕捕获坐标": "Click screen to pick point",
            "按键不可用，请重试": "Key unavailable",
            "等待配置完成": "Complete setup"
        }
        return statusTexts[text] || text
    }

    function setApplicationScale(value) {
        appConfig.uiScale = value
    }

    AtomTheme {
        id: theme; dark: appConfig.darkTheme
    }

    component TrafficButton: Item {
        id: trafficButton
        property color normalColor: theme.control
        property color hoverColor: normalColor
        property color pressedColor: theme.controlPressed
        signal clicked()
        width: 14
        height: 14
        Rectangle {
            anchors.fill: parent
            radius: theme.trafficRadius
            color: trafficMouseArea.pressed ? trafficButton.pressedColor : (trafficMouseArea.containsMouse ? trafficButton.hoverColor : trafficButton.normalColor)
        }
        MouseArea {
            id: trafficMouseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: trafficButton.clicked()
        }
    }

    component NavButton: Item {
        id: navButton
        property bool selected: false
        property string text: ""
        signal clicked()
        width: 88
        height: 32
        Rectangle {
            anchors.fill: parent
            radius: theme.controlRadius
            color: !navButton.enabled ? theme.controlDisabled
                   : (navMouseArea.pressed ? theme.controlPressed
                                      : (navButton.selected ? theme.controlChecked
                                                            : (navMouseArea.containsMouse ? theme.controlHover : theme.controlNormal)))
            border.color: theme.controlBorder
            border.width: 0
        }
        Text {
            anchors.fill: parent
            text: navButton.text
            color: navButton.enabled ? theme.activeText : theme.mutedText
            font.pixelSize: 15
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        MouseArea {
            id: navMouseArea
            anchors.fill: parent
            enabled: navButton.enabled
            hoverEnabled: true
            onClicked: navButton.clicked()
        }
    }

    component SegmentButton: Item {
        id: segmentButton
        property bool selected: false
        property bool rightRounded: false
        property string text: ""
        signal clicked()
        width: 60
        height: 32
        enabled: parent ? parent.enabled : true
        Rectangle {
            anchors.fill: parent
            color: !segmentButton.enabled ? theme.controlDisabled
                   : (segmentMouseArea.pressed ? theme.controlPressed
                                         : (segmentButton.selected ? theme.controlChecked
                                                                  : (segmentMouseArea.containsMouse ? theme.controlHover : theme.controlNormal)))
            topRightRadius: segmentButton.rightRounded ? theme.controlRadius : 0
            bottomRightRadius: segmentButton.rightRounded ? theme.controlRadius : 0
            border.color: theme.controlBorder
            border.width: 0
        }
        Text {
            anchors.fill: parent
            text: segmentButton.text
            color: segmentButton.enabled ? theme.activeText : theme.mutedText
            font.pixelSize: 12
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
        MouseArea {
            id: segmentMouseArea
            anchors.fill: parent
            enabled: segmentButton.enabled
            hoverEnabled: true
            onClicked: segmentButton.clicked()
        }
    }

    component ConfigRow: Rectangle {
        id: configRow
        property string label: ""
        property bool rowEnabled: true
        width: 320
        height: 32
        radius: theme.controlRadius
        color: theme.configBar
        enabled: rowEnabled
        Rectangle {
            x: 0; y: 0; width: 80; height: 32
            topLeftRadius: theme.controlRadius
            bottomLeftRadius: theme.controlRadius
            color: theme.control
            opacity: configRow.enabled ? 1.0 : 0.55
            Text { anchors.centerIn: parent; text: configRow.label; color: configRow.enabled ? theme.activeText : theme.mutedText; font.pixelSize: 12 }
        }
    }

    component SmallActionButton: Item {
        id: smallActionButton
        property string text: ""
        property bool checked: false
        signal clicked()
        width: 77
        height: 32
        enabled: parent ? parent.enabled : true
        Rectangle {
            anchors.fill: parent
            color: !smallActionButton.enabled ? theme.controlDisabled
                   : (smallMouseArea.pressed ? theme.controlPressed
                                             : (smallActionButton.checked ? theme.controlChecked
                                                                         : (smallMouseArea.containsMouse ? theme.controlHover : theme.controlNormal)))
            topRightRadius: theme.controlRadius
            bottomRightRadius: theme.controlRadius
            border.color: theme.controlBorder
            border.width: 0
        }
        Text {
            anchors.fill: parent
            text: smallActionButton.text
            color: smallActionButton.enabled ? theme.activeText : theme.mutedText
            font.pixelSize: 12
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
        MouseArea {
            id: smallMouseArea
            anchors.fill: parent
            enabled: smallActionButton.enabled
            hoverEnabled: true
            onClicked: smallActionButton.clicked()
        }
    }

    Rectangle {
        width: root.baseWidth
        height: root.baseHeight
        scale: root.uiScale
        transformOrigin: Item.TopLeft
        radius: theme.windowRadius
        color: theme.window
        border.color: theme.windowBorder
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

                TrafficButton {
                    x: 407; y: 9; normalColor: theme.green; hoverColor: theme.trafficPinHover; onClicked:
                    { root.pinned = !root.pinned; root.updateWindowFlags() }
                }
                TrafficButton {
                    x: 430; y: 9; normalColor: theme.yellow; hoverColor: theme.trafficMinimizeHover; onClicked: root.showMinimized()
                }
                TrafficButton {
                    x: 453; y: 9; normalColor: theme.danger; hoverColor: theme.trafficCloseHover; onClicked: Qt.quit()
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
                    x: 16; y: 16; text: root.trText("功能", "Func"); selected: root.pageIndex === 0; onClicked: root.pageIndex = 0
                }
                NavButton {
                    x: 16; y: 54; text: root.trText("设置", "Set"); selected: root.pageIndex === 1; onClicked: root.pageIndex = 1
                }
                NavButton {
                    x: 16; y: 92; text: root.trText("关于", "Info"); selected: root.pageIndex === 2; onClicked: root.pageIndex = 2
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
                        y: 168; label: root.trText("输入周期", "Period"); rowEnabled: clicker.inputActionMode === 0 && !clicker.running
                        TextField {
                            x: 80; y: 0; width: 240; height: 32
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
                            onEditingFinished: clicker.cycleSeconds = Number(text)
                        }
                        Text {
                            x: 292; y: 0; width: 28; height: 32; text: root.trText("秒", "s"); color: parent.enabled ? theme.activeText : theme.mutedText; font.family: root.trText(root.uiFontFamily, root.latinFontFamily); font.pixelSize: 12; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
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
                        y: 16; label: root.trText("主题模式", "Theme")
                        SegmentButton { x: 80; y: 0; width: 120; height: 32; text: root.trText("深色", "Dark"); selected: theme.dark; onClicked: appConfig.darkTheme = true }
                        SegmentButton { x: 200; y: 0; width: 120; height: 32; text: root.trText("浅色", "Light"); rightRounded: true; selected: !theme.dark; onClicked: appConfig.darkTheme = false }
                    }

                    ConfigRow {
                        y: 54; label: root.trText("应用缩放", "Scale")
                        SegmentButton { x: 80; y: 0; width: 80; height: 32; text: "100%"; selected: root.uiScale === 1.0; onClicked: root.setApplicationScale(1.0) }
                        SegmentButton { x: 160; y: 0; width: 80; height: 32; text: "125%"; selected: root.uiScale === 1.25; onClicked: root.setApplicationScale(1.25) }
                        SegmentButton { x: 240; y: 0; width: 80; height: 32; text: "150%"; rightRounded: true; selected: root.uiScale === 1.5; onClicked: root.setApplicationScale(1.5) }
                    }

                    ConfigRow {
                        y: 92; label: root.trText("界面语言", "Language")
                        SegmentButton { x: 80; y: 0; width: 120; height: 32; text: "简体中文"; selected: root.language === "zh"; onClicked: appConfig.language = "zh" }
                        SegmentButton { x: 200; y: 0; width: 120; height: 32; text: "English"; rightRounded: true; selected: root.language === "en"; onClicked: appConfig.language = "en" }
                    }

                    ConfigRow {
                        y: 130; label: root.trText("预留设置", "Reserved"); rowEnabled: false
                        Text {
                            x: 80; y: 0; width: 240; height: 32
                            text: root.trText("更多选项将在这里添加", "More options will appear here")
                            color: theme.mutedText
                            font.pixelSize: 12
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                        }
                    }
                }

                Item {
                    id: aboutPage
                    x: 140; y: 0; width: 320; height: 292
                    visible: root.pageIndex === 2

                    Image {
                        x: 115; y: 30; width: 90; height: 90; source: "qrc:/resources/application.png"
                        fillMode: Image.PreserveAspectFit
                    }
                    Text {
                        x: 0; y: 130; width: 320; height: 32
                        text: "FSClicker"
                        color: theme.link
                        font.family: root.latinFontFamily
                        font.pixelSize: 22
                        font.weight: Font.DemiBold
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    Text {
                        x: 0; y: 172; width: 320
                        text: root.trText("版本 1.3.0\n作者 Flowersauce", "Version 1.3.0\nAuthor Flowersauce")
                        color: theme.activeText
                        font.pixelSize: 13
                        lineHeight: 1.6
                        horizontalAlignment: Text.AlignHCenter
                    }
                    Text {
                        x: 0; y: 246; width: 320; height: 24
                        text: root.trText("打开项目仓库", "Open repository")
                        color: theme.link
                        font.pixelSize: 13
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        MouseArea {
                            anchors.fill: parent; onClicked: Qt.openUrlExternally("https://github.com/flowersauce/FlowersauceClicker")
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
            requestActivate();
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
