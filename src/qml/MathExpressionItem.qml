import QtQuick 1.1

Item {
    id: root
    width: parent.width
    height: 20
    
    signal selected()
    signal removed()
    
    property string nameString
    property string valueString
    property string typeString
    property bool itemRemovable: typeString == "User Defined"
    property bool itemHovered: false
    
    Item {
        width: 50
        height: parent.height
        anchors.left: parent.left
        anchors.top: parent.top
        
        Text {
            anchors.fill: parent
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 12
            clip: true
            text: nameString
        }
        
        MouseArea {
            anchors.fill: parent
            hoverEnabled: root.itemRemovable
            onClicked: root.selected()
            onDoubleClicked: console.log("TODO")
            onEntered: root.itemHovered = true
            onExited: root.itemHovered = false
        }
    }
    
    Item {
        width: parent.width - 50
        height: parent.height
        anchors.right: parent.right
        anchors.top: parent.top
        
        Item {
            anchors.fill: parent
            
            Text {
                anchors.fill: parent
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 12
                clip: true
                text: valueString
            }
            
            MouseArea {
                anchors.fill: parent
                hoverEnabled: root.itemRemovable
                onClicked: root.selected()
                onDoubleClicked: console.log("TODO")
                onEntered: root.itemHovered = true
                onExited: root.itemHovered = false
            }
        }
        
        Rectangle { //TODO: replace with image
            width: parent.height
            height: parent.height
            anchors.right: parent.right
            color: root.itemHovered ? "red" : "transparent"
            visible: root.itemRemovable
        
            Text {
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                visible: root.itemHovered
                text: "---"
                color: "white"
            }
            
            MouseArea {
                anchors.fill: parent
                hoverEnabled: root.itemRemovable
                onEntered: root.itemHovered = true
                onExited: root.itemHovered = false
                onClicked: root.removed()
            }
        }
    }
}