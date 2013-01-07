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

    PlasmaComponents.ButtonRow {
        id: configPanel
        height: configPanelHeight
        anchors.top: parent.top
        anchors.right: sidebar.left
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
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        
        onNumeralSelected: editor.text += numeralName
        onNumeralRemoved: mainWindow.removeNumeral(numeralName)
        
        onFunctionSelected: editor.text += functionName
        onFunctionRemoved: mainWindow.removeFunction(functionName)
        
        Connections {
            target: mainWindow
            
            onNumeralsVisibleChanged: {
                sidebar.numeralsVisibleChanged(visible)
            }
            
            onFunctionsVisibleChanged: {
                sidebar.functionsVisibleChanged(visible)
            }
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
