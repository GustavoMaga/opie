CONFIG    = qt warn_on  quick-app
HEADERS   = playlistselection.h mediaplayerstate.h xinecontrol.h \
   	    videowidget.h audiowidget.h playlistwidget.h om3u.h mediaplayer.h inputDialog.h \
            frame.h lib.h xinevideowidget.h volumecontrol.h playlistwidgetgui.h\
            alphablend.h yuv2rgb.h threadutil.h mediawidget.h playlistview.h playlistfileview.h \
	    skin.h
SOURCES   = main.cpp \
            playlistselection.cpp mediaplayerstate.cpp xinecontrol.cpp \
            videowidget.cpp audiowidget.cpp playlistwidget.cpp om3u.cpp mediaplayer.cpp inputDialog.cpp \
            frame.cpp lib.cpp nullvideo.c xinevideowidget.cpp volumecontrol.cpp \
	    playlistwidgetgui.cpp\
            alphablend.c yuv2rgb.c yuv2rgb_arm2.c yuv2rgb_arm4l.S \
	    threadutil.cpp mediawidget.cpp playlistview.cpp playlistfileview.cpp \
	    skin.cpp
TARGET    = opieplayer2
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include 
LIBS        += -lqpe -lpthread -lopie -lxine -lstdc++
MOC_DIR   = qpeobj
OBJECTS_DIR = qpeobj

include ( $(OPIEDIR)/include.pro )
