#CONFIG    += qt warn_on release quick-app
CONFIG    += qt warn_on release 
HEADERS   = opieftp.h inputDialog.h
SOURCES   = opieftp.cpp inputDialog.cpp main.cpp
TARGET    = opieftp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lftplib 
DESTDIR = $(OPIEDIR)/bin

include ( $(OPIEDIR)/include.pro )
