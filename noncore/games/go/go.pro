CONFIG		= qt warn_on release quick-app
HEADERS		= amigo.h \
		  go.h \
		  goplayutils.h \
		  gowidget.h
SOURCES		= amigo.c \
		goplayer.c \
		goplayutils.c \
		killable.c \
		gowidget.cpp \
		main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= go



include ( $(OPIEDIR)/include.pro )
