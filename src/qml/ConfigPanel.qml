import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents

Item {
    id: root
    
    property alias copactModeActive: compactMode.checked
    property int settingsPanelHeigth: 80
    property int buttonHeight
    property QtObject mainWindowObject
    
    signal settingsPanelVisibleChanged(bool settingsVisible)
    
    PlasmaComponents.ButtonRow {
        id: toolbarButtons
        height: root.buttonHeight
        anchors.left: root.left
        anchors.top: root.top
        exclusive: false
        
        PlasmaComponents.ToolButton {
            id: settings
            height: parent.height
            flat: true
            checkable: true
            iconSource: "configure"
            
            onCheckedChanged: {
                if(checked) compactMode.checked = false
                settingsPanelVisibleChanged(checked)
            }
        }
        
        PlasmaComponents.ToolButton {
            id: compactMode
            height: parent.height
            flat: true
            checkable:true
            iconSource: "merge"
            
            onCheckedChanged: {
                if(checked) settings.checked = false
                mainWindowObject.setCompactMode(checked)
            }
            
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
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: toolbarButtons.bottom
        anchors.bottom: root.bottom
        clip: true
        
        Column {
            width: 150
            height: parent.height
            anchors.left: parent.left
            anchors.top: parent. top
            
            Row {
                width: parent.width
                
                PlasmaComponents.Label {
                    id: precissionLabel
                    height: buttonHeight
                    verticalAlignment: Text.AlignVCenter
                    text: i18n("Decimal Precision:")
                }
                    
                PlasmaComponents.TextField {
                    id: precission
                    width: parent.width - precissionLabel.width
                    height: buttonHeight
                    readOnly: true
                    validator: IntValidator{bottom: 1; top: 75;}
                    
                    property string oldPrecision
                    
                    onAccepted: {
                        precission.readOnly = true
                        mainWindowObject.setPrecision(parseInt(precission.text))
                    }
                    
                    onActiveFocusChanged: {
                        if(!activeFocus && !readOnly)
                        {
                            readOnly = true
                            text = oldPrecision
                        }
                    }
                    
                    PlasmaComponents.ToolButton {
                        width: height
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.margins: 1
                        flat: true
                        iconSource: precission.readOnly ? "arrow-down" : "key-enter"
                        
                        property ContextMenu contextMenu
                        onClicked: {
                            if (precission.readOnly)
                            {
                                if (!contextMenu) {
                                    contextMenu = contextMenuComponent.createObject(precission)
                                }
                                contextMenu.open()
                            }
                            else if(precission.acceptableInput)
                            {
                                precission.readOnly = true
                                mainWindowObject.setPrecision(parseInt(precission.text))
                            }
                        }
                    }
                }
            }
            
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
            
            Connections {
                target: mainWindowObject
                
                onPrecisionChanged: precission.text = newPrecision < 0 ? i18n("Auto") : newPrecision
            }
            
            Component {
                id: contextMenuComponent
                PlasmaComponents.ContextMenu {
                    visualParent: precission
                    PlasmaComponents.MenuItem {
                        text: i18n("Automatic")
                        onClicked: mainWindowObject.setPrecision(-1)
                    }
                    PlasmaComponents.MenuItem {
                        text: i18n("Custom (1-75)")
                        onClicked: {
                            precission.oldPrecision = precission.text
                            precission.text = ""
                            precission.focus = true
                            precission.readOnly = false
                        }
                    }
                }
            }
        }
    }
}