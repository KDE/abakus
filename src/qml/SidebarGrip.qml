import QtQuick 1.1
import org.kde.plasma.core 0.1 as PlasmaCore

Item {
    id: root
    
    property bool sidebarShown
    property bool itemHovered
    property Item sidebarItem
    
    signal toggleSidebar()
    signal sidebarWidthChanged(int newWidth)
    
    signal showToolTip(int xPosition, int yPosition, string toolTipText)
    signal hideToolTip()
    
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
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        
        Timer {
            interval: 1000
            running: mouseArea.containsMouse && !mouseArea.pressed && tooltip.length
            
            property string tooltip: root.sidebarShown ? i18n("Click to hide sidebar\nClick and drag to change sidebar width") : i18n("Click to show sidebar")
            
            onTriggered: {
                var position = mapToItem(null, 0, height)
                if(root.sidebarShown)
                {
                    root.showToolTip(position.x, position.y, tooltip)
                }
                else
                {
                    root.showToolTip(position.x, position.y, tooltip)
                }
            }
        }
        
        onEntered: root.itemHovered = true
        onExited: {
            root.itemHovered = false
            root.hideToolTip()
        }
        
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