TEMPLATE  = lib
CONFIG    += qt warn_on release
HEADERS =   notes.h notesappletimpl.h
SOURCES =   notes.cpp notesappletimpl.cpp
TARGET    = notesapplet
DESTDIR   = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe
VERSION   = 1.0.0
MOC_DIR=opieobj
OBJECTS_DIR=opieobj



include ( ../../../include.pro )
