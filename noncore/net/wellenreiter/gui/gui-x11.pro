DESTDIR         = .
TEMPLATE        = app
CONFIG          = qt warn_on debug
#CONFIG         = qt warn_on release
HEADERS         = wellenreiterbase.h wellenreiter.h scanlistitem.h scanlist.h logwindow.h hexwindow.h configwindow.h resource.h wlan.h cardconfig.h manufacturers.h
SOURCES         = main.cpp wellenreiterbase.cpp wellenreiter.cpp scanlistitem.cpp scanlist.cpp logwindow.cpp hexwindow.cpp configwindow.cpp resource.cpp wlan.cpp cardconfig.cpp manufacturers.cpp
INCLUDEPATH     += ../
DEPENDPATH      += ../
LIBS            += -L. -lwellenreiter
INTERFACES      = configbase.ui
TARGET          = wellenreiter



include ( ../../../../include.pro )
