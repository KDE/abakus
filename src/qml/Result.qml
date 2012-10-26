import QtQuick 1.1

Row {
    width: parent.width
    height: 20

    Text {
        id: expressionItem
        width: parent.width * 0.5
        height: parent.height
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignHCenter
        font.pixelSize: 12
        color: "darkgrey"
        text: expression
        
        MouseArea {
            anchors.fill: parent
            onClicked: listView.expressionSelected(expression)
            onDoubleClicked: console.log("TODO")
        }
    }

    Text {
        id: resultItem
        //width: parent.width * 0.3
        width: parent.width - expressionItem.width - tagItem.width
        height: parent.height
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignHCenter
        font.pixelSize: 12
        text: result
        
        MouseArea  {
            anchors.fill: parent
            onClicked: listView.resultSelected(result)
            onDoubleClicked: console.log("TODO")
        }
    }

    Text {
        id: tagItem
        //width: parent.width * 0.2
        width: 36
        height: parent.height
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignHCenter
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
