#-------------------------------------------------
#
# Project created by QtCreator 2015-03-02T11:14:37
#
#-------------------------------------------------

QT       += core gui serialport opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = serialPrueba1
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += /opt/local/include

LIBS += -L/opt/local/lib

LIBS += -lopencv_core \
-lopencv_highgui \
-lopencv_imgproc \
-lopencv_video \
