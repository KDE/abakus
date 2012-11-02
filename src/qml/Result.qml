import QtQuick 1.1

Row {
    width: parent.width
    height: 20
    
    property string rowBackground: index % 2 == 0 ? "white" : "whitesmoke"
    property string highlightColor: "lightblue"

    Rectangle {
        id: expressionItem
        width: parent.width - resultItem.width - tagItem.width
        height: parent.height
        color: index == history.currentIndex ? highlightColor : rowBackground
        
        Text {
            height: parent.height
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 12
            color: "darkgrey"
            text: expression
            
            MouseArea {
                anchors.fill: parent
                onClicked: history.expressionSelected(expression)
                onDoubleClicked: console.log("TODO")
            }
        }
    }

    Rectangle {
        id: resultItem
        width: resultItemText.width
        height: parent.height
        color: index == history.currentIndex ? highlightColor : rowBackground
        
        Text {
            id: resultItemText
            height: parent.height
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 12
            text: result
            
            MouseArea  {
                anchors.fill: parent
                onClicked: history.resultSelected(result)
                onDoubleClicked: console.log("TODO")
            }
        }
    }
    
    Rectangle {
        id: tagItem
        width: tag == "" ? 0 : history.minTagSize
        height: parent.height
        color: index == history.currentIndex ? highlightColor : rowBackground
        
        Text {
            height: parent.height
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 12
            font.italic: true
            color: "darkgrey"
            text: tag
            
            MouseArea  {
                anchors.fill: parent
                onClicked: history.tagSelected(tag)
                onDoubleClicked: {}//do nothing
            }
        }
    }
    
    Text {
        id: tagItemDummy
        font.pixelSize: 12
        font.italic: true
        text: tag
        visible: false
    }
    
    Component.onCompleted: {
        mainWindow.addVisibleHistoryItemIndex(index)
        updateTagWidth()
    }
    
    Component.onDestruction:  {
            mainWindow.removeVisibleHistoryItemIndex(index)
            updateTagWidth()
    }
    
    function updateTagWidth() {
        var i = 0;
        var itemIndexNext = mainWindow.getVisibleHistoryItemIndex(i)
        var minTagSize = 0
        
        while(itemIndexNext > -1)
        {
            tagItemDummy.text = mainWindow.getTag(itemIndexNext)
            if(minTagSize < tagItemDummy.width)
            {
                minTagSize = tagItemDummy.width
            }
            ++i;
            itemIndexNext = mainWindow.getVisibleHistoryItemIndex(i)
        }
        
        tagItemDummy.text = ""
        history.minTagSize = minTagSize
    }
}
