import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents

Item {
    id: root
    
    property alias text: textField.text
    
    signal textChanged ( string text )
    signal toEvaluate ( string text )
    signal upPressed ()
    signal downPressed ()
    
    PlasmaComponents.TextField {
        id: textField
        anchors.fill: parent
        font.pixelSize: 12
        clearButtonShown: true
        placeholderText: i18n("Enter expression")
        
        onAccepted: {
            root.toEvaluate(text)
            textField.selectAll()
        }
        
        onTextChanged: root.textChanged(text)
        
        Keys.onUpPressed: root.upPressed()
        Keys.onDownPressed: root.downPressed()
    }
}