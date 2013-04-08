import QtQuick 1.1
//import abakus 1.0 as Abakus

Item {
    id: root

    QtObject {
        id: internal

        property int fontPointSize: 24
        property string fontColorEditor: "orange"
        property string fontColorExpression: "gray"
        property string fontColorResult: "dodgerblue"
        property string fontColorTag: "gray"
    }

    ListView {
        id: history
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: editor.top

        currentIndex: count -1
        clip: true

        property int minTagSize: 0

        model: resultModel
        delegate: ResultViewItem {
            currentHistoryIndex: history.currentIndex
            currentMinTagSize: history.minTagSize

            fontPointSize: internal.fontPointSize
            fontColorExpression: internal.fontColorExpression
            fontColorResult: internal.fontColorResult
            fontColorTag: internal.fontColorTag

            onExpressionSelected: editor.addText(expression)
            onResultSelected: editor.addText(result)
            onTagSelected: editor.addText(tag)

            Component.onCompleted: {
                mainWindow.addVisibleHistoryItemIndex(model.index)
                history.updateTagWidth()
            }

            Component.onDestruction: {
                mainWindow.removeVisibleHistoryItemIndex(model.index)
                history.updateTagWidth()
            }
        }

        Text {
            id: tagItemDummy
            font.italic: true
            visible: false
            font.pointSize: internal.fontPointSize
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

    Editor {
        id: editor
        width: parent.width
        anchors.bottom: keypad.top
        anchors.bottomMargin: border.width
        fontPointSize: internal.fontPointSize
        fontColorEditor: internal.fontColorEditor

        onTextChanged: mainWindow.slotTextChanged(text)

        onHistoryPrevious: mainWindow.historyPrevious()
        onHistoryNext: mainWindow.historyNext()

        Connections {
            target: mainWindow

            onSetEditorText: editor.setText(editorText)
        }
    }

    KeyPad {
        id: keypad
        width: parent.width
        anchors.bottom: parent.bottom

        onPressed:  editor.addText(value)

        onEvaluate: {
            mainWindow.slotEvaluate(editor.text())
            editor.selectAll()
        }
    }
}
