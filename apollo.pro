#-------------------------------------------------
#
# Project created by QtCreator 2016-03-21T23:10:03
#
#-------------------------------------------------

QT       += core gui
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = apollo
TEMPLATE = app
INCLUDEPATH += inc/

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/hardware.cpp \
    src/inputwindow.cpp \
    src/receiver.cpp \
    src/gamewindow.cpp

LIBS += -L/usr/lib/x86_64-linux-gnu -lboost_system

HEADERS  += \
    inc/hardware.hh \
    inc/inputwindow.hh \
    inc/mainwindow.hh \
    inc/blocking_reader.hh \
    inc/receiver.hh \
    inc/gamewindow.hh

FORMS    += ui/mainwindow.ui \
    ui/inputwindow.ui \
    ui/gamewindow.ui

DISTFILES +=

RESOURCES += \
    res/textures.qrc
