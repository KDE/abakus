import QtQuick 1.1

Item {
    
    Loader {
        id: displayLoader
        anchors.fill: parent
    }
    
    Timer {
        id: delayedLoadingTimer
        interval: 25
        running: false
        repeat: false
        onTriggered: displayLoader.source = "MainView.qml"
    }
    
    Component.onCompleted: delayedLoadingTimer.running = true
}
