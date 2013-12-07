import QtQuick 2.0
import Sailfish.Silica 1.0

Row {
    id: root
    width: parent.width
    height: 40//theme.standardItemHeight
    
    property string rowBackground: "transparent"//model.index % 2 == 0 ? "white" : "whitesmoke"
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
        
        Label {
            height: parent.height
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
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
        
        Label {
            id: resultItemText
            height: parent.height
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
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
        
        Label {
            height: parent.height
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
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
