CONFIG		= qt warn_on quick-app
SUBDIRS		= db ui
HEADERS		= tableviewer.h \
		  xmlencodeattr.h \
		  ui/commonwidgets.h \
		  ui/tvbrowseview.h \
		  ui/tvlistview.h \
		  ui/tvfilterview.h \
		  ui/tveditview.h \
		  ui/browsekeyentry.h \
		  ui/filterkeyentry.h \
		  ui/tvkeyedit.h \
		  db/datacache.h \
		  db/common.h \
		  db/xmlsource.h \
		  db/csvsource.h
SOURCES		= main.cpp \
		  tableviewer.cpp \
		  xmlencodeattr.cpp \
		  ui/commonwidgets.cpp \
		  ui/tvbrowseview.cpp \
		  ui/tvfilterview.cpp \
		  ui/browsekeyentry.cpp \
		  ui/filterkeyentry.cpp \
		  ui/tvlistview.cpp \
		  ui/tveditview.cpp \
		  ui/tvkeyedit.cpp \
		  db/datacache.cpp \
		  db/xmlsource.cpp \
		  db/csvsource.cpp \
		  db/common.cpp
INTERFACES	= ui/tvkeyedit_gen.ui
TARGET		= tableviewer
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS		+= -lqpe -lopiecore2

include ( $(OPIEDIR)/include.pro )
