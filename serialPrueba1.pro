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
    serialportmanager.cpp \
    signalprocessor.cpp

HEADERS  += mainwindow.h \
    serialportmanager.h \
    signalprocessor.h

FORMS    += mainwindow.ui


win32:{
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../Qwt-6.1.2/lib/ -lqwt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../Qwt-6.1.2/lib/ -lqwtd
INCLUDEPATH += $$PWD/../../../../../Qwt-6.1.2/include
DEPENDPATH += $$PWD/../../../../../Qwt-6.1.2/include
}


unix:!macx{
unix:!macx: LIBS += -L$$PWD/../../../../usr/local/qwt-6.1.2/lib/ -lqwt

INCLUDEPATH += $$PWD/../../../../usr/local/qwt-6.1.2/include
DEPENDPATH += $$PWD/../../../../usr/local/qwt-6.1.2/include

}


RESOURCES += \
    resources.qrc


