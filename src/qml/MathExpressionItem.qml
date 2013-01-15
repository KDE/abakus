import QtQuick 1.1
import org.kde.plasma.core 0.1 as PlasmaCore

PlasmaCore.FrameSvgItem {
    id: root
    width: parent.width
    height: 20
    
    imagePath: "widgets/listitem"
    prefix: itemHovered ? "pressed" : "normal"
    
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
            hoverEnabled: true
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
                hoverEnabled: true
                onClicked: root.selected()
                onDoubleClicked: console.log("TODO")
                onEntered: root.itemHovered = true
                onExited: root.itemHovered = false
            }
        }
        
        
        PlasmaCore.IconItem {
            width: parent.height
            height: parent.height
            anchors.right: parent.right
            visible: root.itemRemovable
            source: root.itemHovered ? "list-remove" : ""
            
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