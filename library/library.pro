TEMPLATE	= lib
#CONFIG		+= qt warn_on release
CONFIG		+= qt warn_on debug
HEADERS	=   calendar.h \
	    global.h \
	    resource.h \
	    xmlreader.h \
	    mimetype.h \
	    menubutton.h \
	    network.h \
	    networkinterface.h \
	    filemanager.h \
	    fontmanager.h \
	    qdawg.h \
	    datebookmonth.h \
	    fileselector.h \
	    fileselector_p.h \
	    imageedit.h \
	    qcopenvelope_qws.h \
	    qpedecoration_qws.h \
	    qpeapplication.h \
	    qpestyle.h \
	    qpedialog.h \
	    lightstyle.h \
	    config.h \
	    applnk.h \
	    sound.h \
	    tzselect.h \
	    qmath.h \
	    datebookdb.h \
	    alarmserver.h \
	    process.h \
	    password.h \
	    timestring.h \
	    fontfactoryinterface.h \
	    fontdatabase.h \
	    power.h \
	    storage.h \
	    qpemessagebox.h \
	    timeconversion.h \
	    qpedebug.h \
	    qpemenubar.h \
	    qpetoolbar.h \
	    backend/categories.h \
	    stringutil.h \
	    backend/palmtoprecord.h \
            backend/task.h \
	    backend/event.h \
	    backend/contact.h\
	    categorymenu.h \
	    categoryedit_p.h \
	    categoryselect.h \
	    categorywidget.h \
	ir.h \
	backend/vobject_p.h \
	findwidget_p.h \
	finddialog.h \
	lnkproperties.h \
	windowdecorationinterface.h \
	textcodecinterface.h \
	imagecodecinterface.h \
	qt_override_p.h
	
SOURCES	=   calendar.cpp \
	    global.cpp \
	    xmlreader.cpp \
	    mimetype.cpp \
	    menubutton.cpp \
	    network.cpp \
	    networkinterface.cpp \
	    filemanager.cpp \
	    fontmanager.cpp \
	    qdawg.cpp \
	    datebookmonth.cpp \
	    fileselector.cpp \
	    imageedit.cpp \
	    resource.cpp \
	    qpedecoration_qws.cpp \
	    qcopenvelope_qws.cpp \
	    qpeapplication.cpp \
	    qpestyle.cpp \
	    qpedialog.cpp \
	    lightstyle.cpp \
	    config.cpp \
	    applnk.cpp \
	    sound.cpp \
	    tzselect.cpp \
	    qmath.c \
	    datebookdb.cpp \
	    alarmserver.cpp \
	    password.cpp \
	    process.cpp \
	    process_unix.cpp \
	    timestring.cpp \
	    fontdatabase.cpp \
	    power.cpp \
	    storage.cpp \
	    qpemessagebox.cpp \
            backend/timeconversion.cpp \
	    qpedebug.cpp \
	    qpemenubar.cpp \
	    qpetoolbar.cpp \
	    backend/categories.cpp \
	    backend/stringutil.cpp \
	    backend/palmtoprecord.cpp \
            backend/task.cpp \
	    backend/event.cpp \
	    backend/contact.cpp \
	    categorymenu.cpp \
	categoryedit_p.cpp \
	categoryselect.cpp \
	categorywidget.cpp \
	ir.cpp \
	backend/vcc_yacc.cpp \
	backend/vobject.cpp \
	findwidget_p.cpp \
	finddialog.cpp \
	lnkproperties.cpp \
	qt_override.cpp



# Qt 3 compatibility
HEADERS += quuid.h qcom.h qlibrary.h qlibrary_p.h
SOURCES += quuid.cpp qlibrary.cpp qlibrary_unix.cpp
INCLUDEPATH += $(OPIEDIR)/include backend
LIBS		+= -ldl -lcrypt -lm
INTERFACES = passwordbase_p.ui categoryeditbase_p.ui findwidgetbase_p.ui lnkpropertiesbase_p.ui
TARGET		= qpe
DESTDIR		= $(OPIEDIR)/lib$(PROJMAK)
VERSION		= 1.5.0.1

include ( $(OPIEDIR)/include.pro )

contains( CONFIG, no-override ){
    DEFINES += OPIE_NO_OVERRIDE_QT
}

contains( CONFIG, LIBQPE_WITHROHFEEDBACK ){
    DEFINES += OPIE_ROHFEEDBACK
    SOURCES += backend/rohfeedback.cpp
    HEADERS += backend/rohfeedback.h
}
