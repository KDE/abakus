import QtQuick 1.1

Item {
    id: root
    opacity: 0.5
    clip: true
    
    property alias text: sign.text
    
    signal toggleSidebar()
    
    Rectangle {
        width: parent.width + 10
        height: parent.height
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        color: "lightgrey"
        radius: 3
    }
    
    Text {
        id: sign
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: sidebar.sidebarGripSign
    }
    
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        
        onEntered: root.opacity = 1
        onExited: root.opacity = 0.5
        
        onClicked: toggleSidebar()
    }
}