TEMPLATE	= app
CONFIG		= qt warn_on debug
#CONFIG		= qt warn_on release
HEADERS		= mainwindow.h          \
				timetabwidget.h     \
				formattabwidget.h   \
				settingstabwidget.h \
				predicttabwidget.h  \
				ntptabwidget.h
SOURCES		= main.cpp                \
				mainwindow.cpp        \
				timetabwidget.cpp     \
				formattabwidget.cpp   \
				settingstabwidget.cpp \
				predicttabwidget.cpp  \
				ntptabwidget.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS		+= -lqpe -lopie
INTERFACES	= 
DESTDIR		= $(OPIEDIR)/bin
TARGET		= systemtime

TRANSLATIONS = ../../../i18n/de/systemtime.ts \
	 ../../../i18n/nl/systemtime.ts \
	 ../../../i18n/da/systemtime.ts \
	 ../../../i18n/xx/systemtime.ts \
	 ../../../i18n/en/systemtime.ts \
	 ../../../i18n/es/systemtime.ts \
	 ../../../i18n/fr/systemtime.ts \
	 ../../../i18n/hu/systemtime.ts \
	 ../../../i18n/ja/systemtime.ts \
	 ../../../i18n/ko/systemtime.ts \
	 ../../../i18n/no/systemtime.ts \
	 ../../../i18n/pl/systemtime.ts \
	 ../../../i18n/pt/systemtime.ts \
	 ../../../i18n/pt_BR/systemtime.ts \
	 ../../../i18n/sl/systemtime.ts \
	 ../../../i18n/zh_CN/systemtime.ts \
	 ../../../i18n/zh_TW/systemtime.ts



include ( $(OPIEDIR)/include.pro )
