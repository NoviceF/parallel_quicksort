#-------------------------------------------------
#
# Project created by QtCreator 2016-01-16T22:36:53
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = parallel_qsort
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = app


SOURCES += main.cpp \
    manager.cpp \
    profiler.cpp \
    commandpromptparser.cpp \
    logger.cpp

HEADERS += \
    manager.h \
    profiler.h \
    sortimpl.h \
    commandpromptparser.h \
    sort.h \
    sorttest.h \
    logger.h

#LIBS += -L/usr/lib -llibboost_program_options

unix:!macx: LIBS += -lboost_program_options -lpthread
