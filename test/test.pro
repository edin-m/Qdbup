#-------------------------------------------------
#
# Project created by QtCreator 2016-06-05T19:31:33
#
#-------------------------------------------------

QT       += sql testlib

QT       -= gui

TARGET = qdbup_tablecolumn
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    main.cpp \
    testqdbup.cpp \
    testmodels.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    testqdbup.h \
    testmodels.h

INCLUDEPATH += $$PWD/../src
INCLUDEPATH += $$PWD/../include

include($$PWD/../src/src.pri)
