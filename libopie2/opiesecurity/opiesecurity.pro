TEMPLATE    = lib
CONFIG      += qt warn_on
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = multiauthcommon.h \
              multiauthmainwindow.h \
              multiauthconfigwidget.h \
              multiauthplugininterface.h \
	      multiauthpassword.h
SOURCES     = multiauthcommon.cpp \
              multiauthmainwindow.cpp \
              multiauthpassword.cpp
TARGET      = opiesecurity2
VERSION     = 0.0.2
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lopiepim2

include( $(OPIEDIR)/include.pro )
