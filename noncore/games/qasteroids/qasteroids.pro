TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= ledmeter.h  sprites.h  toplevel.h  view.h
SOURCES		= ledmeter.cpp  toplevel.cpp  view.cpp main.cpp
TARGET		= qasteroids
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe

include ( $(OPIEDIR)/include.pro )
