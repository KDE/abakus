import QtQuick 1.1

Row {
    id: root
    width: parent.width
    height: 20
    
    property string rowBackground: model.index % 2 == 0 ? "white" : "whitesmoke"
    property string highlightColor: "lightblue"
    
    property int currentHistoryIndex: 0
    property int currentMinTagSize: 0
    
    signal expressionSelected(string expression)
    signal resultSelected(string result)
    signal tagSelected(string tag)
    
    Rectangle {
        id: expressionItem
        width: parent.width - resultItem.width - tagItem.width
        height: parent.height
        color: model.index == currentHistoryIndex ? highlightColor : rowBackground
        
        Text {
            height: parent.height
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 12
            color: "darkgrey"
            clip: true
            text: model.expression
            
            MouseArea {
                anchors.fill: parent
                onClicked: root.expressionSelected(model.expression)
                onDoubleClicked: console.log("TODO")
            }
        }
    }

    Rectangle {
        id: resultItem
        width: resultItemText.width
        height: parent.height
        color: model.index == currentHistoryIndex ? highlightColor : rowBackground
        
        Text {
            id: resultItemText
            height: parent.height
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 12
            clip: true
            text: model.result
            
            MouseArea  {
                anchors.fill: parent
                onClicked: root.resultSelected(model.result)
                onDoubleClicked: console.log("TODO")
            }
        }
    }
    
    Rectangle {
        id: tagItem
        width: model.tag == "" ? 0 : currentMinTagSize
        height: parent.height
        color: model.index == currentHistoryIndex ? highlightColor : rowBackground
        
        Text {
            height: parent.height
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 12
            font.italic: true
            color: "darkgrey"
            text: model.tag
            
            MouseArea  {
                anchors.fill: parent
                onClicked: root.tagSelected(model.tag)
                onDoubleClicked: {}//do nothing
            }
        }
    }
}
