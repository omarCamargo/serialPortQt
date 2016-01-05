#-------------------------------------------------
#
# Project created by QtCreator 2015-03-02T11:14:37
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = serialPrueba1
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    serialportmanager.cpp

HEADERS  += mainwindow.h \
    serialportmanager.h

FORMS    += mainwindow.ui

