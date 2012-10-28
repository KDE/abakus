import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents

Item {
    id: baseItem
    width: 500
    height: 300

    PlasmaComponents.TabBar {
        id: trigMode
        height: 30
        anchors.right: parent.right
        z: 10
        
        PlasmaComponents.TabButton {
            id: degrees
            text: "Degrees"
        }
        PlasmaComponents.TabButton {
            id: radians
            text: "Radians"
        }

        onCurrentTabChanged: {
            if(currentTab == degrees) {
                mainWindow.setDegrees()
            }
            else {
                mainWindow.setRadians()
            }
        }
        
        Connections {
            target: mainWindow
            
            onTrigModeChanged: {
                if(trigMode.currentTab == degrees && mode != 0) { //TODO: use the Abakus::TrigMode enum
                    trigMode.currentTab = radians
                }
                else if(trigMode.currentTab == radians && mode != 1) {
                    trigMode.currentTab = degrees
                }
            }
        }
    }
    
    ListView {
        id: history
        width: parent.width
        height: parent.height - trigMode.height - input.height
        anchors.top: trigMode.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        currentIndex: count -1
        visible: true
        
        signal expressionSelected( string expression )
        onExpressionSelected: input.text = expression
        
        signal resultSelected( string result )
        onResultSelected: input.text += result
        
        signal tagSelected( string tag )
        onTagSelected: input.text += tag
        
        property int minTagSize: 0
        property int indexTop: 0
        property int indexBottom: 0
        
        model: resultModel
        delegate: Result { }

        Connections {
            target: mainWindow
            
            onHistoryVisibleChanged: history.visible = visible
        }
    }

    PlasmaComponents.TextField {
        id: input
        width: parent.width
        height: 25
        anchors.horizontalCenter: parent.horizontalCenter
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
