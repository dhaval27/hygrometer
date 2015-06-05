#-------------------------------------------------
#
# Project created by QtCreator 2015-04-20T12:01:48
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += xml
QT       += core


greaterThan(QT_MAJOR_VERSION, 4): QT +=widgets

TARGET = tcp_test
TEMPLATE = app

CONFIG+= static

SOURCES += main.cpp\
        mainwindow.cpp \
    device.cpp \
    devicelist.cpp

HEADERS  += mainwindow.h \
    device.h \
    devicelist.h

FORMS    += mainwindow.ui
