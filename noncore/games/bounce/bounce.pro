TEMPLATE	= app
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
HEADERS		= game.h kbounce.h 
SOURCES		= game.cpp kbounce.cpp main.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS        += -lqpe
DESTDIR     = $(OPIEDIR)/bin
TARGET		= bounce 

include ( $(OPIEDIR)/include.pro )
