TEMPLATE = lib
CONFIG -= moc
CONFIG += qt plugin 

# Input
HEADERS = addressplugin.h addresspluginimpl.h addresspluginconfig.h  \
    addresspluginwidget.h
SOURCES = addressplugin.cpp addresspluginimpl.cpp addresspluginconfig.cpp \
    addresspluginwidget.cpp

INCLUDEPATH     += $(OPIEDIR)/include \
        ../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
        ../ ../library

LIBS+= -lqpe -lopiecore2 -lopieui2 -lopiepim2 -lqtaux2

DESTDIR = $(OPIEDIR)/plugins/today
TARGET = todayaddressbookplugin

include ( $(OPIEDIR)/include.pro )
