import QtQuick 1.1
import org.kde.plasma.core 0.1 as PlasmaCore
import abakus 1.0 as Abakus

Item {
    id: root
    
    property bool itemHovered
    
    signal showToolTip(int xPosition, int yPosition, string toolTipText)
    signal hideToolTip()
    
    QtObject {
        id: internal
        
        property bool sidebarShown: settings.mathematicalSidebarVisible && !settings.compactMode
        property int latestSidebarWidth
        property int xPositionOnChangeStart
        property bool sidebarWidthChanged: false
    }
    
    Abakus.Settings {
        id: settings
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
        elementId: (settings.mathematicalSidebarVisible && !settings.compactMode) ? "right-arrow" : "left-arrow"
    }
    
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        
        Timer {
            interval: 1000
            running: mouseArea.containsMouse && !mouseArea.pressed && tooltip.length
            
            property string tooltip: internal.sidebarShown ? i18n("Click to hide sidebar\nClick and drag to change sidebar width") : i18n("Click to show sidebar")
            
            onTriggered: {
                var position = mapToItem(null, 0, height)
                if(internal.sidebarShown)
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
        
        onClicked: if(!internal.sidebarWidthChanged) {
            if(settings.compactMode)
            {
                settings.compactMode = false
                settings.mathematicalSidebarVisible = true
            }
            else
            {
                settings.mathematicalSidebarVisible = !settings.mathematicalSidebarVisible
            }
            
        }
        
        onPressed: {
            if(mouse.button == Qt.LeftButton) {
                internal.xPositionOnChangeStart = mapToItem(null, mouse.x, 0).x
                internal.sidebarWidthChanged = false
            }
        }
        
        onPositionChanged: {
            if(internal.sidebarShown && (mouse.buttons & Qt.LeftButton)) {
                var currentX = mapToItem(null, mouse.x, 0).x
                if(internal.sidebarWidthChanged) {
                    settings.mathematicalSidebarWidth = internal.latestSidebarWidth + (internal.xPositionOnChangeStart - currentX)
                }
                else if(Math.abs(internal.xPositionOnChangeStart - currentX) > 1) {
                    internal.latestSidebarWidth = settings.mathematicalSidebarWidth
                    internal.xPositionOnChangeStart = currentX
                    internal.sidebarWidthChanged = true
                }
            }
        }
    }
}