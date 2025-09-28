import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects

Rectangle {
    id: root

    property string title: "Toggle Item"
    property string description: "Description here"
    property bool toggleState: false
    property color accentColor: "#C62828"

    signal toggled(bool state)

    width: parent.width
    height: 70
    radius: 8
    color: "#1a1a1a"
    border.color: hovered ? (toggleState ? accentColor + "30" : "#383838") : "#2a2a2a"
    border.width: 1

    property bool hovered: false

    Behavior on border.color {
        ColorAnimation { duration: 200 }
    }

    Behavior on color {
        ColorAnimation { duration: 200 }
    }

    Rectangle {
        id: accentBar
        width: 3
        height: parent.height - 20
        radius: 1.5
        color: root.accentColor
        anchors.left: parent.left
        anchors.leftMargin: 6
        anchors.verticalCenter: parent.verticalCenter
        opacity: root.toggleState ? 1.0 : 0.0

        Behavior on opacity {
            NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 20
        anchors.rightMargin: 120
        anchors.topMargin: 12
        anchors.bottomMargin: 12
        spacing: 0

        Column {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            spacing: 4

            Text {
                text: root.title
                color: root.toggleState ? "#ffffff" : "#d0d0d0"
                font.pixelSize: 16
                font.bold: true

                Behavior on color {
                    ColorAnimation { duration: 200 }
                }
            }

            Text {
                text: root.description
                color: "#999999"
                font.pixelSize: 13
                opacity: 0.85
            }
        }
    }

    Rectangle {
        id: toggleSwitch
        width: 54
        height: 28
        radius: 14
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.verticalCenter: parent.verticalCenter
        color: root.toggleState ? root.accentColor : "#3a3a3a"
        border.width: 1

        Behavior on color {
            ColorAnimation { duration: 250; easing.type: Easing.InOutQuad }
        }

        Behavior on border.color {
            ColorAnimation { duration: 250 }
        }

        Rectangle {
            id: toggleGrip
            width: 24
            height: 24
            radius: 12
            color: "#ffffff"
            x: root.toggleState ? parent.width - width - 2 : 2
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: -1

            Behavior on x {
                NumberAnimation {
                    duration: 250
                    easing.type: Easing.InOutQuad
                }
            }

            layer.enabled: true
            layer.effect: DropShadow {
                horizontalOffset: 0
                verticalOffset: 1
                radius: 2
                color: "#60000000"
                samples: 3
            }
        }

        scale: root.hovered ? 1.05 : 1.0
        Behavior on scale {
            NumberAnimation { duration: 150; easing.type: Easing.OutQuad }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onEntered: {
            root.hovered = true
        }

        onExited: {
            root.hovered = false
        }

        onClicked: {
            root.toggleState = !root.toggleState
            root.toggled(root.toggleState)
        }
    }
}
