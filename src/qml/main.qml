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
        width: parent.width - 200
        height: parent.height - trigMode.height - input.height
        anchors.top: trigMode.bottom
        anchors.left: parent.left
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

    ListView {
        id: numerals
        width: parent.width - history.width
        height: history.height
        anchors.top: trigMode.bottom
        anchors.right: parent.right
        //clip: true //TODO: check why visibility needs seconds to apply a change if this is set
        visible: true
        
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
        
        Connections {
            target: mainWindow
            
            onNumeralsVisibleChanged: numerals.visible = visible
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
