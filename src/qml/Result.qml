import QtQuick 1.1

Row {
    width: parent.width
    height: 20
    
    property string rowBackground: index % 2 == 0 ? "white" : "whitesmoke"

    Rectangle {
        id: expressionItem
        width: parent.width - resultItem.width - tagItem.width
        height: parent.height
        color: rowBackground
        
        Text {
            height: parent.height
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 12
            color: "darkgrey"
            text: expression
            
            MouseArea {
                anchors.fill: parent
                onClicked: listView.expressionSelected(expression)
                onDoubleClicked: console.log("TODO")
            }
        }
    }

    Rectangle {
        id: resultItem
        width: resultItemText.width
        height: parent.height
        color: rowBackground
        
        Text {
            id: resultItemText
            height: parent.height
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 12
            text: result
            
            MouseArea  {
                anchors.fill: parent
                onClicked: listView.resultSelected(result)
                onDoubleClicked: console.log("TODO")
            }
        }
    }
    
    Rectangle {
        id: tagItem
        width: tag == "" ? 0 : 25
        height: parent.height
        color: rowBackground
        
        Text {
            height: parent.height
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 12
            font.italic: true
            color: "darkgrey"
            text: tag
            
            MouseArea  {
                anchors.fill: parent
                onClicked: listView.tagSelected(parent.text)
                onDoubleClicked: {}//do nothing
            }
        }
    }
}
