TEMPLATE	= lib
CONFIG		+= qt plugin warn_on 
HEADERS		= flat.h
SOURCES		= flat.cpp
TARGET		= flatstyle
DESTDIR		= $(OPIEDIR)/plugins/styles
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe
VERSION		= 1.0.0




include ( $(OPIEDIR)/include.pro )
