CONFIG		= qt warn_on release quick-app
HEADERS		= odict.h \
				searchmethoddlg.h \
				configdlg.h \
				dingwidget.h 

SOURCES		= main.cpp \
		  odict.cpp \
		  searchmethoddlg.cpp \
		  configdlg.cpp \
		  dingwidget.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS		+= -lqpe -lstdc++ -lopie
TARGET 		= odict

TRANSLATIONS = ../../../i18n/de/odict.ts \
	 ../../../i18n/nl/odict.ts \
	 ../../../i18n/xx/odict.ts \
	 ../../../i18n/en/odict.ts \
	 ../../../i18n/es/odict.ts \
	 ../../../i18n/fr/odict.ts \
	 ../../../i18n/hu/odict.ts \
	 ../../../i18n/ja/odict.ts \
	 ../../../i18n/ko/odict.ts \
	 ../../../i18n/no/odict.ts \
	 ../../../i18n/pl/odict.ts \
	 ../../../i18n/pt/odict.ts \
	 ../../../i18n/pt_BR/odict.ts \
	 ../../../i18n/sl/odict.ts \
	 ../../../i18n/zh_CN/odict.ts \
	 ../../../i18n/zh_TW/odict.ts \
   	 ../../../i18n/it/odict.ts \
   	 ../../../i18n/da/odict.ts



include ( $(OPIEDIR)/include.pro )
