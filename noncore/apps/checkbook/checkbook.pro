CONFIG    = qt warn_on release quick-app
HEADERS   = mainwindow.h \
      cbinfo.h        \
      traninfo.h      \
      graphinfo.h     \
      configuration.h \
      password.h      \
      checkbook.h     \
      transaction.h   \
      tabledef.h      \
      listedit.h      \
      cfg.h           \
      graph.h
SOURCES   = main.cpp    \
      mainwindow.cpp    \
      cbinfo.cpp        \
      traninfo.cpp      \
      graphinfo.cpp     \
      configuration.cpp \
      password.cpp      \
      checkbook.cpp     \
      transaction.cpp   \
      tabledef.cpp      \
      listedit.cpp      \
      cfg.cpp           \
      graph.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS    += -lqpe -lopie
TARGET    = checkbook

TRANSLATIONS = ../../../i18n/de/checkbook.ts \
	 ../../../i18n/nl/checkbook.ts \
	 ../../../i18n/da/checkbook.ts \
	 ../../../i18n/xx/checkbook.ts \
   ../../../i18n/en/checkbook.ts    \
   ../../../i18n/es/checkbook.ts    \
   ../../../i18n/fr/checkbook.ts    \
   ../../../i18n/hu/checkbook.ts    \
   ../../../i18n/ja/checkbook.ts    \
   ../../../i18n/ko/checkbook.ts    \
   ../../../i18n/no/checkbook.ts    \
   ../../../i18n/pl/checkbook.ts    \
   ../../../i18n/pt/checkbook.ts    \
   ../../../i18n/pt_BR/checkbook.ts \
   ../../../i18n/sl/checkbook.ts    \
   ../../../i18n/zh_CN/checkbook.ts \
   ../../../i18n/zh_TW/checkbook.ts \
   ../../../i18n/it/checkbook.ts



include ( $(OPIEDIR)/include.pro )
