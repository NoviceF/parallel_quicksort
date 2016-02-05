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
    profiler.cpp \
    commandpromptparser.cpp \
    logger.cpp \
    posixsortmanager.cpp

HEADERS += \
    profiler.h \
    commandpromptparser.h \
    sort.h \
    sorttest.h \
    logger.h \
    quicksortimpl.h \
    posixsortmanager.h \
    sortbase.h \
    csort.h \
    stlsort.h \
    posixparallelsort.h \
    cpp11parallelsortasync.h \
    cpp11parallelsortlockfreestack.h \
    lockfreestack.h \
    blockingthreadsafestack.h \
    cpp11parallelsortpartitioning.h

#LIBS += -L/usr/lib -llibboost_program_options
INCLUDEPATH += $$PWD/deps/include

unix:!macx: LIBS += -Ldeps/lib -lboost_program_options -lpthread
