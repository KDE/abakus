import QtQuick 1.1

Row {
    width: parent.width
    height: 20
    
    Text {
        width: parent.width - 100
        height: parent.height
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 12
        clip: true
        text: name
        
        MouseArea {
            anchors.fill: parent
            onClicked: numerals.numeralSelected(name)
            onDoubleClicked: console.log("TODO")
        }
    }
    Text {
        height: parent.height
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 12
        clip: true
        text: valueString
        
        MouseArea {
            anchors.fill: parent
            onClicked: numerals.numeralSelected(name)
            onDoubleClicked: console.log("TODO")
        }
    }
}