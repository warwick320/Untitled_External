import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import Qt5Compat.GraphicalEffects

ApplicationWindow {
    id: appWindow
    visible: true
    width: 1000
    height: 500
    title: qsTr("Untitiled External @warwick.320")
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.Window

    property int sidebarWidthExpanded: 220
    property int sidebarWidthCollapsed: 80
    property int btnHeight: 48
    property int btnSpacing: 12
    property int sidePadding: 16
    property int titleBarHeight: 36
    property int targetWidth: 1000
    property int windowRadius: 12

    property color accentColor: "#C62828"
    property color backgroundColor: "#0f0f0f"
    property color macRed:    "#FF605C"
    property color macGreen:  "#00CA4E"
    property color macYellow: "#FFBD44"

    property bool closingInProgress: false
    property bool firstShowDone: false

    property string currentTab: "aim"

    Component.onCompleted: {
        appWindow.width = 100
        appWindow.opacity = 0
        expandAnimation.start()
        firstShowDone = true
    }

    onVisibilityChanged: function() {
        if ((appWindow.visibility === Window.Windowed || appWindow.visibility === Window.Maximized) && firstShowDone) {
            appWindow.opacity = 0
            fadeInWindow.start()
        }
    }

    onClosing: function(ev) {
        if (!closingInProgress) {
            ev.accepted = false
            closingInProgress = true
            collapseAnimation.start()
        }
    }

    function toggleMaximize() {
        if (visibility === Window.Maximized) showNormal(); else showMaximized()
    }

    ParallelAnimation {
        id: expandAnimation
        NumberAnimation {
            target: appWindow
            property: "width"
            to: appWindow.targetWidth
            duration: 450
            easing.type: Easing.OutQuart
        }
        NumberAnimation {
            target: appWindow
            property: "opacity"
            to: 1
            duration: 350
            easing.type: Easing.OutQuad
        }
        SequentialAnimation {
            PauseAnimation { duration: 100 }
            NumberAnimation {
                target: appWindow
                property: "height"
                from: appWindow.height * 0.95
                to: appWindow.height
                duration: 200
                easing.type: Easing.OutQuad
            }
        }
    }

    NumberAnimation {
        id: fadeInWindow
        target: appWindow
        property: "opacity"
        to: 1
        duration: 220
        easing.type: Easing.InOutQuad
    }

    ParallelAnimation {
        id: collapseAnimation
        NumberAnimation {
            target: appWindow
            property: "width"
            to: 50
            duration: 200
            easing.type: Easing.InQuart
        }
        NumberAnimation {
            target: appWindow
            property: "opacity"
            to: 0
            duration: 250
            easing.type: Easing.InQuad
        }
        onStopped: Qt.quit()
    }

    SequentialAnimation {
        id: minimizeAnim
        PropertyAction { target: contentArea; property: "scale"; value: 1 }
        PropertyAction { target: contentArea; property: "opacity"; value: 1 }
        ParallelAnimation {
            NumberAnimation { target: contentArea; property: "scale"; to: 0.98; duration: 120; easing.type: Easing.OutCubic }
            NumberAnimation { target: appWindow;   property: "opacity"; to: 0.0;  duration: 140; easing.type: Easing.OutCubic }
        }
        ScriptAction { script: appWindow.showMinimized() }
    }

    ListModel {
        id: tabsModel
        ListElement { name: "Aim"; tabId: "aim" }
        ListElement { name: "Visual"; tabId: "visual" }
        ListElement { name: "Misc"; tabId: "misc" }
    }

    property int currentSidebarIndex: 0

    Rectangle {
        id: mainContainer
        anchors.fill: parent
        anchors.margins: 8
        radius: appWindow.windowRadius
        color: appWindow.backgroundColor
        clip: true

        layer.enabled: true
        layer.effect: DropShadow {
            horizontalOffset: 0
            verticalOffset: 4
            radius: 16
            color: "#40000000"
            samples: 17
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            Rectangle {
                id: titleBar
                Layout.fillWidth: true
                Layout.preferredHeight: appWindow.titleBarHeight
                color: appWindow.backgroundColor

                RowLayout {
                    id: titleBarContent
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 8

                    Row {
                        id: macButtons
                        Layout.preferredWidth: 96
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                        spacing: 8

                        ListModel {
                            id: customTitileBtn
                            ListElement { action: "close" }
                            ListElement { action: "maximize" }
                            ListElement { action: "minimize" }
                        }

                        Repeater {
                            model: customTitileBtn
                            delegate: Item {
                                width: 20; height: 20
                                property bool hovered: false
                                property bool pressed: false
                                property color dotColor: action === "close"
                                                         ? appWindow.macRed
                                                         : (action === "maximize" ? appWindow.macGreen
                                                                                  : appWindow.macYellow)

                                Rectangle {
                                    anchors.centerIn: parent
                                    width: 12; height: 12; radius: 6
                                    color: hovered ? Qt.lighter(dotColor, 1.15) : dotColor
                                    scale: pressed ? 0.92 : 1.0
                                    Behavior on scale { NumberAnimation { duration: 90; easing.type: Easing.OutQuad } }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onEntered: parent.hovered = true
                                    onExited: parent.hovered = false
                                    onPressed: parent.pressed = true
                                    onReleased: {
                                        parent.pressed = false
                                        if (action === "close") appWindow.close()
                                        else if (action === "maximize") appWindow.toggleMaximize()
                                        else if (action === "minimize") minimizeAnim.start()
                                    }
                                }
                            }
                        }
                    }

                    Text {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        text: appWindow.title
                        color: "#f0f0f0"
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                        font.pixelSize: 14
                    }
                }

                MouseArea {
                    id: dragArea
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.left: parent.left
                    anchors.leftMargin: macButtons.width + 24
                    acceptedButtons: Qt.LeftButton
                    onPressed: appWindow.startSystemMove()
                    onDoubleClicked: appWindow.toggleMaximize()
                }
            }

            Item {
                id: contentArea
                Layout.fillWidth: true
                Layout.fillHeight: true

                Rectangle {
                    id: sidebar
                    width: sidebar.expanded ? appWindow.sidebarWidthExpanded : appWindow.sidebarWidthCollapsed
                    height: parent.height
                    anchors.left: parent.left
                    anchors.top: parent.top
                    color: appWindow.backgroundColor
                    z: 10

                    property bool expanded: false

                    Behavior on width {
                        NumberAnimation {
                            duration: 250
                            easing.type: Easing.InOutCubic
                        }
                    }

                    Timer {
                        id: collapseTimer
                        interval: 300
                        onTriggered: {
                            if (sidebar) {
                                sidebar.expanded = false
                            }
                        }
                    }

                    HoverHandler {
                        id: sidebarHover
                        onHoveredChanged: {
                            if (hovered) {
                                collapseTimer.stop()
                                sidebar.expanded = true
                            } else {
                                collapseTimer.restart()
                            }
                        }
                    }

                    Item {
                        anchors.fill: parent
                        clip: true

                        Rectangle {
                            id: indicator
                            width: 4
                            color: appWindow.accentColor
                            radius: 2
                            x: 0

                            height: sidebar.expanded ? appWindow.btnHeight : 40
                            y: {
                                const buttonHeight = sidebar.expanded ? appWindow.btnHeight : 40
                                const spacing = sidebar.expanded ? appWindow.btnSpacing : appWindow.btnSpacing
                                return appWindow.sidePadding + appWindow.currentSidebarIndex * (buttonHeight + spacing)
                            }

                            Behavior on y {
                                NumberAnimation {
                                    duration: 250
                                    easing.type: Easing.InOutCubic
                                }
                            }

                            Behavior on height {
                                NumberAnimation {
                                    duration: 250
                                    easing.type: Easing.InOutCubic
                                }
                            }
                        }

                        Column {
                            id: tabColumn
                            anchors {
                                top: parent.top
                                left: parent.left
                                right: parent.right
                                margins: appWindow.sidePadding
                            }
                            spacing: appWindow.btnSpacing

                            Repeater {
                                model: tabsModel
                                delegate: Rectangle {
                                    id: btnRect
                                    width: sidebar.expanded ? parent.width : 40
                                    height: sidebar.expanded ? appWindow.btnHeight : 40
                                    anchors.horizontalCenter: sidebar.expanded ? undefined : parent.horizontalCenter
                                    anchors.left: sidebar.expanded ? parent.left : undefined
                                    radius: sidebar.expanded ? 8 : 6
                                    color: "transparent"

                                    property bool hovered: false
                                    property bool pressed: false

                                    Behavior on width {
                                        NumberAnimation {
                                            duration: 250
                                            easing.type: Easing.InOutCubic
                                        }
                                    }
                                    Behavior on height {
                                        NumberAnimation {
                                            duration: 250
                                            easing.type: Easing.InOutCubic
                                        }
                                    }
                                    Behavior on radius {
                                        NumberAnimation {
                                            duration: 250
                                            easing.type: Easing.InOutCubic
                                        }
                                    }

                                    Rectangle {
                                        id: selectedBackground
                                        width: sidebar.expanded ? parent.parent.width : 40
                                        height: sidebar.expanded ? appWindow.btnHeight : 40
                                        anchors.horizontalCenter: sidebar.expanded ? undefined : parent.horizontalCenter
                                        anchors.left: sidebar.expanded ? parent.left : undefined
                                        anchors.verticalCenter: parent.verticalCenter
                                        radius: sidebar.expanded ? 8 : 6
                                        color: "#161616"

                                        opacity: appWindow.currentTab === model.tabId ? 1.0 : 0.0

                                        Behavior on width {
                                            NumberAnimation {
                                                duration: 250
                                                easing.type: Easing.InOutCubic
                                            }
                                        }
                                        Behavior on height {
                                            NumberAnimation {
                                                duration: 250
                                                easing.type: Easing.InOutCubic
                                            }
                                        }
                                        Behavior on radius {
                                            NumberAnimation {
                                                duration: 250
                                                easing.type: Easing.InOutCubic
                                            }
                                        }
                                        Behavior on opacity {
                                            NumberAnimation {
                                                duration: 150
                                                easing.type: Easing.InOutQuad
                                            }
                                        }
                                    }

                                    Rectangle {
                                        anchors.fill: parent
                                        radius: parent.radius
                                        color: appWindow.accentColor
                                        opacity: pressed ? 0.12 : (hovered ? 0.06 : 0.0)
                                        Behavior on opacity { NumberAnimation { duration: 100 } }
                                    }

                                    scale: pressed ? 0.98 : (hovered ? 1.01 : 1.0)
                                    Behavior on scale { NumberAnimation { duration: 90; easing.type: Easing.OutQuad } }

                                    Item {
                                        anchors.fill: parent

                                        Text {
                                            id: iconText
                                            text: model.name.substring(0, 1)
                                            anchors.centerIn: parent
                                            font.pixelSize: 20
                                            font.bold: true
                                            color: appWindow.currentTab === model.tabId ? "white" : "#bbbbbb"

                                            opacity: sidebar.expanded ? 0.0 : 1.0
                                            visible: opacity > 0.01

                                            Behavior on opacity {
                                                NumberAnimation {
                                                    duration: 200
                                                    easing.type: Easing.InOutQuad
                                                }
                                            }
                                        }

                                        Text {
                                            id: nameText
                                            text: model.name
                                            anchors.verticalCenter: parent.verticalCenter
                                            font.pixelSize: 16
                                            color: appWindow.currentTab === model.tabId ? "white" : "#bdbdbd"

                                            anchors.horizontalCenter: sidebar.expanded ? undefined : parent.horizontalCenter
                                            anchors.left: sidebar.expanded ? parent.left : undefined
                                            anchors.leftMargin: sidebar.expanded ? 22 : 0

                                            opacity: sidebar.expanded ? 1.0 : 0.0

                                            Behavior on opacity {
                                                NumberAnimation {
                                                    duration: 200
                                                    easing.type: Easing.InOutQuad
                                                }
                                            }

                                            Behavior on anchors.leftMargin {
                                                NumberAnimation {
                                                    duration: 250
                                                    easing.type: Easing.InOutCubic
                                                }
                                            }
                                        }
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        hoverEnabled: true

                                        onEntered: btnRect.hovered = true
                                        onExited: btnRect.hovered = false
                                        onPressed: btnRect.pressed = true
                                        onCanceled: btnRect.pressed = false
                                        onReleased: {
                                            btnRect.pressed = false
                                            if (appWindow.currentTab !== model.tabId) {
                                                appWindow.currentSidebarIndex = index
                                                appWindow.currentTab = model.tabId
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                } // sidebar

                // --- AIM TAB ---
                Rectangle {
                    id: aimTab
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    x: sidebar.width
                    width: parent.width - sidebar.width
                    color: "#141414"

                    visible: appWindow.currentTab === "aim"
                    opacity: visible ? 1.0 : 0.0

                    Behavior on opacity {
                        NumberAnimation {
                            duration: 200
                            easing.type: Easing.InOutQuad
                        }
                    }

                    ScrollView {
                        anchors.fill: parent
                        anchors.margins: 0
                        clip: true

                        ScrollBar.vertical.policy: ScrollBar.AlwaysOff
                        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                        Column {
                            width: parent.width
                            spacing: 0

                            Item {
                                width: parent.width
                                height: childrenRect.height + 30

                                Column {
                                    width: parent.width - 40
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.top: parent.top
                                    anchors.topMargin: 15
                                    spacing: 12

                                    ToggleItem {
                                        title: "Aimbot"
                                        description: "Enable or Disable Aimbot functionality"
                                        toggleState: appController.aimbotEnabled
                                        accentColor: appWindow.accentColor

                                        onToggled: function(state) {
                                            console.log("Aimbot toggled:", state)
                                            appController.aimbotEnabled = state
                                        }
                                    }
                                    KeyBindItem {
                                        title: "Toggle Key"
                                        description: "Keybind"
                                        currentKeybind: appController.aimbotKeybind
                                        accentColor: appWindow.accentColor
    
                                        onKeybindChanged: function(newKeybind) {
                                            console.log("Changed to:", newKeybind)
                                            appController.setAimbotKeybind(newKeybind) 
                                        }
                                    }
                                    
                                    DropBox {
                                        id: aimbotTypeDrop
                                        title: "Aimbot Type"
                                        description: "Select Targetting Type"
                                        items: ["Legit", "Sticky", "Custom"]
                                        accentColor: appWindow.accentColor
                                        visible: appController.aimbotEnabled
                                        opacity: visible ? 1.0 : 0.0
                                        Behavior on opacity {
                                            NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
                                        }
                                        Behavior on Layout.preferredHeight {
                                            NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
                                        }


                                        selectedIndex: (typeof appController.aimbotType !== "undefined") ? appController.aimbotType : 0

                                        onValueChanged: function(idx, value) {
                                            console.log("Aimbot Type changed:", idx, value)
                                            if (typeof appController.aimbotType !== "undefined")
                                                appController.aimbotType = idx
                                        }
                                    }

                                    SliderItem {
                                        title: "Aimbot FOV Size"
                                        description: "Adjust the Field of View range for aimbot targeting"
                                        sliderValue: appController.fovSize
                                        minValue: 10.0
                                        maxValue: 400.0
                                        accentColor: appWindow.accentColor

                                        onValueChanged: function(value) {
                                            console.log("FOV Size changed:", value)
                                            appController.fovSize = value
                                        }
                                    }
                                    SliderItem {
                                        title: "Aimbot Smooth"
                                        description: "Adjust smoothness for custom aimbot"
                                        sliderValue: appController.smooth
                                        minValue: 1.0
                                        maxValue: 25.0
                                        stepSize: 0.1  
                                        precision: 1  
                                        accentColor: appWindow.accentColor
    
                                        visible: appController.aimbotEnabled && appController.aimbotType === 2 
                                        opacity: visible ? 1.0 : 0.0
                                        Layout.preferredHeight: visible ? 90 : 0
    
                                        Behavior on opacity {
                                            NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
                                        }
                                        Behavior on Layout.preferredHeight {
                                            NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
                                        }

                                        onValueChanged: function(value) {
                                            console.log("Smooth changed:", value)
                                            appController.smooth = value
                                        }
                                    }
                                    ToggleItem {
                                        title: "Trigger Bot"
                                        description: "Auto shoot(+ wallcheck!)"
                                        toggleState: appController.triggerbotEnabled
                                        accentColor: appWindow.accentColor

                                        onToggled: function(state) {
                                            console.log("TriggerBot toggled:", state)
                                            appController.triggerbotEnabled = state
                                        }
                                    }
                                    Item {
                                        width: parent.width
                                        height: 15
                                    }
                                }
                            }
                        }
                    }
                } // aimTab

                // --- VISUAL TAB ---
                Rectangle {
                    id: visualTab
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    x: sidebar.width
                    width: parent.width - sidebar.width
                    color: "#141414"

                    visible: appWindow.currentTab === "visual"
                    opacity: visible ? 1.0 : 0.0

                    Behavior on opacity {
                        NumberAnimation {
                            duration: 200
                            easing.type: Easing.InOutQuad
                        }
                    }

                    ScrollView {
                        anchors.fill: parent
                        anchors.margins: 0
                        clip: true

                        ScrollBar.vertical.policy: ScrollBar.AlwaysOff
                        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                        Column {
                            width: parent.width
                            spacing: 0

                            Item {
                                width: parent.width
                                height: childrenRect.height + 30

                                Column {
                                    width: parent.width - 40
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.top: parent.top
                                    anchors.topMargin: 15
                                    spacing: 12

                                    ToggleItem {
                                        title: "ESP (Base)"
                                        description: "See enemies through walls and obstacles"
                                        toggleState: appController.espEnabled
                                        accentColor: appWindow.accentColor

                                        onToggled: function(state) {
                                            console.log("ESP toggled:", state)
                                            appController.espEnabled = state
                                        }
                                    }
                                    ToggleItem {
                                        title: "Chams"
                                        description: "Display player Chams"
                                        toggleState: appController.espCharms
                                        accentColor: appWindow.accentColor

                                        onToggled: function(state) {
                                            console.log("ESP Chams toggled:", state)
                                            appController.espCharms = state
                                        }
                                    }
                                    ToggleItem {
                                        title: "Player Names"
                                        description: "Display enemy player names above heads"
                                        toggleState: appController.espShowNames
                                        accentColor: appWindow.accentColor

                                        onToggled: function(state) {
                                            console.log("ESP Names toggled:", state)
                                            appController.espShowNames = state
                                        }
                                    }

                                    ToggleItem {
                                        title: "Bounding Boxes"
                                        description: "Show rectangular boxes around enemies"
                                        toggleState: appController.espShowBox
                                        accentColor: appWindow.accentColor

                                        onToggled: function(state) {
                                            console.log("ESP Box toggled:", state)
                                            appController.espShowBox = state
                                        }
                                    }

                                    ToggleItem {
                                        title: "Skeleton"
                                        description: "Display enemy skeleton/bones"
                                        toggleState: appController.espShowBones
                                        accentColor: appWindow.accentColor

                                        onToggled: function(state) {
                                            console.log("ESP Bones toggled:", state)
                                            appController.espShowBones = state
                                        }
                                    }

                                    ToggleItem {
                                        title: "Distance Info"
                                        description: "Show distance to enemies"
                                        toggleState: appController.espShowDistance
                                        accentColor: appWindow.accentColor

                                        onToggled: function(state) {
                                            console.log("ESP Distance toggled:", state)
                                            appController.espShowDistance = state
                                        }
                                    }

                                    ToggleItem {
                                        title: "Tracer"
                                        description: "Tracer"
                                        toggleState: appController.espShowTracer
                                        accentColor: appWindow.accentColor

                                        onToggled: function(state) {
                                            console.log("ESP Tracer toggled:", state)
                                            appController.espShowTracer = state
                                        }
                                    }

                                    Item {
                                        width: parent.width
                                        height: 15
                                    }
                                }
                            }
                        }
                    }
                } // visualTab

                // --- MISC TAB ---
                Rectangle {
                    id: miscTab
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    x: sidebar.width
                    width: parent.width - sidebar.width
                    color: "#141414"

                    visible: appWindow.currentTab === "misc"
                    opacity: visible ? 1.0 : 0.0

                    Behavior on opacity {
                        NumberAnimation {
                            duration: 200
                            easing.type: Easing.InOutQuad
                        }
                    }

                    ScrollView {
                        anchors.fill: parent
                        anchors.margins: 0
                        clip: true

                        ScrollBar.vertical.policy: ScrollBar.AlwaysOff
                        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                        Column {
                            width: parent.width
                            spacing: 0

                            Item {
                                width: parent.width
                                height: childrenRect.height + 30

                                Column {
                                    width: parent.width - 40
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.top: parent.top
                                    anchors.topMargin: 15
                                    spacing: 12

                                    SliderItem {
                                        title: "Speed"
                                        description: "Adjust movement speed multiplier"
                                        sliderValue: appController.targetSpeed
                                        minValue: 16.0
                                        maxValue: 500.0
                                        accentColor: appWindow.accentColor

                                        onValueChanged: function(value) {
                                            console.log("Speed changed:", value)
                                            appController.targetSpeed = value
                                        }
                                    }

                                    SliderItem {
                                        title: "Jump Power"
                                        description: "Adjust jump power multiplier"
                                        sliderValue: appController.jumpPower
                                        minValue: 50.0
                                        maxValue: 500.0
                                        accentColor: appWindow.accentColor

                                        onValueChanged: function(value) {
                                            console.log("Jump Power changed:", value)
                                            appController.jumpPower = value
                                        }
                                    }

                                    Item {
                                        width: parent.width
                                        height: 15
                                    }
                                }
                            }
                        }
                    }
                } // miscTab

            } // Item contentArea

        } // ColumnLayout
    } // Rectangle mainContainer
} // ApplicationWindow
