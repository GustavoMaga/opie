TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   keyboard.h \
            configdlg.h \
	    keyboardimpl.h 
SOURCES	=   keyboard.cpp \
            configdlg.cpp \
	    keyboardimpl.cpp
TARGET		= qmultikey
DESTDIR		= $(OPIEDIR)/plugins/inputmethods
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ../../launcher
LIBS            += -lqpe -L$(OPIEDIR)/plugins/inputmethods -lqpickboard
VERSION		= 1.0.0

TRANSLATIONS    = ../../i18n/pt_BR/libqmultikey.ts
TRANSLATIONS   += ../../i18n/de/libqmultikey.ts
TRANSLATIONS   += ../../i18n/en/libqmultikey.ts
TRANSLATIONS   += ../../i18n/hu/libqmultikey.ts
TRANSLATIONS   += ../../i18n/sl/libqmultikey.ts
TRANSLATIONS   += ../../i18n/ja/libqmultikey.ts
TRANSLATIONS   += ../../i18n/ko/libqmultikey.ts
TRANSLATIONS   += ../../i18n/pl/libqmultikey.ts
TRANSLATIONS   += ../../i18n/no/libqmultikey.ts
TRANSLATIONS   += ../../i18n/zh_CN/libqmultikey.ts
TRANSLATIONS   += ../../i18n/zh_TW/libqmultikey.ts
TRANSLATIONS   += ../../i18n/fr/libqmultikey.ts
TRANSLATIONS   += ../../i18n/da/libqmultikey.ts
