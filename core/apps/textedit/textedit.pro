TEMPLATE  = app
CONFIG    += qt warn_on release

DESTDIR   = $(OPIEDIR)/bin
HEADERS = textedit.h fileBrowser.h fontDialog.h fileSaver.h filePermissions.h inputDialog.h
SOURCES = main.cpp textedit.cpp fileBrowser.cpp fontDialog.cpp fileSaver.cpp filePermissions.cpp inputDialog.cpp 

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe

TARGET    = textedit

TRANSLATIONS = ../i18n/de/textedit.ts
