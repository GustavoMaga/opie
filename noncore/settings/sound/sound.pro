TEMPLATE  = app
CONFIG    += qt warn_on release
DESTDIR   = $(OPIEDIR)/bin
HEADERS   = soundsettings.h soundsettingsbase.h
SOURCES   = soundsettings.cpp soundsettingsbase.cpp main.cpp
#INTERFACES = soundsettingsbase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe
TARGET    = sound



TRANSLATIONS = ../../../i18n/de/sound.ts
TRANSLATIONS += ../../../i18n/en/sound.ts
TRANSLATIONS += ../../../i18n/es/sound.ts
TRANSLATIONS += ../../../i18n/fr/sound.ts
TRANSLATIONS += ../../../i18n/hu/sound.ts
TRANSLATIONS += ../../../i18n/ja/sound.ts
TRANSLATIONS += ../../../i18n/ko/sound.ts
TRANSLATIONS += ../../../i18n/no/sound.ts
TRANSLATIONS += ../../../i18n/pl/sound.ts
TRANSLATIONS += ../../../i18n/pt/sound.ts
TRANSLATIONS += ../../../i18n/pt_BR/sound.ts
TRANSLATIONS += ../../../i18n/sl/sound.ts
TRANSLATIONS += ../../../i18n/zh_CN/sound.ts
TRANSLATIONS += ../../../i18n/zh_TW/sound.ts

