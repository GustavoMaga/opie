TEMPLATE	= lib
CONFIG		+= qt plugin warn_on 
HEADERS		= polished.h
SOURCES		= polished.cpp
TARGET		= polished
DESTDIR		= $(OPIEDIR)/plugins/decorations
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe
VERSION		= 1.0.0




include ( $(OPIEDIR)/include.pro )
