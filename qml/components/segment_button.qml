// 分段选择按钮。

import QtQuick
import "../theme"

Item {
    id: segmentButton

    property bool selected: false
    property bool rightRounded: false
    property string text: ""

    signal clicked()

    width: 60
    height: 32
    enabled: parent ? parent.enabled : true

    AtomTheme {
        id: theme
        dark: appConfig.darkTheme
    }

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
