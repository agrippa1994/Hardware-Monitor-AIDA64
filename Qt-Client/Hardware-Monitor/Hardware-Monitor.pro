#-------------------------------------------------
#
# Project created by QtCreator 2014-12-11T20:35:10
#
#-------------------------------------------------

QT       += core gui websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Hardware-Monitor
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    aida64.cpp \
    SettingsWindow.cpp

HEADERS  += MainWindow.h \
    aida64.h \
    SettingsWindow.h

FORMS    += MainWindow.ui \
    SettingsWindow.ui
