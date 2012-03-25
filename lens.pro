#-------------------------------------------------
#
# Project created by QtCreator 2012-02-09T17:30:25
#
#-------------------------------------------------

QT       += core gui

TARGET = lens
TEMPLATE = app

QMAKE_CXXFLAGS += -Wall -Wextra -Wold-style-cast -pedantic

SOURCES += main.cpp\
        mainwindow.cpp \
    renderwidget.cpp \
    rayemitter.cpp

HEADERS  += mainwindow.h \
    renderwidget.h \
    rayemitter.h

FORMS    += mainwindow.ui












