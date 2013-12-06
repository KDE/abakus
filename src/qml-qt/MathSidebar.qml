import QtQuick 2.0
import abakus 1.0 as Abakus

Item {
    id: root
    
    property alias tabBarHeight: sidebarTabBar.height
    
    signal numeralSelected(string numeralName)
    signal numeralRemoved(string numeralName)
    
    signal functionSelected(string functionName)
    signal functionRemoved(string functionName)
    
    signal rejectFocus()
    
    states: State {
        name: "shown"
        AnchorChanges { target: root; anchors.left: undefined; anchors.right: parent.right }
    }
    
    transitions: Transition {
        AnchorAnimation { duration: 100; easing.type: Easing.InOutQuad }
    }
    
    Abakus.Settings {
        id: settings
        
        onCompactModeChanged: root.state = (mathematicalSidebarVisible && !compactMode) ? "shown" : ""
        
        onMathematicalSidebarWidthChanged: root.width = mathematicalSidebarWidth
        
        onMathematicalSidebarVisibleChanged: root.state = (mathematicalSidebarVisible && !compactMode) ? "shown" : ""
        
        Component.onCompleted: {
            root.width = mathematicalSidebarWidth
            root.state = (mathematicalSidebarVisible && !compactMode) ? "shown" : ""
        }
    }
    
    Item {
        id: sidebarTabBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 2
        
        property bool tabBarLoaded: false
        
        /*PlasmaComponents.TabButton { id: numeralsTabButton; tab: numeralsTab; text: i18n("Numerals"); property string tabString: "numerals" }
        PlasmaComponents.TabButton { id: functionsTabButton; tab: functionsTab; text: i18n("Functions"); property string tabString: "functions" }
        
        onCurrentTabChanged: if(tabBarLoaded) settings.mathematicalSidebarActiveView = sidebarTabBar.currentTab.tabString
        
        Component.onCompleted: {
            if(settings.mathematicalSidebarActiveView == "numerals")
            {
                sidebarTabBar.currentTab = numeralsTabButton
                sidebarTabGroup.currentTab = numeralsTab 
            }
            else
            {
                sidebarTabBar.currentTab = functionsTabButton
                sidebarTabGroup.currentTab = functionsTab 
            }
            tabBarLoaded = true
        }*/
    }
    
    Column {
        id: sidebarTabGroup
        anchors.left: parent.left
        anchors.top: sidebarTabBar.bottom
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 2
        
        /*PlasmaExtras.ScrollArea {
            id: numeralsTab
            anchors.fill: parent*/

            ListView {
                id: numerals
                //anchors.fill: parent
                width: parent.width
                height: parent.height / 2
                clip: true
                
                model: numeralModel
                delegate: MathExpressionItem {
                    nameString: model.name
                    valueString: model.valueString
                    typeString: model.typeString
                    onSelected: root.numeralSelected(model.name)
                    onRemoved: root.numeralRemoved(model.name)
                    onFocusChanged: root.rejectFocus()
                }
                
                section.property: "typeString"
                section.criteria: ViewSection.FullString
                section.delegate: Item {
                    width: parent.width
                    height: 20
                    
                    //imagePath: "widgets/listitem"
                    //prefix: "section"
                    
                    Text {
                        anchors.centerIn: parent
                        font.pixelSize: 12
                        text: section
                    }
                }

        }
        
        /*PlasmaExtras.ScrollArea {
            id: functionsTab
            anchors.fill: parent*/
                
            ListView {
                id: functions
                //anchors.fill: parent
                width: parent.width
                height: parent.height / 2
                clip: true
                
                model: functionModel
                delegate: MathExpressionItem {
                    nameString: model.name + "(" + model.variable + ")"
                    valueString: model.value
                    typeString: model.typeString
                    onSelected: root.functionSelected(model.name)
                    onRemoved: root.functionRemoved(model.name)
                    onFocusChanged: root.rejectFocus()
                }
                
                section.property: "typeString"
                section.criteria: ViewSection.FullString
                section.delegate: Item {
                    width: parent.width
                    height: 20
                    
                    //imagePath: "widgets/listitem"
                    //prefix: "section"
                    
                    Text {
                        anchors.centerIn: parent
                        font.pixelSize: 12
                        text: section
                    }
                }
            }

    }
}
