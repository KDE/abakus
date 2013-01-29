import QtQuick 1.1
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents

Item {
    id: root
    
    property QtObject mainWindowObject
    
    QtObject {
        id: internal
        
        property ContextMenu contextMenu
    }
    
    Text {
        id: degrees
        visible: false
        text: i18n("Degrees")
    }
    
    Text {
        id: radians
        visible: false
        text: i18n("Radians")
    }
    
    PlasmaCore.FrameSvgItem {
        id: trigModeFrame
        width: (degrees.width > radians.width) ? degrees.width + 1.25 * height : radians.width + 1.25 * height
        height: parent.height
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        imagePath: "widgets/lineedit"
        prefix: "base"
        
        Text {
            id: trigMode
            anchors.fill: parent
            anchors.leftMargin: 6
            verticalAlignment: Text.AlignVCenter
            text: degrees.text
        }
    
        PlasmaComponents.ToolButton {
            width: height
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 1
            flat: true
            iconSource: "arrow-down"
            
            onClicked: {
                if (!internal.contextMenu) {
                    internal.contextMenu = contextMenuComponent.createObject(trigModeFrame)
                }
                internal.contextMenu.open()
            }
        }
    }
    
    
    Connections {
        target: mainWindowObject
        
        onTrigModeChanged: {
            if(mode == 0) { //TODO: use the Abakus::TrigMode enum
                trigMode.text = degrees.text
            }
            else if(mode == 1) {
                trigMode.text = radians.text
            }
        }
    }
    
    Component {
        id: contextMenuComponent
        PlasmaComponents.ContextMenu {
            visualParent: trigModeFrame
            PlasmaComponents.MenuItem {
                text: degrees.text
                onClicked: mainWindowObject.setDegrees()
            }
            PlasmaComponents.MenuItem {
                text: radians.text
                onClicked: mainWindowObject.setRadians()
            }
        }
    }
}
