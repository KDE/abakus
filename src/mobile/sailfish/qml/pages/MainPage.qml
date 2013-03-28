import QtQuick 1.1
import Sailfish.Silica 1.0
import abakus 1.0 as Abakus

Page {
    id: page

    SilicaFlickable {
        anchors.fill: parent

        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
        PushUpMenu {
            MenuItem {
                text: "Settings"
                onClicked: pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
            }
        }

        // Tell SilicaFlickable the height of its content.
        contentHeight: childrenRect.height

        Column {
            width: parent.width
            anchors.top: parent.top

            SilicaListView {
                id: history
                width: parent.width
                height: page.height - (editorRow.height + keypad.height + 10)

                VerticalScrollDecorator { flickable: history }

                currentIndex: count -1
                clip: true

                property int minTagSize: 0

                model: resultModel
                delegate: ResultViewItem {
                    currentHistoryIndex: history.currentIndex
                    currentMinTagSize: history.minTagSize

                    onExpressionSelected: editor.text = expression
                    onResultSelected: editor.text += result
                    onTagSelected: editor.text += tag

                    Component.onCompleted: {
                        mainWindow.addVisibleHistoryItemIndex(model.index)
                        history.updateTagWidth()
                    }

                    Component.onDestruction: {
                        mainWindow.removeVisibleHistoryItemIndex(model.index)
                        history.updateTagWidth()
                    }
                }

                Label {
                    id: tagItemDummy
                    font.italic: true
                    visible: false
                }

                function updateTagWidth() {
                    var i = 0
                    var itemIndexNext = mainWindow.getVisibleHistoryItemIndex(i)
                    var minTagSize = 0

                    while(itemIndexNext > -1)
                    {
                        tagItemDummy.text = mainWindow.getTag(itemIndexNext)
                        if(minTagSize < tagItemDummy.width)
                        {
                            minTagSize = tagItemDummy.width
                        }
                        ++i
                        itemIndexNext = mainWindow.getVisibleHistoryItemIndex(i)
                    }

                    tagItemDummy.text = ""
                    history.minTagSize = minTagSize
                }

                Component.onCompleted: history.positionViewAtEnd()
            }

            Row {
                id: editorRow
                width: page.width

                TextField {
                    id: editor
                    width: page.width - clearButton.width
                    focus: true
                    placeholderText: "Enter expression"
                    enableSoftwareInputPanel: false
                    textMargin: 5
                    clip:false
                    z: 0

                    Keys.onPressed: {
                        if(event.key == Qt.Key_Return || event.key == Qt.Key_Enter) {
                            mainWindow.slotEvaluate(editor.text)
                            editor.selectAll()
                        }
                        console.log(clearButton.pos.x + " - " + clearButton.pos.y)
                    }

                    onTextChanged: mainWindow.slotTextChanged(text)

                    Keys.onUpPressed: mainWindow.historyPrevious()
                    Keys.onDownPressed: mainWindow.historyNext()

                    Connections {
                        target: mainWindow

                        onSetEditorText: editor.text = editorText
                    }
                }

                IconButton{
                    id: clearButton
                    icon.source: "image://theme/icon-l-backspace"
                    onClicked: editor.text = ""
                    onPressAndHold: editor.text = ""
                }
            }

            KeyPad {
                id: keypad
                width: parent.width

                onPressed:  editor.text = editor.text + value

                onEvaluate: {
                    mainWindow.slotEvaluate(editor.text)
                    editor.selectAll()
                }
            }
        }
    }
}


