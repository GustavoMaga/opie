DESTDIR         = $(OPIEDIR)/bin
TEMPLATE        = app
CONFIG          = warn_on debug
#CONFIG         = warn_on release
HEADERS         = 
SOURCES         = orinoco_hopper.c
INCLUDEPATH     +=
DEPENDPATH      +=
LIBS            +=
INTERFACES      = 
TARGET          = orinoco_hopper



include ( $(OPIEDIR)/include.pro )
