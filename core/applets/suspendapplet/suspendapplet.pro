TEMPLATE      = lib
CONFIG       += qt plugin warn_on 
HEADERS       = suspend.h
SOURCES       = suspend.cpp
TARGET        = suspendapplet
DESTDIR       = $(OPIEDIR)/plugins/applets
INCLUDEPATH  += $(OPIEDIR)/include
DEPENDPATH   += $(OPIEDIR)/include
LIBS         += -lqpe -lopiecore2
VERSION       = 1.0.0

include( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
