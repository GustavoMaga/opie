TEMPLATE	= app
CONFIG		= qt warn_on 
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= inlineedit.h  filebrowser.h filePermissions.h
SOURCES		= filebrowser.cpp inlineedit.cpp filePermissions.cpp main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	=
TARGET	        = filebrowser

include( $(OPIEDIR)/include.pro )
