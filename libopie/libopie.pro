TEMPLATE        = lib
CONFIG          += qte warn_on release
HEADERS 	= ofontmenu.h ofileselector.h ofiledialog.h ofileview.h tododb.h todoevent.h todoresource.h todovcalresource.h xmltree.h  colordialog.h colorpopupmenu.h oclickablelabel.h oprocctrl.h oprocess.h ohwinfo.h
SOURCES		= ofontmenu.cc ofileselector.cc ofiledialog.cc xmltree.cc tododb.cpp todoevent.cpp todovcalresource.cpp  colordialog.cpp colorpopupmenu.cpp oclickablelabel.cpp oprocctrl.cpp oprocess.cpp ohwinfo.cpp
TARGET		= opie
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR         = $(QTDIR)/lib$(PROJMAK)
#VERSION = 1.0.0
