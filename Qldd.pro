#-------------------------------------------------
#
# Project created by QtCreator 2014-10-03T15:18:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Qldd
TEMPLATE = app

macx {
    QMAKE_MAC_SDK = macosx10.15
}

SOURCES += main.cpp\
        mainwindow.cpp \
    qldd.cpp

HEADERS  += mainwindow.h \
    qldd.h

FORMS    += mainwindow.ui
