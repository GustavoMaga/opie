CONFIG		= qt warn_on  quick-app
HEADERS		= checkers.h \
		  echeckers.h \
		  field.h \
		  kcheckers.h \
		  rcheckers.h
SOURCES		= checkers.cpp \
		  echeckers.cpp \
		  field.cpp \
		  kcheckers.cpp \
		  main.cpp \
		  rcheckers.cpp
INTERFACES	=
INCLUDEPATH	+= $(OPIEDIR)/include
LIBS		+= -lqpe -lopiecore2
TARGET		= kcheckers


include( $(OPIEDIR)/include.pro )
