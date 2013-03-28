import QtQuick 1.1
import Sailfish.Silica 1.0

Column {
    id: root
    spacing: 4

    property int buttonHeight: width / 8

    signal pressed(string value)
    signal evaluate()

    Row {
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "sin"
            onClicked: root.pressed("sin(")
        }
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "cos"
            onClicked: root.pressed("cos(")
        }
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "π"
            onClicked: root.pressed("pi")
        }
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "("
            onClicked: root.pressed(text)
        }
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: ")"
            onClicked: root.pressed(text)
        }
    }

    Row {
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "7"
            onClicked: root.pressed(text)
        }
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "8"
            onClicked: root.pressed(text)
        }
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "9"
            onClicked: root.pressed(text)
        }
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "x²"
            onClicked: root.pressed("TODO")
        }
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "√"
            onClicked: root.pressed("sqrt(")
        }
    }

    Row {
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "4"
            onClicked: root.pressed(text)
        }
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "5"
            onClicked: root.pressed(text)
        }
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "6"
            onClicked: root.pressed(text)
        }
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "×"
            onClicked: root.pressed("*")
        }
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "÷"
            onClicked: root.pressed("/")
        }
    }

    Row {
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "1"
            onClicked: root.pressed(text)
        }
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "2"
            onClicked: root.pressed(text)
        }
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "3"
            onClicked: root.pressed(text)
        }
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "+"
            onClicked: root.pressed(text)
        }
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "-"
            onClicked: root.pressed(text)
        }
    }

    Row {
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "0"
            onClicked: root.pressed(text)
        }
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "."
            onClicked: root.pressed(text)
        }
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "10^x"
            onClicked: root.pressed("*10^")
        }
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "ans"
            onClicked: root.pressed(text)
        }
        Button {
            width: root.width / 5
            height: root.buttonHeight
            text: "="
            onClicked: root.evaluate()
        }
    }
}
