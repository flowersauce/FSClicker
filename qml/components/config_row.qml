// 配置行底座。

import QtQuick
import "../theme"

Rectangle {
    id: configRow

    property string label: ""
    property bool rowEnabled: true

    width: 320
    height: 32
    radius: theme.controlRadius
    color: theme.configBar
    enabled: rowEnabled

    AtomTheme {
        id: theme
        dark: appConfig.darkTheme
    }

    Rectangle {
        x: 0
        y: 0
        width: 80
        height: 32
        topLeftRadius: theme.controlRadius
        bottomLeftRadius: theme.controlRadius
        color: theme.control
        opacity: configRow.enabled ? 1.0 : 0.55

        Text {
            anchors.centerIn: parent
            text: configRow.label
            color: configRow.enabled ? theme.activeText : theme.mutedText
            font.pixelSize: 12
        }
    }
}
