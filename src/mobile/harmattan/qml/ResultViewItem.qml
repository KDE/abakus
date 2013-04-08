import QtQuick 1.1

Row {
    id: root
    width: parent.width
    height: 40
    
    property int currentHistoryIndex: 0
    property int currentMinTagSize: 0

    property int fontPointSize: 24
    property string fontColorExpression: "lightgray"
    property string fontColorResult: "dodgerblue"
    property string fontColorTag: "lightgray"
    
    signal expressionSelected(string expression)
    signal resultSelected(string result)
    signal tagSelected(string tag)

    QtObject {
        id: internal

        property string rowBackground: "transparent"
        property string highlightColor: "transparent"
    }
    
    Rectangle {
        id: expressionItem
        width: parent.width - resultItem.width - tagItem.width
        height: parent.height
        color: model.index === currentHistoryIndex ? internal.highlightColor : internal.rowBackground
        
        Text {
            height: parent.height
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            color: fontColorExpression
            clip: true
            text: model.expression
            font.pointSize: root.fontPointSize
            
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
        color: model.index === currentHistoryIndex ? internal.highlightColor : internal.rowBackground
        
        Text {
            id: resultItemText
            height: parent.height
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            color: fontColorResult
            clip: true
            text: model.result
            font.pointSize: root.fontPointSize
            
            MouseArea  {
                anchors.fill: parent
                onClicked: root.resultSelected(model.result)
                onDoubleClicked: console.log("TODO")
            }
        }
    }
    
    Rectangle {
        id: tagItem
        width: model.tag === "" ? 0 : currentMinTagSize
        height: parent.height
        color: model.index === currentHistoryIndex ? internal.highlightColor : internal.rowBackground
        
        Text {
            height: parent.height
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            font.italic: true
            color: fontColorTag
            text: model.tag
            font.pointSize: root.fontPointSize
            
            MouseArea  {
                anchors.fill: parent
                onClicked: root.tagSelected(model.tag)
                onDoubleClicked: {}//do nothing
            }
        }
    }
}
