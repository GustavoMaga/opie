TEMPLATE  = app
CONFIG    = qt warn_on release
HEADERS   = inputDialog.h helpwindow.h
SOURCES   = inputDialog.cpp helpwindow.cpp main.cpp
INTERFACES  = 
TARGET    = stockticker
INCLUDEPATH     += $(OPIEDIR)/include 
DEPENDPATH      += $(OPIEDIR)/include
LIBS+= -lqpe -lopie
DESTDIR = $(OPIEDIR)/bin

include ( $(OPIEDIR)/include.pro )
