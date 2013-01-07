import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents

Item {
    id: baseItem
    width: 500
    height: 300

    property int sidebarWidth: 200
    property int sidebarTabBarHeight: 25
    property int configPanelHeight: 25
    property int editorHeight: 25

    Item {
        id: configPanel
        height: configPanelHeight
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: sidebar.left
        
        PlasmaComponents.ButtonRow {
            id: toolbar
            height: configPanelHeight
            anchors.left: parent.left
            anchors.bottom: parent. bottom
            exclusive: false
            
            PlasmaComponents.ToolButton {
                id: settings;
                height: parent.height;
                flat: true;
                checkable: true;
                iconSource: "configure"
            }
            
            PlasmaComponents.ToolButton {
                id: compactMode;
                height: parent.height;
                flat: true;
                checkable:true;
                iconSource: "merge"
                
                onClicked: mainWindow.slotToggleCompactMode()
                
                Connections {
                    target: mainWindow
                    
                    onCompactModeChanged: compactMode.checked = active
                }
            }
            
            PlasmaComponents.ToolButton {
                id: help;
                height: parent.height;
                flat: true;
                iconSource: "help-about"
            }
        }
        
        PlasmaComponents.ButtonRow {
            id: trigMode
            height: configPanelHeight
            anchors.right: sidebarGrip.left
            anchors.bottom: parent.bottom
            exclusive: true

            PlasmaComponents.ToolButton { id: degrees; height: parent.height; flat: true; text: i18n("Degrees") }
            PlasmaComponents.ToolButton { id: radians; height: parent.height; flat: true; text: i18n("Radians") }

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
        
        Item {
            id: sidebarGrip
            width: 15
            height: configPanelHeight
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            opacity: 0.5
            clip: true
            
            Rectangle {
                width: parent.width + 10
                height: parent.height
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                color: "lightgrey"
                radius: 3
            }
            
            Text {
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: sidebar.sidebarGripSign
            }
            
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                
                onEntered: sidebarGrip.opacity = 1
                onExited: sidebarGrip.opacity = 0.5
                
                onClicked: mainWindow.slotToggleMathematicalSidebar()
            }
        }
    }
    
    ListView {
        id: history
        anchors.top: configPanel.bottom
        anchors.left: parent.left
        anchors.right: sidebar.left
        anchors.bottom: editor.top
        currentIndex: count -1
        clip: true
        
        property int minTagSize: 0
        
        model: resultModel
        delegate: ResultViewItem {
            currentHistoryIndex: history.currentIndex
            currentMinTagSize: history.minTagSize
            
            onExpressionSelected: editor.text = expression;
            onResultSelected: editor.text += result
            onTagSelected: editor.text += tag
            
            Component.onCompleted: {
                mainWindow.addVisibleHistoryItemIndex(model.index)
                history.updateTagWidth();
            }
            
            Component.onDestruction: {
                mainWindow.removeVisibleHistoryItemIndex(model.index)
                history.updateTagWidth();
            }
        }
        
        Text {
            id: tagItemDummy
            font.pixelSize: 12
            font.italic: true
            visible: false
        }
        
        function updateTagWidth() {
            var i = 0;
            var itemIndexNext = mainWindow.getVisibleHistoryItemIndex(i)
            var minTagSize = 0
            
            while(itemIndexNext > -1)
            {
                tagItemDummy.text = mainWindow.getTag(itemIndexNext)
                if(minTagSize < tagItemDummy.width)
                {
                    minTagSize = tagItemDummy.width
                }
                ++i;
                itemIndexNext = mainWindow.getVisibleHistoryItemIndex(i)
            }
            
            tagItemDummy.text = ""
            history.minTagSize = minTagSize
        }

        Connections {
            target: mainWindow
            
            onHistoryVisibleChanged: {
                history.visible = visible
                history.opacity = visible ? 1 : 0
            }
        }
    }

    MathSidebar {
        id: sidebar
        width: sidebarWidth
        tabBarHeight: sidebarTabBarHeight
        anchors.top: parent.top
        anchors.left: parent.right
        anchors.bottom: parent.bottom
        
        property string sidebarGripSign: "<"
        
        onNumeralSelected: editor.text += numeralName
        onNumeralRemoved: mainWindow.removeNumeral(numeralName)
        
        onFunctionSelected: editor.text += functionName
        onFunctionRemoved: mainWindow.removeFunction(functionName)
        
        Connections {
            target: mainWindow
            
            onMathematicalSidebarVisibleChanged: {
                if(visible) {
                    sidebar.state = "shown"
                    sidebar.sidebarGripSign = ">"
                }
                else {
                    sidebar.state = ""
                    sidebar.sidebarGripSign = "<"
                }
            }
        }
        
        states: State {
            name: "shown"
            AnchorChanges { target: sidebar; anchors.left: undefined; anchors.right: baseItem.right }
        }
        
        transitions: Transition {
            AnchorAnimation { duration: 100 }
        }
    }

    Editor {
        id: editor
        height: editorHeight
        anchors.left: parent.left
        anchors.right: sidebar.left
        anchors.bottom: parent.bottom
        
        onTextChanged: mainWindow.slotTextChanged(text)
        onToEvaluate: mainWindow.slotEvaluate(text)
        onUpPressed: mainWindow.historyPrevious()
        onDownPressed: mainWindow.historyNext()
        
        Connections {
            target: mainWindow
            
            onSetEditorText: editor.text = editorText
        }
    }

    Component.onCompleted: editor.forceActiveFocus()
}
