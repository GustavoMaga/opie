TEMPLATE = app
CONFIG       += qt warn_on 
HEADERS      = io_layer.h io_serial.h io_irda.h io_bt.h io_modem.h \
           file_layer.h filetransfer.h \
           metafactory.h \
           session.h \
           mainwindow.h \
           profile.h \
           profileconfig.h \
           profilemanager.h  \
           tabwidget.h \
           configdialog.h \
           keytrans.h \
           transferdialog.h \
           profiledialogwidget.h \
           profileeditordialog.h \
           default.h \
           iolayerbase.h \
           serialconfigwidget.h irdaconfigwidget.h \
           btconfigwidget.h modemconfigwidget.h \
           atconfigdialog.h dialdialog.h \
               procctl.h \
               function_keyboard.h  \
           receive_layer.h filereceive.h \
           script.h \
           dialer.h logger.h \
           terminalwidget.h \
           emulation_handler.h TECommon.h \
           TEHistory.h TEScreen.h TEWidget.h \
           TEmuVt102.h TEmulation.h MyPty.h \
           consoleconfigwidget.h fixit.h \
	   comboboxhelper.h

SOURCES      = io_layer.cpp io_serial.cpp io_irda.cpp io_bt.cpp io_modem.cpp \
           file_layer.cpp filetransfer.cpp \
               main.cpp \
           metafactory.cpp \
           session.cpp \
           mainwindow.cpp \
           profile.cpp \
           profileconfig.cpp \
           profilemanager.cpp  \
           tabwidget.cpp \
           configdialog.cpp \
           keytrans.cpp \
           transferdialog.cpp \
           profiledialogwidget.cpp \
           profileeditordialog.cpp \
           iolayerbase.cpp \
           serialconfigwidget.cpp irdaconfigwidget.cpp \
           btconfigwidget.cpp modemconfigwidget.cpp \
           atconfigdialog.cpp dialdialog.cpp \
           default.cpp  procctl.cpp \
               function_keyboard.cpp  \
           receive_layer.cpp filereceive.cpp \
           script.cpp \
           dialer.cpp logger.cpp \
           terminalwidget.cpp \
        emulation_handler.cpp TEHistory.cpp \
        TEScreen.cpp TEWidget.cpp \
        TEmuVt102.cpp TEmulation.cpp MyPty.cpp \
        consoleconfigwidget.cpp fixit.cpp \
	comboboxhelper.cpp


DESTDIR      = $(OPIEDIR)/bin/
INTERFACES   = configurebase.ui editbase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2 -lopieui2 -lutil
TARGET       = opie-console

DEFINES += HAVE_OPENPTY
include(



















































































































 $(OPIEDIR)/include.pro )

teneast {
    DEFINES -= HAVE_OPENPTY
    DEFINES += EAST FSCKED_DISTRI
}