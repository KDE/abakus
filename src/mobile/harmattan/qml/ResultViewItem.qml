import QtQuick 1.1

Rectangle {
    id: root
    width: parent.width
    height: expression.width + result.width + tag.width > parent.width ? 2 * rowHeight : rowHeight
    color: model.index === currentHistoryIndex ? internal.highlightColor : internal.rowBackground
    
    property int currentHistoryIndex: 0
    property int currentMinTagSize: 0

    property int rowHeight: 40

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
    
    Text {
        id: expression
        height: root.rowHeight
        anchors.left: parent.left
        anchors.top: parent.top
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        color: fontColorExpression
        text: model.expression
        font.pointSize: root.fontPointSize

        MouseArea {
            anchors.fill: parent
            onClicked: root.expressionSelected(model.expression)
            onDoubleClicked: console.log("TODO")
        }
    }

    Text {
        id: result
        height: root.rowHeight
        anchors.right: tag.left
        anchors.bottom: parent.bottom
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        color: fontColorResult
        text: model.result
        font.pointSize: root.fontPointSize

        MouseArea  {
            anchors.fill: parent
            onClicked: root.resultSelected(model.result)
            onDoubleClicked: console.log("TODO")
        }
    }
    
    Text {
        id: tag
        width: model.tag === "" ? 0 : currentMinTagSize
        height: root.rowHeight
        anchors.right: parent.right
        anchors.bottom: parent.bottom
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
