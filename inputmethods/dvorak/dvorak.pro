TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   dvorak.h \
	    dvorakimpl.h 
SOURCES	=   dvorak.cpp \
	    dvorakimpl.cpp
TARGET		= qdvorak
DESTDIR		= ../../plugins/inputmethods
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ../../launcher
LIBS            += -lqpe -L$(OPIEDIR)/plugins/inputmethods -lqpickboard
QMAKE_LFLAGS	+= -Wl,-rpath,/opt/QtPalmtop/plugins/inputmethods
VERSION		= 1.0.0

TRANSLATIONS = ../../i18n/de/libqdvorak.ts \
	 ../../i18n/en/libqdvorak.ts \
	 ../../i18n/es/libqdvorak.ts \
	 ../../i18n/fr/libqdvorak.ts \
	 ../../i18n/hu/libqdvorak.ts \
	 ../../i18n/ja/libqdvorak.ts \
	 ../../i18n/ko/libqdvorak.ts \
	 ../../i18n/no/libqdvorak.ts \
	 ../../i18n/pl/libqdvorak.ts \
	 ../../i18n/pt/libqdvorak.ts \
	 ../../i18n/pt_BR/libqdvorak.ts \
	 ../../i18n/sl/libqdvorak.ts \
	 ../../i18n/zh_CN/libqdvorak.ts \
	 ../../i18n/zh_TW/libqdvorak.ts
