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
        
        signal expressionSelected( string expression )
        onExpressionSelected: editor.text = expression
        
        signal resultSelected( string result )
        onResultSelected: editor.text += result
        
        signal tagSelected( string tag )
        onTagSelected: editor.text += tag
        
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
