#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release quick-app
HEADERS		= game.h kbounce.h 
SOURCES		= game.cpp kbounce.cpp main.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS        += -lqpe
TARGET		= bounce 

TRANSLATIONS = ../../../i18n/de/bounce.ts \
	 ../../../i18n/nl/bounce.ts \
	 ../../../i18n/da/bounce.ts \
	 ../../../i18n/xx/bounce.ts \
	 ../../../i18n/en/bounce.ts \
	 ../../../i18n/es/bounce.ts \
	 ../../../i18n/fr/bounce.ts \
	 ../../../i18n/hu/bounce.ts \
	 ../../../i18n/ja/bounce.ts \
	 ../../../i18n/ko/bounce.ts \
	 ../../../i18n/no/bounce.ts \
	 ../../../i18n/pl/bounce.ts \
	 ../../../i18n/pt/bounce.ts \
	 ../../../i18n/pt_BR/bounce.ts \
	 ../../../i18n/sl/bounce.ts \
	 ../../../i18n/zh_CN/bounce.ts \
	 ../../../i18n/zh_TW/bounce.ts \
   	 ../../../i18n/it/bounce.ts



include ( $(OPIEDIR)/include.pro )
