// FSClicker 主题色。

import QtQuick

QtObject {
    id: theme

    property bool dark: true

    readonly property color atomBg: dark ? "#282c34" : "#fafafa"
    readonly property color atomGutter: dark ? "#21252b" : "#f0f0f0"
    readonly property color atomSelection: dark ? "#3e4451" : "#e5e5e6"
    readonly property color atomComment: dark ? "#5c6370" : "#a0a1a7"
    readonly property color atomText: dark ? "#abb2bf" : "#383a42"
    readonly property color atomBlue: dark ? "#61afef" : "#4078f2"
    readonly property color atomRed: dark ? "#e06c75" : "#e45649"
    readonly property color atomGreen: dark ? "#98c379" : "#50a14f"
    readonly property color atomYellow: dark ? "#e5c07b" : "#c18401"

    readonly property color window: dark ? atomGutter : "#edf1f7"
    readonly property color windowBorder: dark ? atomSelection : "#a8b2c1"
    readonly property color titleBar: dark ? atomBg : "#f9fbff"
    readonly property color titleBarBorder: dark ? atomBg : "#c7d0dc"
    readonly property color panel: dark ? atomBg : "#ffffff"
    readonly property color panelBorder: dark ? atomBg : "#cbd5e1"
    readonly property color configBar: dark ? atomSelection : "#d7dee9"
    readonly property color control: dark ? "#4b5263" : "#c3ccd9"
    readonly property color controlHover: dark ? "#262b32" : "#e4e9f1"
    readonly property color controlPressed: dark ? atomGutter : "#aeb9c8"
    readonly property color controlChecked: dark ? atomGutter : "#b8c2d1"
    readonly property color controlDisabled: dark ? "transparent" : "transparent"
    readonly property color controlNormal: "transparent"
    readonly property color controlBorder: "transparent"
    readonly property color controlPressedBorder: "transparent"
    readonly property color text: dark ? atomText : "#24292f"
    readonly property color activeText: dark ? "#ffffff" : "#0f1720"
    readonly property color mutedText: atomComment
    readonly property color titleText: dark ? atomBlue : "#245f9f"
    readonly property color accent: dark ? atomBlue : "#2f6fed"
    readonly property color accentDisabled: dark ? "#1f3552" : "#b8c9ee"
    readonly property color danger: atomRed
    readonly property color link: atomRed
    readonly property color yellow: atomYellow
    readonly property color green: atomGreen
    readonly property int windowRadius: 28
    readonly property int panelRadius: 16
    readonly property int controlRadius: 8
    readonly property int trafficRadius: 7
    readonly property int borderWidth: 2
}
