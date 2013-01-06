import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents

PlasmaComponents.TextField {
    id: editor
    font.pixelSize: 12
    clearButtonShown: true
    placeholderText: i18n("Enter expression")
    
    property alias text: editor.text
    
    signal textChanged ( string text )
    signal toEvaluate ( string text )
    signal upPressed ()
    signal downPressed ()
    
    onAccepted: {
        editor.toEvaluate(text)
        editor.selectAll()
    }
    
    onTextChanged: editor.textChanged(text)
    
    Keys.onUpPressed: editor.upPressed()
    Keys.onDownPressed: editor.downPressed()
}