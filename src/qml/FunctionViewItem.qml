import QtQuick 1.1

Row {
    width: parent.width
    height: 20
    
    Text {
        width: 50
        height: parent.height
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 12
        clip: true
        text: model.name
        
        MouseArea {
            anchors.fill: parent
            onClicked: functions.functionSelected(model.name)
            onDoubleClicked: console.log("TODO")
        }
    }
    Text {
        height: parent.height
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 12
        clip: true
        text: model.value
        
        MouseArea {
            anchors.fill: parent
            onClicked: functions.functionSelected(model.name)
            onDoubleClicked: console.log("TODO")
        }
    }
}