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
    src/highlighter.cpp

HEADERS  += editorwindow.h \
    src/codeeditor.h \
    src/editorwindow.h \
    src/highlighter.h

INCLUDEPATH += src
DEPENDPATH += src

FORMS    += editorwindow.ui
