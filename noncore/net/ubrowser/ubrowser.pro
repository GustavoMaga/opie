TEMPLATE	=app
CONFIG		+=qt warn_on release
DESTDIR		=../../../bin
HEADERS		= mainview.h httpfactory.h httpcomm.h
SOURCES		= main.cpp mainview.cpp httpfactory.cpp httpcomm.cpp
INCLUDEPATH	+=../../../include
DEPENDPATH	+=../../../include
LIBS		+= -lqpe
TARGET          = ubrowser



include ( ../../../include.pro )
