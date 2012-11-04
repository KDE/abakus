import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents

Item {
    id: baseItem
    width: 500
    height: 300

    property int sidebarWidth: 200
    property int sidebarTabBarHeight: 30
    property int trigModeHeight: 30
    property int inputHeight: 25

    PlasmaComponents.ButtonRow {
        id: trigMode
        height: trigModeHeight
        anchors.top: parent.top
        anchors.right: sidebar.left
        exclusive: true

        PlasmaComponents.ToolButton { id: degrees; height: parent.height; flat: true; text: "Degrees" }
        PlasmaComponents.ToolButton { id: radians; height: parent.height; flat: true; text: "Radians" }

        onCheckedButtonChanged: {
            if(degrees.checked) {
                mainWindow.setDegrees()
            }
            else {
                mainWindow.setRadians()
            }
        }

        Connections {
            target: mainWindow
            
            onTrigModeChanged: {
                if(degrees.checked && mode != 0) { //TODO: use the Abakus::TrigMode enum
                    radians.checked = true
                }
                else if(radians.checked && mode != 1) {
                    degrees.checked = true
                }
            }
        }
    }
    
    ListView {
        id: history
        anchors.top: trigMode.bottom
        anchors.left: parent.left
        anchors.right: sidebar.left
        anchors.bottom: input.top
        currentIndex: count -1
        clip: true
        
        signal expressionSelected( string expression )
        onExpressionSelected: input.text = expression
        
        signal resultSelected( string result )
        onResultSelected: input.text += result
        
        signal tagSelected( string tag )
        onTagSelected: input.text += tag
        
        property int minTagSize: 0
        
        model: resultModel
        delegate: ResultViewItem { }

        Connections {
            target: mainWindow
            
            onHistoryVisibleChanged: {
                history.visible = visible
                history.opacity = visible ? 1 : 0
            }
        }
    }

    Item {
        id: sidebar
        width: sidebarWidth
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        
        PlasmaComponents.TabBar {
            id: sidebarTabBar
            height: sidebarTabBarHeight
            width: parent.width
            anchors.right: parent.right
            anchors.top: parent.top
            PlasmaComponents.TabButton { id: numeralsTabButton; tab: numeralsTab; text: "Numerals" }
            PlasmaComponents.TabButton { id: functionsTabButton; tab: functionsTab; text: "Functions" }

            Connections {
                target: mainWindow
                
                onNumeralsVisibleChanged: {
                    if(!visible && sidebarTabBar.currentTab == numeralsTabButton && functionsTabButton.visible) {
                        sidebarTabBar.currentTab = functionsTabButton
                        sidebarTabGroup.currentTab = functionsTab    
                    }

                    if(visible) {
                        sidebarTabBar.currentTab = numeralsTabButton
                        sidebarTabGroup.currentTab = numeralsTab
                    }
                    
                    numeralsTabButton.visible = visible
                    numeralsTabButton.opacity = visible ? 1 : 0

                    if(numeralsTabButton.visible && functionsTabButton.visible) {
                        sidebarTabBar.anchors.bottom = undefined
                        sidebarTabBar.anchors.top = sidebar.top
                    }
                    else {
                        sidebarTabBar.anchors.top = undefined
                        sidebarTabBar.anchors.bottom = sidebar.top
                    }

                    if(numeralsTabButton.visible || functionsTabButton.visible) {
                        sidebar.anchors.left = undefined
                        sidebar.anchors.right = baseItem.right
                    }
                    else {
                        sidebar.anchors.right = undefined
                        sidebar.anchors.left = baseItem.right
                    }
                }

                onFunctionsVisibleChanged: {
                    if(!visible && sidebarTabBar.currentTab == functionsTabButton && numeralsTabButton.visible) {
                        sidebarTabBar.currentTab = numeralsTabButton
                        sidebarTabGroup.currentTab = numeralsTab
                        
                    }

                    if(visible) {
                        sidebarTabBar.currentTab = functionsTabButton
                        sidebarTabGroup.currentTab = functionsTab
                    }
                    
                    functionsTabButton.visible = visible
                    functionsTabButton.opacity = visible ? 1 : 0
                    
                    if(numeralsTabButton.visible && functionsTabButton.visible) {
                        sidebarTabBar.anchors.bottom = undefined
                        sidebarTabBar.anchors.top = sidebar.top
                    }
                    else {
                        sidebarTabBar.anchors.top = undefined
                        sidebarTabBar.anchors.bottom = sidebar.top
                    }
                    
                    if(numeralsTabButton.visible || functionsTabButton.visible) {
                        sidebar.anchors.left = undefined
                        sidebar.anchors.right = baseItem.right
                    }
                    else {
                        sidebar.anchors.right = undefined
                        sidebar.anchors.left = baseItem.right
                    }
                }
            }
        }

        PlasmaComponents.TabGroup {
            id: sidebarTabGroup
            width: sidebarWidth
            anchors.top: sidebarTabBar.bottom
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            PlasmaComponents.Page {
                id: numeralsTab
                anchors.fill: parent

                ListView {
                    id: numerals
                    anchors.fill: parent
                    clip: true

                    signal numeralSelected( string numeral )
                    onNumeralSelected: input.text += numeral

                    model: numeralModel
                    delegate: NumeralViewItem { }

                    section.property: "typeString"
                    section.criteria: ViewSection.FullString
                    section.delegate: Rectangle {
                        color: "lightsteelblue"
                        width: parent.width
                        height: 20
                        Text {
                            anchors.centerIn: parent
                            font.pixelSize: 12
                            text: section
                        }
                    }
                }
            }

            PlasmaComponents.Page {
                id: functionsTab
                anchors.fill: parent

                ListView {
                    id: functions
                    anchors.fill: parent
                    clip: true

                    signal functionSelected( string functionName )
                    onFunctionSelected: input.text += functionName

                    model: functionModel
                    delegate: FunctionViewItem { }

                    section.property: "typeString"
                    section.criteria: ViewSection.FullString
                    section.delegate: Rectangle {
                        color: "lightsteelblue"
                        width: parent.width
                        height: 20
                        Text {
                            anchors.centerIn: parent
                            font.pixelSize: 12
                            text: section
                        }
                    }
                }
            }
        }
    }

    PlasmaComponents.TextField {
        id: input
        height: inputHeight
        anchors.left: parent.left
        anchors.right: sidebar.left
        anchors.bottom: parent.bottom
        font.pixelSize: 12
        clearButtonShown: true
        placeholderText: qsTr("Enter expression")

        onAccepted: {
            mainWindow.slotEvaluate(text)
            input.selectAll()
        }
        
        onTextChanged: mainWindow.slotTextChanged(text)
        
        Connections {
            target: mainWindow
            
            onSetInputText: input.text = inputText
        }
    }

    Component.onCompleted: input.forceActiveFocus()
}
