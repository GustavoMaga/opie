CONFIG		= qt warn_on  quick-app
HEADERS		= buzzword.h
SOURCES		= buzzword.cpp main.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2

TARGET 		= buzzword

include( $(OPIEDIR)/include.pro )
