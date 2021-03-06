#-------------------------------------------------
#
# Project created by QtCreator 2015-07-17T18:45:47
#
#-------------------------------------------------

QT       += core gui serialport

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Kodillo
TEMPLATE = app


SOURCES += main.cpp\
    src/codeeditor.cpp \
    src/editorwindow.cpp \
    src/highlighter.cpp \
    src/utils/autoformat.cpp \
    src/monitordialog.cpp

HEADERS  += editorwindow.h \
    src/codeeditor.h \
    src/editorwindow.h \
    src/highlighter.h \
    src/utils/autoformat.h \
    src/monitordialog.h

INCLUDEPATH += src utils
DEPENDPATH += src utils

FORMS    += editorwindow.ui \
    monitordialog.ui

RESOURCES += \
    images.qrc
