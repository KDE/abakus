TARGET = abakus

BUILD_DIR = build-harmattan
RCC_DIR = build-harmattan/RCCFiles
UI_DIR = build-harmattan/UICFiles
MOC_DIR = build-harmattan/MOCFiles
OBJECTS_DIR = build-harmattan/ObjFiles

CONFIG(debug, debug|release) {
    DESTDIR = "debug"
}
CONFIG(release, debug|release) {
    DESTDIR = "release"
}

DEFINES += QT_USE_FAST_CONCATENATION
DEFINES += ABAKUS_QTONLY

# Add more folders to ship with the application, here
folder_01.source = src/mobile/harmattan/qml
folder_01.target = .
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
#QML_IMPORT_PATH += /usr/lib/kde4/imports

QT += declarative

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
CONFIG += qdeclarative-boostable

QMAKE_CXXFLAGS += -fPIC -fvisibility=hidden -fvisibility-inlines-hidden
QMAKE_LFLAGS += -pie -rdynamic

LIBS += -L/usr/lib -lmpfr

INCLUDEPATH += src/common src/mobile/harmattan

!exists($$BUILD_DIR) system(mkdir $$BUILD_DIR)

include(qmake/bison/bison.pri)
BISONSOURCES = src/common/parser.yy
include(qmake/flex/flex.pri)
FLEXSOURCES = src/common/lexer.ll

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += src/mobile/harmattan/main.cpp \
    src/mobile/harmattan/mainwindow.cpp \
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

HEADERS += \
    src/mobile/harmattan/mainwindow.h \
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

# Please do not modify the following two lines. Required for deployment.
include(src/mobile/harmattan/qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

OTHER_FILES = \ 
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog

contains(MEEGO_EDITION,harmattan) {
    desktopfile.files = src/mobile/harmattan/abakus.desktop
    desktopfile.path = /usr/share/applications
    INSTALLS += desktopfile

    icon.files = pics/hi64-app-abakus.png
    icon.path = /usr/share/icons/hicolor/64x64/apps
    INSTALLS += icon

    splashfiles.files = src/mobile/harmattan/splash-portrait.png
    splashfiles.path = /opt/abakus/splash
    INSTALLS += splashfiles
}
