# The name of your app.
# NOTICE: name defined in TARGET has a corresponding QML filename.
#         If name defined in TARGET is changed, following needs to be
#         done to match new name:
#         - corresponding QML filename must be changed
#         - desktop icon filename must be changed
#         - desktop filename must be changed
#         - icon definition filename in desktop file must be changed
TARGET = abakus

DEFINES += ABAKUS_QTONLY

CONFIG += sailfishapp

LIBS += -L/usr/lib -lmpfr

INCLUDEPATH += ../../common

SOURCES += main.cpp \
    mainobject.cpp \
    lexer.ll.cpp \
    parser.yy.cpp \
    ../../common/settingscore.cpp \
    ../../common/settings.cpp \
    ../../common/resultmodelitem.cpp \
    ../../common/resultmodel.cpp \
    ../../common/result.cpp \
    ../../common/numerictypes.cpp \
    ../../common/numeralmodelitem.cpp \
    ../../common/numeralmodel.cpp \
    ../../common/node.cpp \
    ../../common/functionmodelitem.cpp \
    ../../common/functionmodel.cpp \
    ../../common/function.cpp

HEADERS += \
    mainobject.h \
    parser.yy.hpp \
    ../../common/sharedptr.h \
    ../../common/settingscore.h \
    ../../common/settings.h \
    ../../common/resultmodelitem.h \
    ../../common/resultmodel.h \
    ../../common/result.h \
    ../../common/numerictypes.h \
    ../../common/numeralmodelitem.h \
    ../../common/numeralmodel.h \
    ../../common/node.h \
    ../../common/functionmodelitem.h \
    ../../common/functionmodel.h \
    ../../common/function.h

OTHER_FILES += qml/* \
    rpm/abakus.spec \
    rpm/abakus.yaml \
    abakus.desktop
