import QtQuick 1.1

Rectangle {
    id: root
    height: grid.height
    color: "black"

    signal pressed(string value)
    signal evaluate()

    QtObject {
        id: internal

        property int spacing: 0
        property int buttonWidth: width / 5 - spacing
        property int buttonHeight: width / 8
    }

    Grid {
        id: grid
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        columns: 5
        spacing: internal.spacing

        // first row
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "sin"
            onPressed: root.pressed("sin(")
        }
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "cos"
            onPressed: root.pressed("cos(")
        }
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "π"
            onPressed: root.pressed("pi")
        }
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "("
            onPressed: root.pressed(text)
        }
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: ")"
            onPressed: root.pressed(text)
        }

        // second row
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "7"
            onPressed: root.pressed(text)
        }
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "8"
            onPressed: root.pressed(text)
        }
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "9"
            onPressed: root.pressed(text)
        }
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "x²"
            onPressed: root.pressed("^2")
        }
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "√"
            onPressed: root.pressed("sqrt(")
        }

        // third row
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "4"
            onPressed: root.pressed(text)
        }
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "5"
            onPressed: root.pressed(text)
        }
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "6"
            onPressed: root.pressed(text)
        }
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "×"
            onPressed: root.pressed("*")
        }
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "÷"
            onPressed: root.pressed("/")
        }

        // fourth row
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "1"
            onPressed: root.pressed(text)
        }
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "2"
            onPressed: root.pressed(text)
        }
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "3"
            onPressed: root.pressed(text)
        }
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "+"
            onPressed: root.pressed(text)
        }
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "-"
            onPressed: root.pressed(text)
        }

        // fifth row
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "0"
            onPressed: root.pressed(text)
        }
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "."
            onPressed: root.pressed(text)
        }
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "10^x"
            onPressed: root.pressed("*10^")
        }
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "ans"
            onPressed: root.pressed(text)
        }
        Button {
            width: internal.buttonWidth
            height: internal.buttonHeight
            text: "="
            textColor: "orange"
            onPressed: root.evaluate()
        }
    }
}
