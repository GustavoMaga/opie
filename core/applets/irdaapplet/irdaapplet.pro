TEMPLATE  = lib
CONFIG    += qt warn_on release
HEADERS =   irda.h irdaappletimpl.h
SOURCES =   irda.cpp irdaappletimpl.cpp
TARGET    = irdaapplet
DESTDIR   = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include
LIBS            += -lqpe
VERSION   = 1.0.0

include ( $(OPIEDIR)/include.pro )
