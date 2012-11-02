import QtQuick 1.1

Row {
    width: parent.width
    height: 20
    
    Text {
        width: parent.width - 150
        height: parent.height
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 12
        clip: true
        text: name
        
        MouseArea {
            anchors.fill: parent
            onClicked: functions.functionSelected(name)
            onDoubleClicked: console.log("TODO")
        }
    }
    Text {
        height: parent.height
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 12
        clip: true
        text: value
        
        MouseArea {
            anchors.fill: parent
            onClicked: functions.functionSelected(name)
            onDoubleClicked: console.log("TODO")
        }
    }
}