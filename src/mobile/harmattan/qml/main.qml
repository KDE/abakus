import QtQuick 1.1

Item {
    id: appWindow
    // the resize mode of the QDeclarativeView is set to SizeRootObjectToView
    // but is has no effect if width and height are not set with dummy values
    width: 1
    height: 1

    MainPage {
        id: mainPage
        width: parent.height
        height: parent.width
        anchors.centerIn: parent
        rotation: -90
    }
}
