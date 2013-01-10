import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents

Item {
    id: root
    
    property alias settingsPanelHeigth: settingsPanel.height
    property int buttonHeight: height
    property QtObject mainWindowObject
    
    signal settingsPanelVisibleChanged(bool settingsVisible)
    
    PlasmaComponents.ButtonRow {
        id: toolbarButtons
        height: parent.height
        anchors.left: parent.left
        anchors.bottom: parent. bottom
        exclusive: false
        
        PlasmaComponents.ToolButton {
            id: settings
            height: parent.height
            flat: true
            checkable: true
            iconSource: "configure"
            
            onClicked: settingsPanelVisibleChanged(checked)
        }
        
        PlasmaComponents.ToolButton {
            id: compactMode
            height: parent.height
            flat: true
            checkable:true
            iconSource: "merge"
            
            onClicked: mainWindowObject.slotToggleCompactMode()
            
            Connections {
                target: mainWindowObject
                
                onCompactModeChanged: compactMode.checked = active
            }
        }
        
        PlasmaComponents.ToolButton {
            id: help
            height: parent.height
            flat: true
            iconSource: "help-about"
            
            onClicked: {
                var position = mapToItem(null, 0, height)
                mainWindowObject.showHelpMenu(position.x, position.y)
            }
        }
    }
    
    Item {
        id: settingsPanel
        height: 80
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: toolbarButtons.top
        
        Column {
            width: 150
            height: parent.height
            anchors.left: parent.left
            anchors.bottom: parent. bottom
            
            PlasmaComponents.ToolButton {
                id: configureShortcuts
                width: parent.width
                height: buttonHeight
                flat: true
                iconSource: "configure-shortcuts"
                text: i18n("Configure Shortcuts...")
                
                onClicked: mainWindowObject.configureShortcuts()
            }
            
            PlasmaComponents.ToolButton {
                id: clearHistory
                width: parent.width
                height: buttonHeight
                flat: true
                iconSource: "edit-clear-list"
                text: i18n("Clear History")
                
                onClicked: mainWindowObject.clearHistory()
            }
            
            PlasmaComponents.ToolButton {
                id: precission
                width: parent.width
                height: buttonHeight
                flat: true
                iconSource: ""
                text: i18n("Automatic Precision")
                property ContextMenu contextMenu
                onClicked: {
                    if (!contextMenu) {
                        contextMenu = contextMenuComponent.createObject(precission)
                    }
                    contextMenu.open()
                }
            }
            
            Connections {
                target: mainWindowObject
                
                onPrecisionChanged: precission.text = newPrecision < 0 ? i18n("Automatic Precision") : newPrecision + " Digit Precison"
            }
            
            Component {
                id: contextMenuComponent
                PlasmaComponents.ContextMenu {
                    visualParent: precission
                    PlasmaComponents.MenuItem {
                        text: i18n("Automatic Precision")
                        onClicked: mainWindowObject.setPrecision(-1)
                    }
                    PlasmaComponents.MenuItem {
                        text: i18n("3 Digit Precison")
                        onClicked: mainWindowObject.setPrecision(3)
                    }
                    PlasmaComponents.MenuItem {
                        text: i18n("8 Digit Precison")
                        onClicked: mainWindowObject.setPrecision(8)
                    }
                    PlasmaComponents.MenuItem {
                        text: i18n("15 Digit Precison")
                        onClicked: mainWindowObject.setPrecision(15)
                    }
                    PlasmaComponents.MenuItem {
                        text: i18n("50 Digit Precison")
                        onClicked: mainWindowObject.setPrecision(50)
                    }
                    PlasmaComponents.MenuItem {
                        text: i18n("Custom Precision...")
                        onClicked: mainWindowObject.setPrecision(-2)
                    }
                }
            }
        }
    }
}