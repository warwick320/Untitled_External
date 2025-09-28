import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

FocusScope {
    id: root
    property string title: "Keybind Setting"
    property string description: "Press to set key combination"
    property string currentKeybind: "None"
    property color accentColor: "#C62828"
    property bool isRecording: false
    signal keybindChanged(string newKeybind)
    signal recordingStarted()
    signal recordingFinished()

    width: parent ? parent.width : 320
    height: 70

    property bool hovered: false
    property var pressedKeys: []
    property string tempKeybind: ""
    property bool mousePressed: false

    Rectangle {
        id: mainRect
        anchors.fill: parent
        radius: 8
        color: "#1a1a1a"
        border.color: root.hovered ? (root.isRecording ? root.accentColor : "#383838") : "#2a2a2a"
        border.width: 1

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
            opacity: root.isRecording ? 1.0 : 0.0
            Behavior on opacity { NumberAnimation { duration: 300; easing.type: Easing.InOutQuad } }
        }

        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            color: root.accentColor
            opacity: root.isRecording ? pulseAnimation.opacity : 0.0
            visible: root.isRecording

            SequentialAnimation {
                id: pulseAnimation
                running: root.isRecording
                loops: Animation.Infinite

                property real opacity: 0.0

                NumberAnimation {
                    target: pulseAnimation
                    property: "opacity"
                    from: 0.0
                    to: 0.15
                    duration: 800
                    easing.type: Easing.InOutSine
                }
                NumberAnimation {
                    target: pulseAnimation
                    property: "opacity"
                    from: 0.15
                    to: 0.0
                    duration: 800
                    easing.type: Easing.InOutSine
                }
            }
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
                    width: parent.width - 140
                    spacing: 4

                    Text {
                        text: root.title
                        color: root.isRecording ? "#ffffff" : "#d0d0d0"
                        font.pixelSize: 16
                        font.bold: true
                        Behavior on color { ColorAnimation { duration: 200 } }
                    }

                    Text {
                        text: root.isRecording ? "Press key/mouse combination..." : root.description
                        color: root.isRecording ? root.accentColor : "#999999"
                        font.pixelSize: 13
                        opacity: 0.85
                        Behavior on color { ColorAnimation { duration: 200 } }
                    }
                }

                Item {
                    width: 140
                    height: 40

                    Item {
                        id: keycapContainer
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        width: 120
                        height: 32


                        Rectangle {
                            id: outerFrame
                            anchors.fill: parent
                            radius: 8
                            color: root.isRecording ? "#2A0A0F" : "#1A1A1A"
                            border.width: 2
                            border.color: root.isRecording ? "#4A1419" : "#2A2A2A"
                        }

                        Rectangle {
                            id: keycapSide
                            anchors.fill: outerFrame
                            anchors.margins: 4
                            radius: 6
                            color: root.isRecording ? "#3D1016" : "#2F2F2F"
                            border.width: 1
                            border.color: root.isRecording ? "#5A1821" : "#3F3F3F"
                        }

                        Rectangle {
                            id: keycapTop
                            anchors.fill: keycapSide
                            anchors.margins: 4
                            radius: 4

                            gradient: Gradient {
                                GradientStop {
                                    position: 0.0
                                    color: root.isRecording ? "#8B2635" : "#6A6A6A"
                                }
                                GradientStop {
                                    position: 0.3
                                    color: root.isRecording ? "#7A2129" : "#5A5A5A"
                                }
                                GradientStop {
                                    position: 0.7
                                    color: root.isRecording ? "#6A1D25" : "#4A4A4A"
                                }
                                GradientStop {
                                    position: 1.0
                                    color: root.isRecording ? "#5A1A23" : "#3A3A3A"
                                }
                            }

                            border.width: 1
                            border.color: root.isRecording ? root.accentColor : "#777777"

                            Behavior on border.color { ColorAnimation { duration: 200 } }

                            Rectangle {
                                anchors.fill: parent
                                anchors.margins: 3
                                radius: 2
                                color: "transparent"
                                border.width: 1
                                border.color: root.isRecording ? "#00000030" : "#00000020"
                            }

                            Text {
                                anchors.centerIn: parent
                                anchors.verticalCenterOffset: 1
                                anchors.horizontalCenterOffset: 1
                                text: root.isRecording ? (root.tempKeybind !== "" ? root.tempKeybind : "...") :
                                      (root.currentKeybind !== "None" ? root.currentKeybind : "Click to set")
                                color: "#000000"
                                font.pixelSize: 9
                                font.bold: true
                                font.family: "Consolas, 'Courier New', monospace"
                                elide: Text.ElideRight
                                width: parent.width - 12
                                horizontalAlignment: Text.AlignHCenter
                                opacity: 0.5
                            }

                            Text {
                                id: keycapText
                                anchors.centerIn: parent
                                text: root.isRecording ? (root.tempKeybind !== "" ? root.tempKeybind : "...") :
                                      (root.currentKeybind !== "None" ? root.currentKeybind : "Click to set")
                                color: root.isRecording ? "#ffffff" : "#f0f0f0"
                                font.pixelSize: 9
                                font.bold: true
                                font.family: "Consolas, 'Courier New', monospace"
                                elide: Text.ElideRight
                                width: parent.width - 12
                                horizontalAlignment: Text.AlignHCenter

                                Behavior on color { ColorAnimation { duration: 200 } }
                            }


                            Rectangle {
                                anchors.fill: parent
                                radius: parent.radius
                                color: "#000000"
                                opacity: root.isRecording ? 0.2 : 0.0

                                Behavior on opacity { NumberAnimation { duration: 150 } }
                            }
                        }


                        Rectangle {
                            anchors.right: keycapTop.right
                            anchors.rightMargin: 8
                            anchors.verticalCenter: keycapTop.verticalCenter
                            width: 2
                            height: 10
                            radius: 1
                            color: root.accentColor
                            visible: root.isRecording
                            opacity: cursorBlink.opacity


                            Rectangle {
                                anchors.centerIn: parent
                                width: parent.width + 4
                                height: parent.height + 4
                                radius: 2
                                color: root.accentColor
                                opacity: 0.4
                            }

                            SequentialAnimation {
                                id: cursorBlink
                                running: root.isRecording
                                loops: Animation.Infinite

                                property real opacity: 1.0

                                NumberAnimation {
                                    target: cursorBlink
                                    property: "opacity"
                                    from: 1.0
                                    to: 0.3
                                    duration: 600
                                    easing.type: Easing.InOutSine
                                }
                                NumberAnimation {
                                    target: cursorBlink
                                    property: "opacity"
                                    from: 0.3
                                    to: 1.0
                                    duration: 600
                                    easing.type: Easing.InOutSine
                                }
                            }
                        }


                        Rectangle {
                            anchors.centerIn: outerFrame
                            width: outerFrame.width + 6
                            height: outerFrame.height + 6
                            radius: outerFrame.radius + 3
                            color: "transparent"
                            border.width: root.isRecording ? 2 : 0
                            border.color: root.accentColor
                            opacity: root.isRecording ? 0.8 : 0.0

                            Behavior on opacity { NumberAnimation { duration: 300 } }
                            Behavior on border.width { NumberAnimation { duration: 200 } }
                        }

                        transform: Translate {
                            x: root.isRecording ? 1 : 0
                            y: root.isRecording ? 1 : 0
                            Behavior on x { NumberAnimation { duration: 150 } }
                            Behavior on y { NumberAnimation { duration: 150 } }
                        }
                    }
                }
            }
        }

        // 마우스 입력 처리
        MouseArea {
            id: mainMouseArea
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.AllButtons
            propagateComposedEvents: !root.isRecording

            onEntered: root.hovered = true
            onExited: root.hovered = false

            onClicked: function(mouse) { 
                if (!root.isRecording) {
                    startRecording()
                    mouse.accepted = true
                } else {
                    mouse.accepted = false
                }
            }

            onPressed: {
                if (root.isRecording) {
                    var buttonName = ""
                    if (mouse.button === Qt.LeftButton) {
                        buttonName = "LMB"
                    } else if (mouse.button === Qt.RightButton) {
                        buttonName = "RMB"
                    } else if (mouse.button === Qt.MiddleButton) {
                        buttonName = "MMB"
                    }

                    if (buttonName !== "" && !root.pressedKeys.includes(buttonName)) {
                        root.pressedKeys.push(buttonName)
                        updateTempKeybind()
                        root.mousePressed = true
                    }
                    mouse.accepted = true
                }
            }

            onReleased: {
                if (root.isRecording && root.mousePressed) {
                    finishTimer.start()
                    mouse.accepted = true
                }
            }
        }
    }

    Item {
        id: keyDetector
        anchors.fill: parent
        focus: root.isRecording

        Keys.onPressed: {
            if (!root.isRecording) return

            console.log("Key pressed:", event.key, "Modifiers:", event.modifiers)
            event.accepted = true

            handleModifiers(event.modifiers)

            var keyName = getKeyName(event.key)
            if (keyName !== "" && !root.pressedKeys.includes(keyName)) {
                root.pressedKeys.push(keyName)
                updateTempKeybind()
            }
        }

        Keys.onReleased: {
            if (!root.isRecording) return
            event.accepted = true

            if (!root.mousePressed && root.pressedKeys.length > 0) {
                finishTimer.start()
            }
        }

        Keys.onEscapePressed: {
            if (root.isRecording) {
                cancelRecording()
                event.accepted = true
            }
        }
    }

    Timer {
        id: finishTimer
        interval: 100
        onTriggered: {
            if (root.isRecording) {
                finishRecording()
            }
        }
    }

    function startRecording() {
        console.log("Starting recording...")
        root.isRecording = true
        root.pressedKeys = []
        root.tempKeybind = ""
        root.mousePressed = false
        keyDetector.focus = true
        keyDetector.forceActiveFocus()
        root.recordingStarted()
    }

    function cancelRecording() {
        console.log("Cancelling recording...")
        root.isRecording = false
        root.pressedKeys = []
        root.tempKeybind = ""
        root.mousePressed = false
        keyDetector.focus = false
        finishTimer.stop()
        root.recordingFinished()
    }

    function finishRecording() {
        console.log("Finishing recording, keys:", root.pressedKeys, "tempKeybind:", root.tempKeybind)
        if (root.tempKeybind !== "") {
            root.currentKeybind = root.tempKeybind
            root.keybindChanged(root.currentKeybind)
        }
        root.isRecording = false
        root.pressedKeys = []
        root.tempKeybind = ""
        root.mousePressed = false
        keyDetector.focus = false
        finishTimer.stop()
        root.recordingFinished()
    }

    function handleModifiers(modifiers) {
        if (modifiers & Qt.ControlModifier && !root.pressedKeys.includes("Ctrl")) {
            root.pressedKeys.push("Ctrl")
        }
        if (modifiers & Qt.ShiftModifier && !root.pressedKeys.includes("Shift")) {
            root.pressedKeys.push("Shift")
        }
        if (modifiers & Qt.AltModifier && !root.pressedKeys.includes("Alt")) {
            root.pressedKeys.push("Alt")
        }
        if (modifiers & Qt.MetaModifier && !root.pressedKeys.includes("Meta")) {
            root.pressedKeys.push("Meta")
        }
        updateTempKeybind()
    }

    function updateTempKeybind() {
        var modifiers = []
        var regularKeys = []
        var mouseButtons = []

        for (var i = 0; i < root.pressedKeys.length; i++) {
            var key = root.pressedKeys[i]
            if (key === "Ctrl" || key === "Shift" || key === "Alt" || key === "Meta") {
                if (!modifiers.includes(key)) {
                    modifiers.push(key)
                }
            } else if (key === "LMB" || key === "RMB" || key === "MMB") {
                if (!mouseButtons.includes(key)) {
                    mouseButtons.push(key)
                }
            } else {
                if (!regularKeys.includes(key)) {
                    regularKeys.push(key)
                }
            }
        }

        var orderedModifiers = []
        if (modifiers.includes("Ctrl")) orderedModifiers.push("Ctrl")
        if (modifiers.includes("Shift")) orderedModifiers.push("Shift")
        if (modifiers.includes("Alt")) orderedModifiers.push("Alt")
        if (modifiers.includes("Meta")) orderedModifiers.push("Meta")

        var allKeys = orderedModifiers.concat(regularKeys).concat(mouseButtons)
        root.tempKeybind = allKeys.join(" + ")
        console.log("Updated tempKeybind:", root.tempKeybind)
    }

    function getKeyName(key) {
        switch (key) {
            case Qt.Key_Control:
            case Qt.Key_Shift:
            case Qt.Key_Alt:
            case Qt.Key_Meta:
                return ""
            case Qt.Key_A: return "A"
            case Qt.Key_B: return "B"
            case Qt.Key_C: return "C"
            case Qt.Key_D: return "D"
            case Qt.Key_E: return "E"
            case Qt.Key_F: return "F"
            case Qt.Key_G: return "G"
            case Qt.Key_H: return "H"
            case Qt.Key_I: return "I"
            case Qt.Key_J: return "J"
            case Qt.Key_K: return "K"
            case Qt.Key_L: return "L"
            case Qt.Key_M: return "M"
            case Qt.Key_N: return "N"
            case Qt.Key_O: return "O"
            case Qt.Key_P: return "P"
            case Qt.Key_Q: return "Q"
            case Qt.Key_R: return "R"
            case Qt.Key_S: return "S"
            case Qt.Key_T: return "T"
            case Qt.Key_U: return "U"
            case Qt.Key_V: return "V"
            case Qt.Key_W: return "W"
            case Qt.Key_X: return "X"
            case Qt.Key_Y: return "Y"
            case Qt.Key_Z: return "Z"
            case Qt.Key_0: return "0"
            case Qt.Key_1: return "1"
            case Qt.Key_2: return "2"
            case Qt.Key_3: return "3"
            case Qt.Key_4: return "4"
            case Qt.Key_5: return "5"
            case Qt.Key_6: return "6"
            case Qt.Key_7: return "7"
            case Qt.Key_8: return "8"
            case Qt.Key_9: return "9"
            case Qt.Key_F1: return "F1"
            case Qt.Key_F2: return "F2"
            case Qt.Key_F3: return "F3"
            case Qt.Key_F4: return "F4"
            case Qt.Key_F5: return "F5"
            case Qt.Key_F6: return "F6"
            case Qt.Key_F7: return "F7"
            case Qt.Key_F8: return "F8"
            case Qt.Key_F9: return "F9"
            case Qt.Key_F10: return "F10"
            case Qt.Key_F11: return "F11"
            case Qt.Key_F12: return "F12"
            case Qt.Key_Space: return "Space"
            case Qt.Key_Enter: return "Enter"
            case Qt.Key_Return: return "Return"
            case Qt.Key_Tab: return "Tab"
            case Qt.Key_Backspace: return "Backspace"
            case Qt.Key_Delete: return "Delete"
            case Qt.Key_Insert: return "Insert"
            case Qt.Key_Home: return "Home"
            case Qt.Key_End: return "End"
            case Qt.Key_PageUp: return "PageUp"
            case Qt.Key_PageDown: return "PageDown"
            case Qt.Key_Up: return "↑"
            case Qt.Key_Down: return "↓"
            case Qt.Key_Left: return "←"
            case Qt.Key_Right: return "→"
            case Qt.Key_Escape: return ""
            default:
                console.log("Unknown key:", key)
                return ""
        }
    }

    onIsRecordingChanged: {
        if (!isRecording) {
            keyDetector.focus = false
        }
    }
}
