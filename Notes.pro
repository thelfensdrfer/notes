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
    src/main.cpp \
    src/mainwindow.cpp \
    src/library/libraryhelperwindow.cpp \
    src/notetree/notetreeitem.cpp \
    src/notetree/notetreewidget.cpp \
    src/editor/editor.cpp \
    src/editor/editorcolorscheme.cpp \
    src/editor/editorlatexhighlighter.cpp

HEADERS += \
    src/mainwindow.hpp \
    src/library/libraryhelperwindow.hpp \
    src/notetree/notetreeitem.hpp \
    src/notetree/notetreewidget.hpp \
    src/editor/editor.hpp \
    src/editor/editorcolorscheme.hpp \
    src/editor/editorlatexhighlighter.hpp

FORMS += \
    src/mainwindow.ui \
    src/library/libraryhelperwindow.ui

RESOURCES += \
    resources.qrc
