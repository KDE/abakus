import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents

Item {
    id: baseItem
    width: 500
    height: 300
    
    ListView {
        id: history
        width: parent.width
        height: parent.height - input.height
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        currentIndex: count -1
        
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
        highlight: Rectangle {
            width: baseItem.width
            color: "lightblue"
            
            opacity: 0.5
            z: 10
        }
        highlightFollowsCurrentItem: true
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
