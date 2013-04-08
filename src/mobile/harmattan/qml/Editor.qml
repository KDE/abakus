import QtQuick 1.1

Rectangle {
    id: root
    color: "black"
    border.color: "dimgray"
    border.width: 2
    height: editor.height + 16

    property int fontPointSize: 24
    property string fontColorEditor: "orange"
    property string fontColorEditorSelection: "black"
    property string colorEditorSelection: "orange"

    signal textChanged(string text)
    signal historyPrevious()
    signal historyNext()

    function selectAll() {editor.selectAll()}

    function text() {return editor.text}

    function setText(text) {editor.text = text}

    function addText(text) {
        var selectionStart = editor.selectionStart
        var selectionEnd = editor.selectionEnd
        var editorTextLength = editor.text.length
        var finalCursorPosition = selectionStart + text.length
        editor.text = editor.text.substring(0, selectionStart) + text + editor.text.substring(selectionEnd, editorTextLength)
        //this is needed, because a text change can trigger the setEditorText signal from mainWindow, which replace the text
        if(selectionStart + text.length + editorTextLength - selectionEnd === editor.text.length)
        {
            editor.cursorPosition = finalCursorPosition
        }
    }

    function removeText() {
        var selectionStart = editor.selectionStart
        var selectionEnd = editor.selectionEnd
        var editorTextLength = editor.text.length
        var finalCursorPosition = selectionStart == selectionEnd ? selectionStart - 1 : selectionStart
        editor.text = editor.text.substring(0, finalCursorPosition) + editor.text.substring(selectionEnd, editorTextLength)
        editor.cursorPosition = finalCursorPosition
    }

    Rectangle {
        width: parent.width
        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: -parent.border.width/2
        color: parent.color
    }

    TextInput {
        id: editor
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: clearButton.left
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        focus: true
        activeFocusOnPress: false
        selectByMouse: true
        clip:false
        font.pointSize: root.fontPointSize
        color: root.fontColorEditor
        selectedTextColor: fontColorEditorSelection
        selectionColor: colorEditorSelection

        onTextChanged: root.textChanged(text)

        Keys.onUpPressed: root.historyPrevious()
        Keys.onDownPressed: root.historyNext()

        Text {
            id: placeholderText
            anchors.fill: parent
            font.pointSize: root.fontPointSize
            color: "dimgray"
            visible: editor.text.length == 0
            text: "Enter expression"
        }
    }

    Text {
        id: dummy
        visible: false
        font.pointSize: root.fontPointSize
        text: "⌫"
    }

    Button {
        id: clearButton
        width: dummy.width
        height: dummy.height
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        textColor: "dimgray"
        textColorHighlight: "white"
        buttonColorHighlight: buttonColor
        fontPointSize: root.fontPointSize
        text: "⌫"

        onPressed: removeText()
        onPressAndHold: editor.text = ""
    }
}
