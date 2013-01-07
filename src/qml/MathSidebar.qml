import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents

Item {
    id: root
    
    property alias tabBarHeight: sidebarTabBar.height
    
    signal numeralSelected(string numeralName)
    signal numeralRemoved(string numeralName)
    
    signal functionSelected(string functionName)
    signal functionRemoved(string functionName)
    
    PlasmaComponents.TabBar {
        id: sidebarTabBar
        width: parent.width
        anchors.right: parent.right
        anchors.top: parent.top
        PlasmaComponents.TabButton { id: numeralsTabButton; tab: numeralsTab; text: i18n("Numerals") }
        PlasmaComponents.TabButton { id: functionsTabButton; tab: functionsTab; text: i18n("Functions") }
    }
    
    PlasmaComponents.TabGroup {
        id: sidebarTabGroup
        width: parent.width
        anchors.top: sidebarTabBar.bottom
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        
        PlasmaComponents.Page {
            id: numeralsTab
            anchors.fill: parent
            
            ListView {
                id: numerals
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.right: numeralsScrollBar.visible ? numeralsScrollBar.left : parent.right
                clip: true
                
                model: numeralModel
                delegate: NumeralViewItem {
                    onNumeralSelected: root.numeralSelected(numeralName)
                    onNumeralRemoved: root.numeralRemoved(numeralName)
                }
                
                section.property: "typeString"
                section.criteria: ViewSection.FullString
                section.delegate: Rectangle {
                    color: "lightsteelblue"
                    width: parent.width
                    height: 20
                    Text {
                        anchors.centerIn: parent
                        font.pixelSize: 12
                        text: section
                    }
                }
            }
            
            PlasmaComponents.ScrollBar {
                id: numeralsScrollBar
                flickableItem: numerals
                anchors.right: parent.right
            }
        }
        
        PlasmaComponents.Page {
            id: functionsTab
            anchors.fill: parent
            
            ListView {
                id: functions
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.right: functionsScrollBar.visible ? functionsScrollBar.left : parent.right
                clip: true
                
                model: functionModel
                delegate: FunctionViewItem {
                    onFunctionSelected: root.functionSelected(functionName)
                    onFunctionRemoved: root.functionRemoved(functionName)
                }
                
                section.property: "typeString"
                section.criteria: ViewSection.FullString
                section.delegate: Rectangle {
                    color: "lightsteelblue"
                    width: parent.width
                    height: 20
                    Text {
                        anchors.centerIn: parent
                        font.pixelSize: 12
                        text: section
                    }
                }
            }
            
            PlasmaComponents.ScrollBar {
                id: functionsScrollBar
                flickableItem: functions
                anchors.right: parent.right
            }
        }
    }
    
    function numeralsVisibleChanged(visible) {
        if(!visible && sidebarTabBar.currentTab == numeralsTabButton && functionsTabButton.visible) {
            sidebarTabBar.currentTab = functionsTabButton
            sidebarTabGroup.currentTab = functionsTab    
        }
        
        if(visible) {
            sidebarTabBar.currentTab = numeralsTabButton
            sidebarTabGroup.currentTab = numeralsTab
        }
        
        numeralsTabButton.visible = visible
        numeralsTabButton.opacity = visible ? 1 : 0
        
        if(numeralsTabButton.visible && functionsTabButton.visible) {
            sidebarTabBar.anchors.bottom = undefined
            sidebarTabBar.anchors.top = root.top
        }
        else {
            sidebarTabBar.anchors.top = undefined
            sidebarTabBar.anchors.bottom = root.top
        }
        
        if(numeralsTabButton.visible || functionsTabButton.visible) {
            root.anchors.left = undefined
            root.anchors.right = baseItem.right
        }
        else {
            root.anchors.right = undefined
            root.anchors.left = baseItem.right
        }
    }
    
    function functionsVisibleChanged(visible) {
        if(!visible && sidebarTabBar.currentTab == functionsTabButton && numeralsTabButton.visible) {
            sidebarTabBar.currentTab = numeralsTabButton
            sidebarTabGroup.currentTab = numeralsTab
            
        }
        
        if(visible) {
            sidebarTabBar.currentTab = functionsTabButton
            sidebarTabGroup.currentTab = functionsTab
        }
        
        functionsTabButton.visible = visible
        functionsTabButton.opacity = visible ? 1 : 0
        
        if(numeralsTabButton.visible && functionsTabButton.visible) {
            sidebarTabBar.anchors.bottom = undefined
            sidebarTabBar.anchors.top = root.top
        }
        else {
            sidebarTabBar.anchors.top = undefined
            sidebarTabBar.anchors.bottom = root.top
        }
        
        if(numeralsTabButton.visible || functionsTabButton.visible) {
            root.anchors.left = undefined
            root.anchors.right = baseItem.right
        }
        else {
            root.anchors.right = undefined
            root.anchors.left = baseItem.right
        }
    }
}