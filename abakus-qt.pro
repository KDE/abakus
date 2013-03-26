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
#QML_IMPORT_PATH += /usr/lib/kde4/imports

QT += declarative

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
# CONFIG += qdeclarative-boostable

LIBS += -L/usr/lib -lmpfr

INCLUDEPATH += src/common src/desktop

!exists($$BUILD_DIR) system(mkdir $$BUILD_DIR)

include(qmake/bison/bison.pri)
BISONSOURCES = src/common/parser.yy
include(qmake/flex/flex.pri)
FLEXSOURCES = src/common/lexer.ll

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += src/desktop/main.cpp \
    src/desktop/mainwindow.cpp \
    src/common/settingscore.cpp \
    src/common/settings.cpp \
    src/common/resultmodelitem.cpp \
    src/common/resultmodel.cpp \
    src/common/result.cpp \
    src/common/numerictypes.cpp \
    src/common/numeralmodelitem.cpp \
    src/common/numeralmodel.cpp \
    src/common/node.cpp \
    src/common/functionmodelitem.cpp \
    src/common/functionmodel.cpp \
    src/common/function.cpp

# Please do not modify the following two lines. Required for deployment.
#include(qt/qmlapplicationviewer/qmlapplicationviewer.pri)
#qtcAddDeployment()

HEADERS += \
    src/desktop/mainwindow.h \
    src/common/sharedptr.h \
    src/common/settingscore.h \
    src/common/settings.h \
    src/common/resultmodelitem.h \
    src/common/resultmodel.h \
    src/common/result.h \
    src/common/numerictypes.h \
    src/common/numeralmodelitem.h \
    src/common/numeralmodel.h \
    src/common/node.h \
    src/common/functionmodelitem.h \
    src/common/functionmodel.h \
    src/common/function.h

qml.path = $$DESTDIR/qml
qml.files += src/qml-qt/*

INSTALLS += qml
