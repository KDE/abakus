import QtQuick 1.1
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents
import abakus 1.0 as Abakus

Item {
    id: root
    
    property int settingsPanelHeigth: 80
    property int buttonHeight
    property QtObject mainWindowObject
    
    signal settingsPanelVisibleChanged(bool settingsVisible)
    
    signal rejectFocus()
    
    Abakus.Settings {
        id: settings
        
        onPrecisionChanged: precisionText.text = precision < 0 ? i18n("Auto") : precision
        
        onCompactModeChanged: compactModeButton.checked = compactMode
        
        Component.onCompleted: {
            precisionText.text = precision < 0 ? i18n("Auto") : precision
            compactModeButton.checked = compactMode
        }
    }
    
    PlasmaComponents.ButtonRow {
        id: toolbarButtons
        height: root.buttonHeight
        anchors.left: root.left
        anchors.top: root.top
        exclusive: false
        
        PlasmaComponents.ToolButton {
            id: settingsButton
            height: parent.height
            flat: true
            checkable: true
            iconSource: "configure"
            
            onCheckedChanged: {
                if(checked) settings.compactMode = false
                settingsPanelVisibleChanged(checked)
            }
        }
        
        PlasmaComponents.ToolButton {
            id: compactModeButton
            height: parent.height
            flat: true
            checkable:true
            iconSource: "merge"
            
            onCheckedChanged: {
                if(checked) settingsButton.checked = false
                settings.compactMode = checked
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
                    id: precisionLabel
                    height: buttonHeight
                    verticalAlignment: Text.AlignVCenter
                    text: i18n("Decimal Precision:")
                }
                
                Item {
                    id: precisionComboBox
                    width: parent.width - precisionLabel.width
                    height: buttonHeight
                    
                    property bool editCustom: false
                    
                    PlasmaCore.FrameSvgItem {
                        anchors.fill: parent
                        imagePath: "widgets/lineedit"
                        prefix: "base"
                        visible: !precisionComboBox.editCustom
                        
                        Text {
                            id: precisionText
                            anchors.fill: parent
                            anchors.leftMargin: 6
                            verticalAlignment: Text.AlignVCenter
                        }
                        
                        MouseArea {
                            anchors.fill: parent
                            
                            onDoubleClicked: {
                                precisionComboBox.editCustom = true
                                precisionCustom.text = (precisionText.text == i18n("Auto")) ? "" : precisionText.text
                                precisionCustom.forceActiveFocus()
                            }
                        }
                    }
                    
                    PlasmaComponents.TextField {
                        id: precisionCustom
                        anchors.fill: parent
                        validator: IntValidator{bottom: 1; top: 75;}
                        visible: precisionComboBox.editCustom
                        
                        onAccepted: {
                            precisionComboBox.editCustom = false
                            precisionComboBox.focus = false
                            settings.precision = parseInt(precisionCustom.text)
                            root.rejectFocus()
                        }
                        
                        onActiveFocusChanged: {
                            if(!activeFocus && precisionComboBox.editCustom)
                            {
                                precisionComboBox.editCustom = false
                            }
                        }
                    }
                    
                    PlasmaComponents.ToolButton {
                        width: height
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.margins: 1
                        flat: true
                        iconSource: precisionComboBox.editCustom ? "key-enter" : "arrow-down"
                        
                        property ContextMenu contextMenu
                        onClicked: {
                            if (!precisionComboBox.editCustom)
                            {
                                if (!contextMenu) {
                                    contextMenu = contextMenuComponent.createObject(precisionComboBox)
                                }
                                contextMenu.open()
                            }
                            else if(precisionCustom.acceptableInput)
                            {
                                precisionComboBox.editCustom = false
                                precisionComboBox.focus = false
                                settings.precision = parseInt(precisionCustom.text)
                                root.rejectFocus()
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
            
            Component {
                id: contextMenuComponent
                PlasmaComponents.ContextMenu {
                    visualParent: precisionComboBox
                    PlasmaComponents.MenuItem {
                        text: i18n("Automatic")
                        onClicked: settings.precision = -1
                    }
                    PlasmaComponents.MenuItem {
                        text: i18n("Custom (1...75)")
                        onClicked: {
                            precisionComboBox.editCustom = true
                            precisionCustom.text = ""
                            precisionCustom.forceActiveFocus()
                        }
                    }
                }
            }
        }
    }
}