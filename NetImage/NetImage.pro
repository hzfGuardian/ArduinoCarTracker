#-------------------------------------------------
#
# Project created by QtCreator 2016-05-11T22:02:38
#
#-------------------------------------------------

QT       += core gui network

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NetImage
TEMPLATE = app
CONFIG += c++11
CONFIG -= app_bundle

SOURCES += main.cpp\
        mainwindow.cpp \
    KernelBasedTracker.cpp \
    timer.cpp

INCLUDEPATH += /usr/local/include

LIBS += -L/usr/local/lib \
        -lopencv_highgui \
        -lopencv_core \
        -lopencv_imgproc \
        -lopencv_objdetect

HEADERS  += mainwindow.h \
    KernelBasedTracker.h \
    Histogram.h \
    timer.h

FORMS    += mainwindow.ui

CONFIG += mobility
MOBILITY = 

