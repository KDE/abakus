import QtQuick 1.1

Row {
    id: numeralViewItem
    width: parent.width
    height: 20
    
    signal numeralSelected(string numeralName)
    signal numeralRemoved(string numeralName)
    
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
            onClicked: numeralViewItem.numeralSelected(model.name)
            onDoubleClicked: console.log("TODO")
        }
    }
    
    Item {
        id: description
        width: parent.width - 50
        height: parent.height
        
        property bool itemHovered: false
        
        Item {
            anchors.fill: parent
            
            Text {
                anchors.fill: parent
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 12
                clip: true
                text: model.valueString
            }
            
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onClicked: numeralViewItem.numeralSelected(model.name)
                onDoubleClicked: console.log("TODO")
                onEntered: description.itemHovered = true
                onExited: description.itemHovered = false
            }
        }
        
        Rectangle { //TODO: replace with image
            width: parent.height
            height: parent.height
            anchors.right: parent.right
            color: removeItem.visible ? "red" : "transparent"
            
            Text {
                id: removeItem
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                visible: model.typeString == "User Variables" ? description.itemHovered : false
                text: "---"
                color: "white"
            }
            
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: description.itemHovered = true
                onExited: description.itemHovered = false
                onClicked: numeralViewItem.numeralRemoved(model.name)
            }
        }
    }
}