TEMPLATE  = app
CONFIG    = qt warn_on release
DESTDIR   = $(OPIEDIR)/bin
HEADERS   = clock.h setAlarm.h
SOURCES   = clock.cpp setAlarm.cpp \
      main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lopie -lpthread
INTERFACES  =
TARGET    = clock

TRANSLATIONS = ../../../i18n/de/clock.ts \
   ../../../i18n/xx/clock.ts \
   ../../../i18n/en/clock.ts \
   ../../../i18n/es/clock.ts \
   ../../../i18n/fr/clock.ts \
   ../../../i18n/hu/clock.ts \
   ../../../i18n/ja/clock.ts \
   ../../../i18n/ko/clock.ts \
   ../../../i18n/no/clock.ts \
   ../../../i18n/pl/clock.ts \
   ../../../i18n/pt/clock.ts \
   ../../../i18n/pt_BR/clock.ts \
   ../../../i18n/sl/clock.ts \
   ../../../i18n/zh_CN/clock.ts \
   ../../../i18n/zh_TW/clock.ts \
   ../../../i18n/da/clock.ts



include ( $(OPIEDIR)/include.pro )
