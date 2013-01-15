import QtQuick 1.1
import org.kde.plasma.core 0.1 as PlasmaCore

Item {
    id: root
    clip: true
    
    property bool sidebarShown
    property bool itemHovered
    
    signal toggleSidebar()
    
    PlasmaCore.FrameSvgItem {
        width: parent.width + 10
        height: parent.height
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        
        imagePath: "widgets/listitem"
        prefix: itemHovered ? "pressed" : ""
    }
    
    PlasmaCore.SvgItem {
        id: sign
        width: parent.width < parent.height ? parent.width : parent.height
        height: width
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        svg: PlasmaCore.Svg { imagePath: "widgets/arrows" }
        elementId: root.sidebarShown ? "right-arrow" : "left-arrow"
    }
    
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        
        onEntered: root.itemHovered = true
        onExited: root.itemHovered = false
        
        onClicked: toggleSidebar()
    }
}