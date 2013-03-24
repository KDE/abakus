TARGET = abakus

BUILD_DIR = build-qt
RCC_DIR = build-qt/RCCFiles
UI_DIR = build-qt/UICFiles
MOC_DIR = build-qt/MOCFiles
OBJECTS_DIR = build-qt/ObjFiles

CONFIG(debug, debug|release) {
    DESTDIR = "debug"
}
CONFIG(release, debug|release) {
    DESTDIR = "release"
}

DEFINES += ABAKUS_QTONLY

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH += /usr/lib/kde4/imports

QT += declarative

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
# CONFIG += qdeclarative-boostable

LIBS += -L/usr/lib -lmpfr

INCLUDEPATH += src

include(qmake/bison/bison.pri)
BISONSOURCES = src/parser.yy
include(qmake/flex/flex.pri)
FLEXSOURCES = src/lexer.ll

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += src/main.cpp \
    src/settingscore.cpp \
    src/settings.cpp \
    src/resultmodelitem.cpp \
    src/resultmodel.cpp \
    src/result.cpp \
    src/numerictypes.cpp \
    src/numeralmodelitem.cpp \
    src/numeralmodel.cpp \
    src/node.cpp \
    src/mainwindow.cpp \
    src/functionmodelitem.cpp \
    src/functionmodel.cpp \
    src/function.cpp

# Please do not modify the following two lines. Required for deployment.
#include(qt/qmlapplicationviewer/qmlapplicationviewer.pri)
#qtcAddDeployment()

HEADERS += \
    src/sharedptr.h \
    src/settingscore.h \
    src/settings.h \
    src/resultmodelitem.h \
    src/resultmodel.h \
    src/result.h \
    src/numerictypes.h \
    src/numeralmodelitem.h \
    src/numeralmodel.h \
    src/node.h \
    src/mainwindow.h \
    src/functionmodelitem.h \
    src/functionmodel.h \
    src/function.h

qml.path = $$DESTDIR/qml
qml.files += qml-qt/*

INSTALLS += qml
