CONFIG += qt quick-app
HEADERS = sshkeys.h
SOURCES = main.cpp sshkeys.cpp
TARGET = sshkeys
INTERFACES = sshkeysbase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH += $(OPIEDIR)/include
LIBS += -lqpe -lopiecore2

include( $(OPIEDIR)/include.pro )
