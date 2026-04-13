// 左侧导航按钮。

import QtQuick
import "../theme"

Item {
    id: navButton

    property bool selected: false
    property string text: ""

    signal clicked()

    width: 88
    height: 32

    AtomTheme {
        id: theme
        dark: appConfig.darkTheme
    }

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
