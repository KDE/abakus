import QtQuick 1.1
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.extras 0.1 as PlasmaExtras

Item {
    id: baseItem
    
    property int sidebarWidth: sidebar.width
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
            height: parent.height
            anchors.left: parent.left
            anchors.top: parent.top
            buttonHeight: toolbarHeight
            mainWindowObject: mainWindow
            
            onSettingsPanelVisibleChanged: toolbar.state = settingsVisible ? "settingsVisible" : ""
        }
        
        TrigonometricMode {
            id: trigMode
            height: toolbarHeight
            anchors.right: sidebarGrip.left
            anchors.rightMargin: 5
            anchors.top: parent.top
            mainWindowObject: mainWindow
        }
        
        SidebarGrip {
            id: sidebarGrip
            width: 15
            height: toolbarHeight
            anchors.right: parent.right
            anchors.top: parent.top
            sidebarItem: sidebar
            
            onToggleSidebar: mainWindow.slotToggleMathematicalSidebar()
            
            onSidebarWidthChanged: mainWindow.mathematicalSidebarWidthChanged(newWidth)
        }
        
        states: State {
            name: "settingsVisible"
            PropertyChanges { target: toolbar; height: toolbarHeight + configPanel.settingsPanelHeigth }
        }
        
        transitions: Transition {
            NumberAnimation { property: "height"; duration: 100; easing.type: Easing.InOutQuad }
        }
    }
    
    PlasmaCore.FrameSvgItem {
        id: historyBase
        anchors.top: toolbar.bottom
        anchors.left: parent.left
        anchors.right: sidebar.left
        anchors.bottom: editor.top
        visible: !configPanel.copactModeActive
        
        imagePath: "widgets/frame"
        prefix: "sunken"
        
        //PlasmaExtras.ScrollArea {
        Item {
            anchors.fill: parent
            anchors.margins: 2
            
            ListView {
                id: history
                anchors.fill: parent
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
                
                Component.onCompleted: history.positionViewAtEnd()
            }
        }
    }
    
    MathSidebar {
        id: sidebar
        width: 200
        tabBarHeight: sidebarTabBarHeight
        anchors.top: parent.top
        anchors.left: parent.right
        anchors.bottom: parent.bottom
        
        onCurrentMathSidebarTabChanged: mainWindow.mathematicalSidebarActiveTabChanged(tabString)
        
        onNumeralSelected: editor.text += numeralName
        onNumeralRemoved: mainWindow.removeNumeral(numeralName)
        
        onFunctionSelected: editor.text += functionName
        onFunctionRemoved: mainWindow.removeFunction(functionName)
        
        onRejectFocus: editor.focus = true
        
        Connections {
            target: mainWindow
            
            onMathematicalSidebarVisibleChanged: {
                if(visible) {
                    sidebar.state = "shown"
                    sidebarGrip.sidebarShown = true
                }
                else {
                    sidebar.state = ""
                    sidebarGrip.sidebarShown = false
                }
            }
            
            onSetMathematicalActiveTab: sidebar.activeTab = activeTab
            
            onSetMathematicalSidebarWidth: sidebar.width = width
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
        focus: true
        mainWindowObject: mainWindow
    }
    
    Component.onCompleted: {
        mainWindow.applySettings()
        editor.forceActiveFocus()
    }
}