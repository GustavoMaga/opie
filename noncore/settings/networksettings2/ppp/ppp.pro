TEMPLATE        = lib
CONFIG          += qt warn_on release plugin
DESTDIR         = $(OPIEDIR)/plugins/networksettings2
HEADERS		= ppp_NN.h \
                  ppp_NNI.h \
                  PPPedit.h \
                  PPPAuthedit.h \
                  PPPDNSedit.h \
                  PPPRunedit.h \
                  PPPDialingedit.h \
                  PPPIPedit.h
SOURCES		= ppp_NN.cpp \
                  ppp_NNI.cpp \
                  PPPedit.cpp \
                  PPPDNSedit.cpp \
                  PPPRunedit.cpp \
                  PPPDialingedit.cpp \
                  PPPAuthedit.cpp \
                  ppprun.cpp \
                  PPPIPedit.cpp
INCLUDEPATH	+= $(OPIEDIR)/include ../ ../networksettings2
DEPENDPATH	+= $(OPIEDIR)/include ../ ../networksettings2
LIBS            += -lqpe
INTERFACES	= PPPGUI.ui \
                  PPPAuthGUI.ui \
                  PPPIPGUI.ui \
                  PPPDNSGUI.ui \
                  PPPDialingGUI.ui \
                  PPPRunGUI.ui
TARGET		= ppp
VERSION		= 1.0.0

include( $(OPIEDIR)/include.pro )
