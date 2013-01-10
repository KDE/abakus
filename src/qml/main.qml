import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents

Item {
    id: baseItem
    width: 500
    height: 300

    property int sidebarWidth: 200
    property int sidebarTabBarHeight: 25
    property int toolbarHeight: 25
    property int editorHeight: 25

    Item {
        id: toolbar
        height: toolbarHeight
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: sidebar.left
        
        ConfigPanel {
            id: configPanel
            width: parent.width
            height: toolbarHeight
            anchors.left: parent.left
            anchors.bottom: parent. bottom
            mainWindowObject: mainWindow
            
            onSettingsPanelVisibleChanged: toolbar.state = settingsVisible ? "settingsVisible" : ""
        }
        
        TrigonometricMode {
            id: trigMode
            height: toolbarHeight
            anchors.right: sidebarGrip.left
            anchors.rightMargin: 5
            anchors.bottom: parent.bottom
            mainWindowObject: mainWindow
        }
        
        SidebarGrip {
            id: sidebarGrip
            width: 15
            height: toolbarHeight
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            text: sidebar.sidebarGripSign
            
            onToggleSidebar: mainWindow.slotToggleMathematicalSidebar()
        }
        
        states: State {
            name: "settingsVisible"
            PropertyChanges { target: toolbar; height: toolbarHeight + configPanel.settingsPanelHeigth }
        }
        
        transitions: Transition {
            NumberAnimation { property: "height"; duration: 100; easing.type: Easing.InOutQuad }
        }
    }
    
    ListView {
        id: history
        anchors.top: toolbar.bottom
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
            
            onExpressionSelected: editor.text = expression
            onResultSelected: editor.text += result
            onTagSelected: editor.text += tag
            
            Component.onCompleted: {
                mainWindow.addVisibleHistoryItemIndex(model.index)
                history.updateTagWidth()
            }
            
            Component.onDestruction: {
                mainWindow.removeVisibleHistoryItemIndex(model.index)
                history.updateTagWidth()
            }
        }
        
        Text {
            id: tagItemDummy
            font.pixelSize: 12
            font.italic: true
            visible: false
        }
        
        function updateTagWidth() {
            var i = 0
            var itemIndexNext = mainWindow.getVisibleHistoryItemIndex(i)
            var minTagSize = 0
            
            while(itemIndexNext > -1)
            {
                tagItemDummy.text = mainWindow.getTag(itemIndexNext)
                if(minTagSize < tagItemDummy.width)
                {
                    minTagSize = tagItemDummy.width
                }
                ++i
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
            AnchorAnimation { duration: 100; easing.type: Easing.InOutQuad }
        }
    }

    Editor {
        id: editor
        height: editorHeight
        anchors.left: parent.left
        anchors.right: sidebar.left
        anchors.bottom: parent.bottom
        mainWindowObject: mainWindow
    }

    Component.onCompleted: editor.forceActiveFocus()
}
