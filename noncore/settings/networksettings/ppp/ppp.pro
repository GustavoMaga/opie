TEMPLATE        = lib
CONFIG          += qt warn_on release
#CONFIG		+= qt warn_on debug
DESTDIR         = $(OPIEDIR)/plugins/networksetup
HEADERS		= pppimp.h pppmodule.h
SOURCES		= pppimp.cpp pppmodule.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../ ../interfaces/
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -L../interfaces/ -linterfaces
INTERFACES	= ppp.ui
TARGET		= pppplugin
VERSION		= 1.0.0



include ( ../../../../include.pro )
