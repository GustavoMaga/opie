CONFIG += warn_on qt
 
TEMPLATE = lib
DESTDIR = $(OPIEDIR)/plugins/applets
TARGET  = example

SOURCES = simpleimpl.cpp
HEADERS = simpleimpl.h

INCLUDEPATH += $(OPIEDIR)/include
DEPENDSPATH += $(OPIEDIR)/include

LIBS += -lqpe -lopieui2


include( $(OPIEDIR)/include.pro ) 
