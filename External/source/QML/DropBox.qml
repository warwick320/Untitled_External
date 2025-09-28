import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    property string title: "Dropdown Item"
    property string description: "Description here"
    property var items: ["Option 1", "Option 2", "Option 3"]
    property int selectedIndex: 0
    property color accentColor: "#C62828" 
    property bool menuOpen: false
    property int maxVisibleItems: 4  
    signal valueChanged(int index, string value)

    width: parent ? parent.width : 320
    height: 70
    radius: 8
    color: "#1a1a1a"
    border.color: hovered ? (menuOpen ? accentColor + "30" : "#383838") : "#2a2a2a"
    border.width: 1

    property bool hovered: false
    property bool isToggling: false  

    Behavior on border.color { ColorAnimation { duration: 200 } }
    Behavior on color { ColorAnimation { duration: 200 } }

    Rectangle {
        id: accentBar
        width: 3
        height: parent.height - 20
        radius: 1.5
        color: root.accentColor
        anchors.left: parent.left
        anchors.leftMargin: 6
        anchors.verticalCenter: parent.verticalCenter
        opacity: root.menuOpen ? 1.0 : 0.0
        Behavior on opacity { NumberAnimation { duration: 300; easing.type: Easing.InOutQuad } }
    }

    Column {
        anchors.fill: parent
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        anchors.topMargin: 12
        anchors.bottomMargin: 12
        spacing: 4

        Row {
            width: parent.width
            spacing: 0

            Column {
                width: parent.width - 120  
                spacing: 4

                Text {
                    text: root.title
                    color: root.menuOpen ? "#ffffff" : "#d0d0d0"
                    font.pixelSize: 16
                    font.bold: true
                    Behavior on color { ColorAnimation { duration: 200 } }
                }

                Text {
                    text: root.description
                    color: "#999999"
                    font.pixelSize: 13
                    opacity: 0.85
                }
            }

            Item {
                width: 120
                height: 40

                Rectangle {
                    id: dropdownBox
                    anchors.right: arrowContainer.left
                    anchors.rightMargin: 8
                    anchors.verticalCenter: parent.verticalCenter
                    width: 80
                    height: 24
                    radius: 4
                    color: root.menuOpen ? root.accentColor + "15" : "#2a2a2a"  
                    border.color: root.accentColor + "40"
                    border.width: 1

                    Behavior on border.color { ColorAnimation { duration: 200 } }
                    Behavior on color { ColorAnimation { duration: 200 } }

                    Text {
                        anchors.centerIn: parent
                        text: root.items[root.selectedIndex] || ""
                        color: root.accentColor
                        font.pixelSize: 12
                        font.bold: true
                        font.family: "Arial"
                        elide: Text.ElideRight
                        width: parent.width - 8
                        horizontalAlignment: Text.AlignHCenter
                    }
                }


                Rectangle {
                    id: arrowContainer
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    width: 24
                    height: 24
                    radius: 12
                    color: root.menuOpen ? root.accentColor + "20" : "transparent"
                    border.color: root.menuOpen ? root.accentColor + "60" : root.accentColor + "30"
                    border.width: 1

                    Behavior on color { ColorAnimation { duration: 200 } }
                    Behavior on border.color { ColorAnimation { duration: 200 } }

                    Canvas {
                        id: arrowCanvas
                        anchors.centerIn: parent
                        width: 12
                        height: 8

                        property color arrowColor: root.accentColor
                        property real rotationAngle: root.menuOpen ? 180 : 0

                        rotation: rotationAngle

                        Behavior on rotation {
                            RotationAnimation {
                                duration: 250
                                easing.type: Easing.OutQuart
                            }
                        }

                        Behavior on arrowColor { ColorAnimation { duration: 200 } }

                        onPaint: {
                            var ctx = getContext("2d")
                            ctx.clearRect(0, 0, width, height)

                            ctx.strokeStyle = arrowColor
                            ctx.lineWidth = 2
                            ctx.lineCap = "round"
                            ctx.lineJoin = "round"

                            ctx.beginPath()
                            ctx.moveTo(2, 3)
                            ctx.lineTo(6, 7)
                            ctx.lineTo(10, 3)
                            ctx.stroke()
                        }

                        onArrowColorChanged: requestPaint()
                        Component.onCompleted: requestPaint()
                    }
                }
            }
        }
    }

    MouseArea {
        id: mainMouseArea
        anchors.fill: parent
        hoverEnabled: true
        onEntered: root.hovered = true
        onExited: root.hovered = false
        onClicked: {
            if (root.isToggling) return

            root.isToggling = true
            root.toggleMenu()

            toggleTimer.restart()
        }
    }

    Timer {
        id: toggleTimer
        interval: 300
        repeat: false
        onTriggered: root.isToggling = false
    }


    function toggleMenu() {
        if (root.menuOpen) {
            root.menuOpen = false
            dropdownMenu.close()
        } else {
            root.menuOpen = true
            dropdownMenu.open()
        }
    }

    function closeMenu() {
        if (root.menuOpen) {
            root.menuOpen = false
            dropdownMenu.close()
        }
    }


    MouseArea {
        id: outsideClickArea
        anchors.fill: parent.parent || parent
        visible: root.menuOpen
        enabled: root.menuOpen && !root.isToggling
        z: -1

        onClicked: {
            if (!root.isToggling) {
                root.closeMenu()
            }
        }
    }

    Popup {
        id: dropdownMenu
        x: 20
        y: root.height + 4
        width: root.width - 40
        height: root.menuOpen ? (root.maxVisibleItems * 32 + 8 + (root.items.length > root.maxVisibleItems ? 20 : 0)) : 0
        visible: root.menuOpen
        modal: false
        focus: false
        closePolicy: Popup.NoAutoClose

        property real showAnim: 0.0

        background: Rectangle {
            color: "#1e1e1e"
            radius: 8
            border.color: root.accentColor + "30"
            border.width: 1
            opacity: dropdownMenu.showAnim

            Rectangle {
                anchors.fill: parent
                anchors.margins: -2
                radius: parent.radius + 2
                color: root.accentColor
                opacity: 0.1
                z: -1
            }
        }

        onVisibleChanged: {
            if (visible) {
                dropdownMenu.showAnim = 0.0
                showAnimAnim.running = true
            } else {
                showAnimAnim.running = false
                dropdownMenu.showAnim = 0.0
            }
        }

        Item {
            anchors.fill: parent
            anchors.margins: 4
            opacity: dropdownMenu.showAnim
            y: (1.0 - dropdownMenu.showAnim) * 16

            ListView {
                id: listView
                width: parent.width
                height: root.maxVisibleItems * 32
                model: root.items
                interactive: root.items.length > root.maxVisibleItems
                clip: true

                delegate: Rectangle {
                    width: listView.width
                    height: 32
                    radius: 6
                    color: {
                        if (index === root.selectedIndex)
                            return root.accentColor + "44"
                        else if (itemMouseArea.containsMouse)
                            return root.accentColor + "20"
                        else
                            return "transparent"
                    }

                    Behavior on color { ColorAnimation { duration: 150 } }


                    scale: itemMouseArea.pressed ? 0.98 : 1.0
                    Behavior on scale { NumberAnimation { duration: 100; easing.type: Easing.OutQuart } }

                    Row {
                        anchors.fill: parent
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        spacing: 8

                        Rectangle {
                            id: selectionIndicator
                            width: 6
                            height: 6
                            radius: 3
                            anchors.verticalCenter: parent.verticalCenter
                            color: root.accentColor
                            visible: index === root.selectedIndex
                            opacity: index === root.selectedIndex ? 1.0 : 0.0

                            Behavior on opacity {
                                NumberAnimation {
                                    duration: 200
                                    easing.type: Easing.OutQuart
                                }
                            }
                            Rectangle {
                                anchors.centerIn: parent
                                width: parent.width + 4
                                height: parent.height + 4
                                radius: (parent.width + 4) / 2
                                color: root.accentColor
                                opacity: 0.4
                                visible: parent.visible

                                SequentialAnimation on scale {
                                    running: index === root.selectedIndex && root.menuOpen
                                    loops: Animation.Infinite
                                    NumberAnimation { from: 1.0; to: 1.3; duration: 800; easing.type: Easing.InOutSine }
                                    NumberAnimation { from: 1.3; to: 1.0; duration: 800; easing.type: Easing.InOutSine }
                                }
                            }
                        }

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: modelData
                            color: index === root.selectedIndex ? "#ffffff" : "#e0e0e0"
                            font.pixelSize: 14
                            font.bold: index === root.selectedIndex
                            elide: Text.ElideRight
                            width: parent.width - selectionIndicator.width - parent.spacing - 20

                            Behavior on color { ColorAnimation { duration: 150 } }
                        }
                    }

                    MouseArea {
                        id: itemMouseArea
                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            root.selectedIndex = index
                            root.valueChanged(index, modelData)
                            root.closeMenu()
                        }
                    }
                }
            }

            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: root.items.length > root.maxVisibleItems ? 20 : 0
                visible: root.items.length > root.maxVisibleItems
                color: "transparent"

                Text {
                    anchors.centerIn: parent
                    text: `+${root.items.length - root.maxVisibleItems} more`
                    color: root.accentColor + "80"
                    font.pixelSize: 10
                    font.italic: true
                }
            }
        }

        NumberAnimation {
            id: showAnimAnim
            target: dropdownMenu
            property: "showAnim"
            from: 0.0
            to: 1.0
            duration: 180
            easing.type: Easing.OutQuad
        }
    }

    Keys.onEscapePressed: {
        root.closeMenu()
    }


    focus: true
    Keys.onSpacePressed: {
        if (!root.isToggling) {
            root.isToggling = true
            root.toggleMenu()
            toggleTimer.restart()
        }
    }
    Keys.onEnterPressed: {
        if (!root.isToggling) {
            root.isToggling = true
            root.toggleMenu()
            toggleTimer.restart()
        }
    }
    Keys.onReturnPressed: {
        if (!root.isToggling) {
            root.isToggling = true
            root.toggleMenu()
            toggleTimer.restart()
        }
    }
}
