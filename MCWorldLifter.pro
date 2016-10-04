#-------------------------------------------------
#
# Project created by QtCreator 2016-09-30T07:01:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MCWorldLifter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    region.cpp \
    chunk.cpp \
    chunklifter.cpp

HEADERS  += mainwindow.h \
    region.h \
    chunk.h \
    chunklifter.h \
    zconf.h \
    zlib.h

FORMS    += mainwindow.ui


LIBS += -L$$PWD/./ -lzlibstat

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

PRE_TARGETDEPS += $$PWD/./zlibstat.lib
