// 配置行右侧小按钮。

import QtQuick
import "../theme"

Item {
    id: smallActionButton

    property string text: ""
    property bool checked: false

    signal clicked()

    width: 77
    height: 32
    enabled: parent ? parent.enabled : true

    AtomTheme {
        id: theme
        dark: appConfig.darkTheme
    }

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
