TEMPLATE      = lib
CONFIG       += qt plugin warn_on 
HEADERS       = home.h
SOURCES       = home.cpp
TARGET        = homeapplet
DESTDIR       = $(OPIEDIR)/plugins/applets
INCLUDEPATH  += $(OPIEDIR)/include
DEPENDPATH   += $(OPIEDIR)/include
LIBS         += -lqpe
VERSION       = 1.0.0

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
