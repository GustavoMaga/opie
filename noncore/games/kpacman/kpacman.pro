TEMPLATE	= app
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
#TMAKE_CXXFLAGS	+= 
HEADERS		= kpacmanwidget.h \
		    referee.h \
		    status.h \
		    painter.h \
		    score.h \
		    pacman.h \
		    monster.h \
		    keys.h \
		    fruit.h \
		    energizer.h \
		    board.h \
		    bitfont.h \
		    kpacman.h \
		    bitmaps.h \
		    colors.h \
		    config.h \
		    portable.h
SOURCES		= kpacmanwidget.cpp \
		    referee.cpp \
		    status.cpp \
		    painter.cpp \
		    score.cpp \
		    pacman.cpp \
		    monster.cpp \
		    keys.cpp \
		    fruit.cpp \
		    energizer.cpp \
		    board.cpp \
		    bitfont.cpp \
		    kpacman.cpp \
		    config.cpp \
		    main.cpp 

INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
DESTDIR         = $(OPIEDIR)/bin
TARGET		= kpacman


TRANSLATIONS = ../../../i18n/de/kpacman.ts
TRANSLATIONS += ../../../i18n/en/kpacman.ts
TRANSLATIONS += ../../../i18n/es/kpacman.ts
TRANSLATIONS += ../../../i18n/fr/kpacman.ts
TRANSLATIONS += ../../../i18n/hu/kpacman.ts
TRANSLATIONS += ../../../i18n/ja/kpacman.ts
TRANSLATIONS += ../../../i18n/ko/kpacman.ts
TRANSLATIONS += ../../../i18n/no/kpacman.ts
TRANSLATIONS += ../../../i18n/pl/kpacman.ts
TRANSLATIONS += ../../../i18n/pt/kpacman.ts
TRANSLATIONS += ../../../i18n/pt_BR/kpacman.ts
TRANSLATIONS += ../../../i18n/sl/kpacman.ts
TRANSLATIONS += ../../../i18n/zh_CN/kpacman.ts
TRANSLATIONS += ../../../i18n/zh_TW/kpacman.ts

