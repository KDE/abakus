import QtQuick 1.1
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.extras 0.1 as PlasmaExtras

Item {
    id: root
    
    property alias tabBarHeight: sidebarTabBar.height
    
    signal numeralSelected(string numeralName)
    signal numeralRemoved(string numeralName)
    
    signal functionSelected(string functionName)
    signal functionRemoved(string functionName)
    
    PlasmaComponents.TabBar {
        id: sidebarTabBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 3
        
        PlasmaComponents.TabButton { id: numeralsTabButton; tab: numeralsTab; text: i18n("Numerals") }
        PlasmaComponents.TabButton { id: functionsTabButton; tab: functionsTab; text: i18n("Functions") }
    }
    
    PlasmaComponents.TabGroup {
        id: sidebarTabGroup
        anchors.left: parent.left
        anchors.top: sidebarTabBar.bottom
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 3
        
        PlasmaExtras.ScrollArea {
            id: numeralsTab
            anchors.fill: parent
            
            ListView {
                id: numerals
                anchors.fill: parent
                clip: true
                
                model: numeralModel
                delegate: MathExpressionItem {
                    nameString: model.name
                    valueString: model.valueString
                    typeString: model.typeString
                    onSelected: root.numeralSelected(model.name)
                    onRemoved: root.numeralRemoved(model.name)
                }
                
                section.property: "typeString"
                section.criteria: ViewSection.FullString
                section.delegate: PlasmaCore.FrameSvgItem {
                    width: parent.width
                    height: 20
                    
                    imagePath: "widgets/listitem"
                    prefix: "section"
                    
                    Text {
                        anchors.centerIn: parent
                        font.pixelSize: 12
                        text: section
                    }
                }
            }
        }
        
        PlasmaExtras.ScrollArea {
            id: functionsTab
            anchors.fill: parent
                
            ListView {
                id: functions
                anchors.fill: parent
                clip: true
                
                model: functionModel
                delegate: MathExpressionItem {
                    nameString: model.name + "(" + model.variable + ")"
                    valueString: model.value
                    typeString: model.typeString
                    onSelected: root.functionSelected(model.name)
                    onRemoved: root.functionRemoved(model.name)
                }
                
                section.property: "typeString"
                section.criteria: ViewSection.FullString
                section.delegate: PlasmaCore.FrameSvgItem {
                    width: parent.width
                    height: 20
                    
                    imagePath: "widgets/listitem"
                    prefix: "section"
                    
                    Text {
                        anchors.centerIn: parent
                        font.pixelSize: 12
                        text: section
                    }
                }
            }
        }
    }
}