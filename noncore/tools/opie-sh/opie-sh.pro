TEMPLATE			=app
CONFIG				+=qt warn_on release
DESTDIR				=$(OPIEDIR)/bin
HEADERS				=mbox.h fviewer.h inputdialog.h
SOURCES				=opie-sh.cpp mbox.cpp fviewer.cpp inputdialog.cpp
INCLUDEPATH		+=$(OPIEDIR)/include
DEPENDPATH		+=$(OPIEDIR)/include
LIBS						+=-lqpe
TARGET                   = opie-sh

include ( $(OPIEDIR)/include.pro )
