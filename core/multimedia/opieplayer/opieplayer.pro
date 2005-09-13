TEMPLATE  = app
CONFIG    = qt warn_on
#
DESTDIR   = $(OPIEDIR)/bin
HEADERS   = loopcontrol.h mediaplayerplugininterface.h \
	playlistselection.h mediaplayerstate.h \
	videowidget.h audiowidget.h playlistwidget.h \
	mediaplayer.h audiodevice.h inputDialog.h om3u.h \
	rssparser.h
SOURCES   = main.cpp \
      loopcontrol.cpp playlistselection.cpp mediaplayerstate.cpp \
      videowidget.cpp audiowidget.cpp playlistwidget.cpp \
      mediaplayer.cpp audiodevice.cpp inputDialog.cpp om3u.cpp \
      rssparser.cpp
TARGET    = opieplayer
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lpthread -lopiecore2 -lopieui2 -lqtaux2

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

include( $(OPIEDIR)/include.pro )
