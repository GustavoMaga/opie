CONFIG		= qt quick-app
HEADERS		= oyatzee.h
SOURCES		= main.cpp oyatzee.cpp
TARGET		= oyatzee
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2

include ( $(OPIEDIR)/include.pro )
