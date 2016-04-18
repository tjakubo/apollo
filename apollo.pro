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


SOURCES += main.cpp\
        mainwindow.cpp \
    hardware.cpp \
    inputwindow.cpp

LIBS += -L/usr/lib/x86_64-linux-gnu -lboost_system

HEADERS  += mainwindow.h \
    hardware.h \
    inputwindow.h

FORMS    += mainwindow.ui \
    inputwindow.ui
