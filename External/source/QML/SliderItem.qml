import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt5Compat.GraphicalEffects

Rectangle {
    id: root

    property string title: "Slider Item"
    property string description: "Description here"
    property real sliderValue: 50.0
    property real minValue: 0.0
    property real maxValue: 100.0
    property string valueText: Math.round(sliderValue).toString()
    property color accentColor: "#C62828"

    signal valueChanged(real value)

    width: parent.width
    height: 90
    radius: 8
    color: "#1a1a1a"
    border.color: hovered ? accentColor + "30" : "#2a2a2a"
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
        opacity: 0.8

        Behavior on opacity {
            NumberAnimation { duration: 300; easing.type: Easing.InOutQuad }
        }
    }

    Column {
        anchors.fill: parent
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        anchors.topMargin: 12
        anchors.bottomMargin: 12
        spacing: 8

        // Title and description
        Row {
            width: parent.width
            spacing: 0

            Column {
                width: parent.width - 80
                spacing: 4

                Text {
                    text: root.title
                    color: "#ffffff"
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

            // Value display
            Item {
                width: 80
                height: 40

                Rectangle {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    width: 70
                    height: 24
                    radius: 4
                    color: "#2a2a2a"
                    border.color: root.accentColor + "40"
                    border.width: 1

                    Text {
                        anchors.centerIn: parent
                        text: Math.round(root.sliderValue).toString()
                        color: root.accentColor
                        font.pixelSize: 12
                        font.bold: true
                        font.family: "Arial" // 안전한 폰트 사용
                    }
                }
            }
        }

        // Custom slider
        Item {
            width: parent.width
            height: 24

            Rectangle {
                id: sliderTrack
                width: parent.width
                height: 4
                radius: 2
                anchors.verticalCenter: parent.verticalCenter
                color: "#3a3a3a"

                Rectangle {
                    id: sliderProgress
                    width: (Math.round(root.sliderValue) - root.minValue) / (root.maxValue - root.minValue) * parent.width
                    height: parent.height
                    radius: 2
                    color: root.accentColor

                    Behavior on width {
                        NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
                    }
                }
            }

            Rectangle {
                id: sliderHandle
                width: 20
                height: 20
                radius: 10
                color: "#ffffff"
                border.color: root.accentColor
                border.width: 2
                
                x: (Math.round(root.sliderValue) - root.minValue) / (root.maxValue - root.minValue) * (parent.width - width)
                anchors.verticalCenter: parent.verticalCenter

                scale: handleMouseArea.pressed ? 1.2 : (root.hovered ? 1.1 : 1.0)

                Behavior on x {
                    NumberAnimation { duration: 100; easing.type: Easing.OutQuad }
                }

                Behavior on scale {
                    NumberAnimation { duration: 150; easing.type: Easing.OutQuad }
                }

                layer.enabled: true
                layer.effect: DropShadow {
                    horizontalOffset: 0
                    verticalOffset: 2
                    radius: 4
                    color: "#40000000"
                    samples: 9
                }
            }

            MouseArea {
                id: handleMouseArea
                anchors.fill: parent
                hoverEnabled: true

                property real startValue: 0
                property real startMouseX: 0

                onEntered: {
                    root.hovered = true
                }

                onExited: {
                    root.hovered = false
                }

                onPressed: function(mouse) {
                    startValue = root.sliderValue
                    startMouseX = mouse.x
                }

                onPositionChanged: function(mouse) {
                    if (pressed) {
                        var delta = mouse.x - startMouseX
                        var valueRange = root.maxValue - root.minValue
                        var pixelRange = width - sliderHandle.width
                        var valueDelta = (delta / pixelRange) * valueRange
                        
                        var newValue = startValue + valueDelta
                        newValue = Math.max(root.minValue, Math.min(root.maxValue, newValue))
                        
                        // 정수로 반올림
                        newValue = Math.round(newValue)
                        
                        root.sliderValue = newValue
                        root.valueChanged(newValue)
                    }
                }

                onClicked: function(mouse) {
                    var pixelRange = width - sliderHandle.width
                    var valueRange = root.maxValue - root.minValue
                    var clickRatio = (mouse.x - sliderHandle.width/2) / pixelRange
                    clickRatio = Math.max(0, Math.min(1, clickRatio))
                    
                    var newValue = root.minValue + (clickRatio * valueRange)
                    
                    // 정수로 반올림
                    newValue = Math.round(newValue)
                    
                    root.sliderValue = newValue
                    root.valueChanged(newValue)
                }
            }
        }
    }
}