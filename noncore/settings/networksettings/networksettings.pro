CONFIG   = qt warn_on quick-app
HEADERS   = mainwindowimp.h addconnectionimp.h defaultmodule.h  module.h
SOURCES   = main.cpp mainwindowimp.cpp addconnectionimp.cpp
INCLUDEPATH += $(OPIEDIR)/include interfaces/ .
DEPENDPATH  += $(OPIEDIR)/include interfaces/ wlan/ ppp/
LIBS        += -lqpe -L$(OPIEDIR)/plugins/networksettings -Linterfaces/ -linterfaces -lopiecore2 -lopieui2
INTERFACES = mainwindow.ui addconnection.ui
TARGET    = networksettings


include ( $(OPIEDIR)/include.pro )
