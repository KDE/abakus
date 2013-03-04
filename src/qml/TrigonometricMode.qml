import QtQuick 1.1
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents
import abakus 1.0 as Abakus

Item {
    id: root
    
    QtObject {
        id: internal
        
        property ContextMenu contextMenu
    }
    
    Abakus.Settings {
        id: settings
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
            id: trigModeText
            anchors.fill: parent
            anchors.leftMargin: 6
            verticalAlignment: Text.AlignVCenter
            text: settings.trigMode == Abakus.Settings.Degrees ? degrees.text: radians.text
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
    
    Component {
        id: contextMenuComponent
        PlasmaComponents.ContextMenu {
            visualParent: trigModeFrame
            PlasmaComponents.MenuItem {
                text: degrees.text
                onClicked: settings.trigMode = Abakus.Settings.Degrees
            }
            PlasmaComponents.MenuItem {
                text: radians.text
                onClicked: settings.trigMode = Abakus.Settings.Radians
            }
        }
    }
}
