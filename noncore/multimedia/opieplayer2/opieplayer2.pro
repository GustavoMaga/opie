TEMPLATE  = app
CONFIG    = qt warn_on release
#release
DESTDIR   = $(OPIEDIR)/bin
HEADERS   = playlistselection.h mediaplayerstate.h xinecontrol.h mediadetect.h\
	      videowidget.h audiowidget.h playlistwidget.h mediaplayer.h inputDialog.h \
	      frame.h lib.h xinevideowidget.h \
              alphablend.h yuv2rgb.h
SOURCES   = main.cpp \
    	  playlistselection.cpp mediaplayerstate.cpp xinecontrol.cpp mediadetect.cpp\
     	 videowidget.cpp audiowidget.cpp playlistwidget.cpp mediaplayer.cpp inputDialog.cpp \
	 frame.cpp lib.cpp nullvideo.c xinevideowidget.cpp \
         alphablend.c yuv2rgb.c yuv2rgb_mlib.c yuv2rgb_mmx.c
TARGET    = opieplayer2
INCLUDEPATH += $(OPIEDIR)/include 
DEPENDPATH  += $(OPIEDIR)/include 
LIBS            += -lqpe -lpthread -lopie -lxine -lxineutils

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

