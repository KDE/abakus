import QtQuick 2.0
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
        text: qsTr("Degrees")
    }
    
    Text {
        id: radians
        visible: false
        text: qsTr("Radians")
    }
    
    Item {
        id: trigModeFrame
        width: (degrees.width > radians.width) ? degrees.width + 1.25 * height : radians.width + 1.25 * height
        height: parent.height
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        //imagePath: "widgets/lineedit"
        //prefix: "base"
        
        Text {
            id: trigModeText
            anchors.fill: parent
            anchors.leftMargin: 6
            verticalAlignment: Text.AlignVCenter
            text: settings.trigMode == Abakus.Settings.Degrees ? degrees.text: radians.text
        }
    
        Item { //ToolButton
            width: height
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 1
            /*flat: true
            iconSource: "arrow-down"
            
            onClicked: {
                if (!internal.contextMenu) {
                    internal.contextMenu = contextMenuComponent.createObject(trigModeFrame)
                }
                internal.contextMenu.open()
            }*/
        }
    }
    
    /*Component {
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
    }*/
}
