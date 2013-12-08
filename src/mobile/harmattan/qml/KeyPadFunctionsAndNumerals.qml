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
        text: "ans"
        onPressed: root.pressed(text)
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "e"
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
        text: "sin"
        onPressed: root.pressed("sin(")
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "cos"
        onPressed: root.pressed("cos(")
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "tan"
        onPressed: root.pressed("tan(")
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

    // third row
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "abs"
        onPressed: root.pressed("abs(")
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
        text: "ln"
        onPressed: root.pressed("ln(")
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "log"
        onPressed: root.pressed("log(")
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "eˣ"
        onPressed: root.pressed("exp(")
    }

    // fourth row
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "round"
        onPressed: root.pressed("round(")
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "ceil"
        onPressed: root.pressed("ceil(")
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "floor"
        onPressed: root.pressed("floor(")
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "int"
        onPressed: root.pressed("int(")
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "frac"
        onPressed: root.pressed("frac(")
    }

    // fifth row
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "hex"
        onPressed: root.pressed("hex(")
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "bin"
        onPressed: root.pressed("bin(")
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "dec"
        onPressed: root.pressed("dec(")
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: "oct"
        onPressed: root.pressed("oct(")
    }
    Button {
        width: root.buttonWidth
        height: root.buttonHeight
        text: ""
        textColor: "orange"
        onPressed: root.evaluate()
    }
}
