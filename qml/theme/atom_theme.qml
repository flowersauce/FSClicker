// FSClicker 主题色。

import QtQuick

QtObject {
    id: theme

    property bool dark: true

    readonly property color atomBg: "#282c34"
    readonly property color atomGutter: "#21252b"
    readonly property color atomSelection: "#3e4451"
    readonly property color atomComment: "#5c6370"
    readonly property color atomText: "#abb2bf"
    readonly property color atomBlue: "#61afef"
    readonly property color atomRed: "#e06c75"
    readonly property color atomGreen: "#98c379"
    readonly property color atomYellow: "#e5c07b"

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
    readonly property color mutedText: dark ? atomComment : "#5e6875"
    readonly property color titleText: dark ? atomBlue : "#245f9f"
    readonly property color accent: dark ? atomBlue : "#2f6fed"
    readonly property color accentDisabled: dark ? "#1f3552" : "#b8c9ee"
    readonly property color danger: dark ? atomRed : "#d64545"
    readonly property color link: dark ? atomRed : "#c94141"
    readonly property color yellow: dark ? atomYellow : "#e3a008"
    readonly property color green: dark ? atomGreen : "#2da44e"
    readonly property color trafficCloseHover: dark ? "#be5046" : "#cf222e"
    readonly property color trafficMinimizeHover: dark ? "#d19a66" : "#bf8700"
    readonly property color trafficPinHover: dark ? "#56b6c2" : "#1a7f37"

    readonly property int windowRadius: 28
    readonly property int panelRadius: 16
    readonly property int controlRadius: 8
    readonly property int trafficRadius: 7
    readonly property int borderWidth: 2
}
