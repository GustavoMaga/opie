TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= calibrate.h
SOURCES		= calibrate.cpp main.cpp
INCLUDEPATH	+= $(QTDIR)/include $(OPIEDIR)/include
DEPENDPATH	+= $(QTDIR)/include $(OPIEDIR)/include
TARGET		= calibrate
LIBS		+= -lqpe 

TRANSLATIONS = ../../../i18n/de/calibrate.pro.in.ts \
	 ../../../i18n/nl/calibrate.pro.in.ts \
	 ../../../i18n/da/calibrate.pro.in.ts \
	 ../../../i18n/xx/calibrate.pro.in.ts \
	 ../../../i18n/en/calibrate.pro.in.ts \
	 ../../../i18n/es/calibrate.pro.in.ts \
	 ../../../i18n/fr/calibrate.pro.in.ts \
	 ../../../i18n/hu/calibrate.pro.in.ts \
	 ../../../i18n/ja/calibrate.pro.in.ts \
	 ../../../i18n/ko/calibrate.pro.in.ts \
	 ../../../i18n/no/calibrate.pro.in.ts \
	 ../../../i18n/pl/calibrate.pro.in.ts \
	 ../../../i18n/pt/calibrate.pro.in.ts \
	 ../../../i18n/pt_BR/calibrate.pro.in.ts \
	 ../../../i18n/sl/calibrate.pro.in.ts \
	 ../../../i18n/zh_CN/calibrate.pro.in.ts \
	 ../../../i18n/it/calibrate.pro.in.ts \
	 ../../../i18n/zh_TW/calibrate.pro.in.ts

include ( $(OPIEDIR)/include.pro )
