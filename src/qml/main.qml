import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents

Item {
    
    PlasmaComponents.BusyIndicator {
        anchors.centerIn: parent
        visible: displayLoader.status != Loader.Ready
        running: visible
    }
    
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
