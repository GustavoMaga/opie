#TEMPLATE        = app
#
TEMPLATE        = lib
#CONFIG          += qt warn_on release
CONFIG		+= qt warn_on debug
DESTDIR         = $(OPIEDIR)/plugins/networksettings
HEADERS		= pppmodule.h devices.h modem.h modeminfo.h pppdata.h kpppconfig.h pppdata.h runtests.h general.h modemcmds.h knuminput.h knumvalidator.h kpppwidget.h conwindow.h debug.h accounts.h connect.h edit.h scriptedit.h pppdargs.h  iplined.h pwentry.h pppconfig.h
SOURCES		= pppmodule.cpp modem.cpp modeminfo.cpp pppdata.cpp runtests.cpp general.cpp modemcmds.cpp knuminput.cpp knumvalidator.cpp kpppwidget.cpp  conwindow.cpp debug.cpp accounts.cpp connect.cpp edit.cpp scriptedit.cpp pppdargs.cpp  iplined.cpp pwentry.cpp pppconfig.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../ ../interfaces/
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -L../interfaces/ -linterfaces
INTERFACES	=  
TARGET		= pppplugin
VERSION		= 1.0.0



include ( $(OPIEDIR)/include.pro )
