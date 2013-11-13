import QtQuick 1.1

Rectangle {
    id: root
    color: mouseArea.pressed ? buttonColorHighlight : buttonColor
    radius: height / 4
    enabled: text.length != 0

    property alias text: label.text

    property int fontPointSize: 24
    property string buttonColor: "black"
    property string buttonColorHighlight: "orange"
    property string textColor: "white"
    property string textColorHighlight: "black"

    signal pressed()
    signal pressAndHold()

    Text {
        id: label
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: mouseArea.pressed ? root.textColorHighlight : root.textColor
        font.bold: true
        font.pixelSize: root.height / 1.8
    }

    Rectangle {
        visible: mouseArea.pressed
        width: label.width * 2 + 20
        height: label.height * 2 + 20
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.top
        anchors.bottomMargin: 15
        radius: parent.radius
        color: parent.color
        border.color: buttonColor
        border.width: 2

        Text {
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: label.color
            font.bold: label.font.bold
            font.pixelSize: label.font.pixelSize * 2
            text: label.text
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onClicked: root.pressed()
        onPressAndHold: root.pressAndHold()
    }
}
