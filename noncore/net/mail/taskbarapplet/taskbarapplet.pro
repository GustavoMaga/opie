TEMPLATE	= lib
CONFIG		+=	qt plugin warn_on 
HEADERS		+=	mailapplet.h \
			mailappletimpl.h 
SOURCES		+=	mailapplet.cpp \
			mailappletimpl.cpp 
INCLUDEPATH += $(OPIEDIR)/include $(OPIEDIR)/noncore/net/mail
LIBS		+=	-lmailwrapper -lqpe -lopiecore2
TARGET		=	mailapplet
DESTDIR		+=	$(OPIEDIR)/plugins/applets/

include( $(OPIEDIR)/include.pro )
