import QtQuick 1.1

Rectangle {
    id: root
    height: flickable.height + 20
    color: "black"

    signal pressed(string value)
    signal evaluate()

    Component.onCompleted: {
        genericPad.pressed.connect(pressed)
        genericPad.evaluate.connect(evaluate)
        logicPad.pressed.connect(pressed)
        logicPad.evaluate.connect(evaluate)
        trigPad.pressed.connect(pressed)
        trigPad.evaluate.connect(evaluate)

        flickable.contentX = root.width
        internal.animationDuration = 250
    }

    QtObject {
        id: internal

        property int spacing: 0
        property int buttonWidth: width / 5 - spacing
        property int buttonHeight: width / 8

        property int selectedKeyPad: 2
        property int keyPadSwipeTreshold: width / 4
        property int animationDuration: 0
    }

    Flickable {
        id: flickable
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        height: container.height
        contentWidth: container.width
        contentHeight: container.height
        flickableDirection: Flickable.HorizontalFlick
        pressDelay: 50

        Behavior on contentX { NumberAnimation { duration: internal.animationDuration; easing.type: Easing.OutQuad } }
        Behavior on height { NumberAnimation { duration: internal.animationDuration; easing.type: Easing.OutQuad } }

        onFlickStarted: {
            // this will stop flicking and an onMovementEnded event will be generated
            contentX = contentX
        }

        onMovementEnded: {
            switch(internal.selectedKeyPad) {
                case 1:
                    if(contentX > internal.keyPadSwipeTreshold)
                    {
                        internal.selectedKeyPad = 2
                        contentX = root.width
                        contentHeight = genericPad.height
                        container.height = genericPad.height
                    }
                    else
                    {
                        internal.selectedKeyPad = 1
                        contentX = 0
                    }
                    break
                case 2:
                    if(contentX < root.width - internal.keyPadSwipeTreshold)
                    {
                        internal.selectedKeyPad = 1
                        contentX = 0
                        contentHeight = logicPad.height
                        container.height = logicPad.height
                    }
                    else if(contentX > root.width + 50)
                    {
                        internal.selectedKeyPad = 3
                        contentX = 2 * root.width
                        contentHeight = trigPad.height
                        container.height = trigPad.height
                    }
                    else
                    {
                        internal.selectedKeyPad = 2
                        contentX = root.width
                    }
                    break
                case 3:
                    if(contentX < 2 * root.width - internal.keyPadSwipeTreshold)
                    {
                        internal.selectedKeyPad = 2
                        contentX = root.width
                        contentHeight = genericPad.height
                        container.height = genericPad.height
                    }
                    else
                    {
                        internal.selectedKeyPad = 3
                        contentX = 2 * root.width
                    }
                    break
            }
        }

        Item {
            id: container
            width: logicPad.width + genericPad.width +trigPad.width
            height: genericPad.height
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter

            KeyPadLogic {
                id: logicPad
                anchors.top: parent.top
                anchors.right: genericPad.left

                spacing: internal.spacing
                buttonWidth: internal.buttonWidth
                buttonHeight: internal.buttonHeight
            }
            KeyPadGeneric {
                id: genericPad
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter

                spacing: internal.spacing
                buttonWidth: internal.buttonWidth
                buttonHeight: internal.buttonHeight
            }
            KeyPadTrig {
                id: trigPad
                anchors.top: parent.top
                anchors.left: genericPad.right

                spacing: internal.spacing
                buttonWidth: internal.buttonWidth
                buttonHeight: internal.buttonHeight
            }
        }
    }

    Item {
        id: activePadIndicator
        height: 20
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        Row {
            anchors.centerIn: parent
            spacing: 10

            Rectangle {
                width: 10
                height: width
                radius: width / 2
                color: "orange"
                border.width: 5
                border.color: internal.selectedKeyPad === 1 ? "orange" : "transparent"
            }
            Rectangle {
                width: 10
                height: width
                radius: width / 2
                color: "orange"
                border.width: 5
                border.color: internal.selectedKeyPad === 2 ? "orange" : "transparent"
            }
            Rectangle {
                width: 10
                height: width
                radius: width / 2
                color: "orange"
                border.width: 5
                border.color: internal.selectedKeyPad === 3 ? "orange" : "transparent"
            }
        }
    }
}
