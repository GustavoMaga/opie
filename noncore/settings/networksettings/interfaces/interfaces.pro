TEMPLATE        = lib
CONFIG          += qt warn_on 
#CONFIG		+= qt warn_on 
DESTDIR		= $(OPIEDIR)/lib$(PROJMAK)
HEADERS		= interface.h  interfaceinformationimp.h  interfaces.h  interfacesetupimp.h
SOURCES		= interface.cpp interfaces.cpp interfaceinformationimp.cpp  interfacesetupimp.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2
INTERFACES	= interfaceadvanced.ui  interfaceinformation.ui  interfacesetup.ui
TARGET		= interfaces
VERSION		= 1.0.0

include( $(OPIEDIR)/include.pro )
