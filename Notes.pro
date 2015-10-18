#-------------------------------------------------
#
# Project created by QtCreator 2015-10-17T12:03:23
#
#-------------------------------------------------

QT += core gui widgets webkitwidgets


TARGET = Notes
TEMPLATE = app
CONFIG += c++11
CONFIG += warn_on


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    libraryhelperwindow.cpp \
    notetreeitem.cpp \
    notetreewidget.cpp \
    editor.cpp

HEADERS += \
    mainwindow.hpp \
    libraryhelperwindow.hpp \
    notetreeitem.hpp \
    notetreewidget.hpp \
    editor.hpp

FORMS += \
    mainwindow.ui \
    libraryhelperwindow.ui
