import QtQuick 1.1
import Sailfish.Silica 1.0

Page {
    id: page
    SilicaFlickable {
        id: listView
        anchors.fill: parent

        Column {
            width: page.width
            spacing: theme.paddingLarge
            PageHeader {
                title: "Settings"
            }
            Label {
                x: theme.paddingLarge
                text: "TODO"
                color: theme.secondaryHighlightColor
                font.pixelSize: theme.fontSizeLarge
            }
        }
    }
}





