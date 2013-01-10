import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents

Item {
    id: root
    
    property string strDegrees: i18n("Degrees")
    property string strRadians: i18n("Radians")
    
    property QtObject mainWindowObject
    
    PlasmaComponents.ToolButton {
        id: trigModeButton
        height: parent.height
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        flat: true
        property ContextMenu contextMenu
        text: root.strDegrees
        onClicked: {
            if (!contextMenu) {
                contextMenu = contextMenuComponent.createObject(trigModeButton)
            }
            contextMenu.open()
        }
    }
    
    Connections {
        target: mainWindowObject
        
        onTrigModeChanged: {
            if(mode == 0) { //TODO: use the Abakus::TrigMode enum
                trigModeButton.text = root.strDegrees
            }
            else if(mode == 1) {
                trigModeButton.text = root.strRadians
            }
        }
    }
    
    Component {
        id: contextMenuComponent
        PlasmaComponents.ContextMenu {
            visualParent: trigModeButton
            PlasmaComponents.MenuItem {
                text: root.strDegrees
                onClicked: mainWindowObject.setDegrees()
            }
            PlasmaComponents.MenuItem {
                text: root.strRadians
                onClicked: mainWindowObject.setRadians()
            }
        }
    }
}
