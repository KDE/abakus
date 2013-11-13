import QtQuick 1.1

Grid {
    id: root

    property int buttonWidth: 40
    property int buttonHeight: 20

    signal pressed(string value)
    signal evaluate()

    columns: 5
    spacing: 0

    // first row
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "<<"
        onPressed: root.pressed("<<")
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: ">>"
        onPressed: root.pressed(">>")
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: ""
        onPressed: root.pressed(text)
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: ""
        onPressed: root.pressed(text)
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: ""
        onPressed: root.pressed(text)
    }

    // second row
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "7"
        onPressed: root.pressed(text)
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "8"
        onPressed: root.pressed(text)
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "9"
        onPressed: root.pressed(text)
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "x^y"
        onPressed: root.pressed("^")
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: ""
        onPressed: root.pressed(text)
    }

    // third row
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "4"
        onPressed: root.pressed(text)
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "5"
        onPressed: root.pressed(text)
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "6"
        onPressed: root.pressed(text)
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "×"
        onPressed: root.pressed("*")
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "÷"
        onPressed: root.pressed("/")
    }

    // fourth row
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "1"
        onPressed: root.pressed(text)
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "2"
        onPressed: root.pressed(text)
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "3"
        onPressed: root.pressed(text)
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "+"
        onPressed: root.pressed(text)
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "-"
        onPressed: root.pressed(text)
    }

    // fifth row
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "0"
        onPressed: root.pressed(text)
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: ""
        onPressed: root.pressed(text)
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "10^x"
        onPressed: root.pressed("*10^")
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "ans"
        onPressed: root.pressed(text)
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "="
        textColor: "orange"
        onPressed: root.evaluate()
    }
}
