TEMPLATE    = lib
CONFIG      += qt plugin warn_on release
HEADERS     = zkbapplet.h\
            zkbwidget.h \
            ../../apps/keyz-cfg/zkbcfg.h  \
            ../../apps/keyz-cfg/zkbnames.h  \
            ../../apps/keyz-cfg/zkbxml.h  \
            ../../apps/keyz-cfg/zkb.h  
SOURCES     = zkbapplet.cpp \
	    zkbwidget.cpp \
            ../../apps/keyz-cfg/zkbcfg.cpp  \
            ../../apps/keyz-cfg/zkbnames.cpp  \
            ../../apps/keyz-cfg/zkbxml.cpp  \
            ../../apps/keyz-cfg/zkb.cpp  
TARGET      = zkbapplet
DESTDIR     = $(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include ../../apps/keyz-cfg
DEPENDPATH  += ../$(OPIEDIR)/include
VERSION     = 0.6.0
LIBS        += -lqpe

include ( $(OPIEDIR)/include.pro )
