TEMPLATE     = app
#CONFIG       = qt warn_on release
CONFIG 	     = qt debug
DESTDIR      = $(OPIEDIR)/bin
HEADERS      = io_layer.h io_serial.h \
	       file_layer.h sz_transfer.h \
	       metafactory.h \
	       session.h \
	       mainwindow.h \
	       profile.h \
	       profileconfig.h \
	       profilemanager.h  \
	       configwidget.h \
	       tabwidget.h \
	       configdialog.h \
               emulation_layer.h \
               widget.h \
               vt102emulation.h \
               common.h \
               history.h \
               screen.h \
	       keytrans.h \
	       widget_layer.h \
	       transferdialog.h \
	       profiledialogwidget.h \
	       profileeditordialog.h \
	       default.h \
	       terminalwidget.h \
	       iolayerbase.h \
	       serialconfigwidget.h

SOURCES      = io_layer.cpp io_serial.cpp \
	       file_layer.cpp sz_transfer.cpp \
               main.cpp \
	       metafactory.cpp \
	       session.cpp \
	       mainwindow.cpp \
	       profile.cpp \
	       profileconfig.cpp \
	       profilemanager.cpp  \
	       tabwidget.cpp \
	       configdialog.cpp \
	       emulation_layer.cpp \
	       widget.cpp \
	       vt102emulation.cpp \
	       history.cpp \
	       screen.cpp \
	       keytrans.cpp \
	       widget_layer.cpp \
	       transferdialog.cpp \
	       profiledialogwidget.cpp \
	       profileeditordialog.cpp \
	       default.cpp \
	       terminalwidget.cpp \
	       iolayerbase.cpp \
	       serialconfigwidget.cpp
   
INTERFACES   = configurebase.ui editbase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopie
TARGET       = opie-console

