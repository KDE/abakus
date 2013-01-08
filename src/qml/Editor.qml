import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents

Item {
    id: root
    
    property alias text: textField.text
    property QtObject mainWindowObject
    
    PlasmaComponents.TextField {
        id: textField
        anchors.fill: parent
        font.pixelSize: 12
        clearButtonShown: true
        placeholderText: i18n("Enter expression")
        
        onAccepted: {
            mainWindowObject.slotEvaluate(text)
            textField.selectAll()
        }
        
        onTextChanged: mainWindowObject.slotTextChanged(text)
        
        Keys.onUpPressed: mainWindowObject.historyPrevious()
        Keys.onDownPressed: mainWindowObject.historyNext()
        
        Connections {
            target: mainWindowObject
            
            onSetEditorText: editor.text = editorText
        }
    }
}