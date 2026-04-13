// 标题栏按钮。

import QtQuick
import "../theme"

Item {
    id: trafficButton

    property color normalColor: theme.control
    property color hoverColor: Qt.lighter(normalColor, 1.12)
    property color pressedColor: Qt.darker(normalColor, 1.22)
    property bool leftRounded: false
    property bool rightRounded: false
    property bool checked: false

    signal clicked()

    width: 23
    height: 14

    AtomTheme {
        id: theme
        dark: appConfig.darkTheme
    }

    Rectangle {
        anchors.fill: parent
        topLeftRadius: trafficButton.leftRounded ? theme.trafficRadius : 3
        bottomLeftRadius: trafficButton.leftRounded ? theme.trafficRadius : 3
        topRightRadius: trafficButton.rightRounded ? theme.trafficRadius : 3
        bottomRightRadius: trafficButton.rightRounded ? theme.trafficRadius : 3
        color: trafficMouseArea.pressed || trafficButton.checked ? trafficButton.pressedColor : (trafficMouseArea.containsMouse ? trafficButton.hoverColor : trafficButton.normalColor)
    }

    MouseArea {
        id: trafficMouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: trafficButton.clicked()
    }
}
