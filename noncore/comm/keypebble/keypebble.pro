TEMPLATE        = app
CONFIG          += qt warn_on release
DESTDIR         = $(OPIEDIR)/bin
HEADERS		= d3des.h \
		  krfbbuffer.h \
		  krfbcanvas.h \
		  krfbconnection.h \
		  krfbdecoder.h \
		  krfblogin.h \
		  krfboptions.h \
		  krfbserverinfo.h \
		  kvnc.h \
		  kvncconnectdlg.h \
		  kvncoptionsdlg.h \
		  version.h \
		  vncauth.h
SOURCES		= d3des.c \
		  vncauth.c \
		  krfbbuffer.cpp \
		  krfbcanvas.cpp \
		  krfbconnection.cpp \
		  krfbdecoder.cpp \
		  krfblogin.cpp \
		  krfboptions.cpp \
		  kvnc.cpp \
		  kvncconnectdlg.cpp \
		  kvncoptionsdlg.cpp \
		  main.cpp
INTERFACES	= vncoptionsbase.ui
TARGET          = keypebble
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../../../i18n/de/keypebble.ts \
	 ../../../i18n/en/keypebble.ts \
	 ../../../i18n/es/keypebble.ts \
	 ../../../i18n/fr/keypebble.ts \
	 ../../../i18n/hu/keypebble.ts \
	 ../../../i18n/ja/keypebble.ts \
	 ../../../i18n/ko/keypebble.ts \
	 ../../../i18n/no/keypebble.ts \
	 ../../../i18n/pl/keypebble.ts \
	 ../../../i18n/pt/keypebble.ts \
	 ../../../i18n/pt_BR/keypebble.ts \
	 ../../../i18n/sl/keypebble.ts \
	 ../../../i18n/zh_CN/keypebble.ts \
	 ../../../i18n/zh_TW/keypebble.ts
