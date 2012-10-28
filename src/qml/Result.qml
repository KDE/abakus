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
        if(history.count == 1) // in case the history was cleared, the count is 1 after the first element is completed
        {
            history.indexTop = 0
            history.indexBottom = 0
        }
        
        if(index == history.indexTop - 1)
        {
            history.indexTop = index
            updateTagWidth()
        }
        else if(index == history.indexBottom + 1)
        {
            history.indexBottom = index
            updateTagWidth()
        }
        else if(history.indexBottom < index)
        {
            if(history.indexBottom + 2 != index)   //as the latest element will no be destroyed, there will also not be a onCompleted event
            {
                history.indexTop = index - (history.indexBottom - history.indexTop)
            }
            history.indexBottom = index
            updateTagWidth()
        }
        else if(history.indexTop == history.indexBottom)
        {
            updateTagWidth()
        }
    }
    
    Component.onDestruction:  {
        if(index == history.indexTop)
        {
            history.indexTop = index + 1
            updateTagWidth()
        }
        else if(index == history.indexBottom
            || index == history.count - 2)     //the latest added item will no be destroyed
        {
            history.indexBottom = index - 1
            updateTagWidth()
        }
    }
    
    function updateTagWidth() {
        var indexTop = history.indexTop
        var indexBottom = history.indexBottom
        var minTagSize = 0
        
        for(var index = indexTop; index < indexBottom + 1; index++)
        {
            tagItemDummy.text = mainWindow.getTag(index)
            if(minTagSize < tagItemDummy.width)
            {
                minTagSize = tagItemDummy.width
            }
        }
        history.minTagSize = minTagSize
    }
}
