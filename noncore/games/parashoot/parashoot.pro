CONFIG		+= qt warn_on  quick-app
HEADERS		= interface.h man.h cannon.h base.h bullet.h helicopter.h
SOURCES		= main.cpp interface.cpp man.cpp cannon.cpp base.cpp bullet.cpp helicopter.cpp
TARGET		= parashoot
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2



include( $(OPIEDIR)/include.pro )
