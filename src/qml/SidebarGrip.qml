import QtQuick 1.1
import org.kde.plasma.core 0.1 as PlasmaCore

Item {
    id: root
    
    property bool sidebarShown
    property bool itemHovered
    property Item sidebarItem
    
    signal toggleSidebar()
    signal sidebarWidthChanged(int newWidth)
    
    QtObject {
        id: internal
        
        property int latestSidebarWidth
        property int xPositionOnChangeStart
        property bool sidebarWidthChanged: false
    }
    
    PlasmaCore.FrameSvgItem {
        width: parent.width + 5
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
        
        onClicked: if(!internal.sidebarWidthChanged) toggleSidebar()
        
        onPressed: {
            if(mouse.button == Qt.LeftButton) {
                internal.xPositionOnChangeStart = mapToItem(null, mouse.x, 0).x
                internal.sidebarWidthChanged = false
            }
        }
        
        onReleased: {
            if(internal.sidebarWidthChanged && mouse.button == Qt.LeftButton) {
                root.sidebarWidthChanged(root.sidebarItem.width)
            }
        }
        
        onPositionChanged: {
            if(root.sidebarShown && (mouse.buttons & Qt.LeftButton)) {
                var currentX = mapToItem(null, mouse.x, 0).x
                if(internal.sidebarWidthChanged) {
                    root.sidebarItem.width = internal.latestSidebarWidth + (internal.xPositionOnChangeStart - currentX)
                }
                else if(Math.abs(internal.xPositionOnChangeStart - currentX) > 1) {
                    internal.latestSidebarWidth = root.sidebarItem.width
                    internal.xPositionOnChangeStart = currentX
                    internal.sidebarWidthChanged = true
                }
            }
        }
    }
}