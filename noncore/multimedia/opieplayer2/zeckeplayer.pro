TEMPLATE	= app
DESTDIR         = .
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
HEADERS		= frame.h lib.h
SOURCES		= alphablend.c yuv2rgb.c nullvideo.o frame.cpp lib.cpp mainTest.cpp 
INCLUDEPATH	+= $(OPIEDIR)/include /opt/QtPalmtop/include
DEPENDPATH	+= $(OPIEDIR)/include /opt/QtPalmtop/include
LIBS            += -L/opt/QtPalmtop/lib -lxine -lxineutils
TARGET          = zeckeplayer


include ( $(OPIEDIR)/include.pro )
