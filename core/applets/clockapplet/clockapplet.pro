TEMPLATE	= lib
CONFIG		+= qt plugin warn_on 
HEADERS		= clock.h
SOURCES		= clock.cpp
TARGET		= clockapplet
DESTDIR		= $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      +=  ..
LIBS            += -lqpe
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
