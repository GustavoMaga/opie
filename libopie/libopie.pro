TEMPLATE        = lib
CONFIG          += qte warn_on release
HEADERS   = ofontmenu.h \
    ocolorbutton.h \
    ofiledialog.h ofileselector.h \
    ocheckitem.h  \
    xmltree.h \
    colordialog.h colorpopupmenu.h \
    oclickablelabel.h oprocctrl.h \
    oprocess.h odevice.h odevicebutton.h \
    otimepicker.h otabwidget.h \
    otabbar.h otabinfo.h \
    ofontselector.h \
    pim/opimrecord.h \
    pim/otodo.h \
    pim/orecordlist.h \
    pim/opimaccesstemplate.h \
    pim/opimaccessbackend.h \
    pim/otodoaccess.h \
    pim/otodoaccessbackend.h \
    pim/ocontact.h \
    pim/ocontactaccess.h \
    pim/ocontactaccessbackend.h \
    pim/ocontactaccessbackend_xml.h \
    pim/ocontactaccessbackend_vcard.h \
    pim/obackendfactory.h \
    pim/opimcache.h \ 
    pim/otodoaccessvcal.h \
    pim/orecur.h \
    pim/opimstate.h \
    pim/opimxrefpartner.h \
    pim/opimxref.h \
    pim/opimxrefmanager.h \
    pim/opimmaintainer.h \
    pim/opimnotify.h \
    pim/opimnotifymanager.h \
    pim/opimmainwindow.h \
    pim/opimresolver.h \
    pim/oevent.h \
    pim/otimezone.h \
    pim/odatebookaccess.h \
    pim/odatebookaccessbackend.h \
    pim/odatebookaccessbackend_xml.h \
    orecurrancewidget.h \
    oticker.h owait.h
#    pim/otodoaccesssql.h \

SOURCES   = ofontmenu.cc  \
        ocolorbutton.cpp \
        sharp_compat.cpp \
      xmltree.cc \
      ofiledialog.cc ofileselector.cc \
            ocheckitem.cpp  \
      colordialog.cpp \
      colorpopupmenu.cpp oclickablelabel.cpp \
      oprocctrl.cpp oprocess.cpp \
      odevice.cpp odevicebutton.cpp otimepicker.cpp \
       otabwidget.cpp otabbar.cpp \
             ofontselector.cpp \
             pim/otodo.cpp \
             pim/opimrecord.cpp \
       pim/otodoaccess.cpp \
       pim/otodoaccessbackend.cpp \
             pim/otodoaccessxml.cpp \
             pim/ocontact.cpp \
             pim/ocontactaccess.cpp \
             pim/ocontactaccessbackend_vcard.cpp \
             pim/ocontactaccessbackend_xml.cpp \
       pim/otodoaccessvcal.cpp \
       pim/orecur.cpp \
       pim/opimstate.cpp \
       pim/opimxrefpartner.cpp \
       pim/opimxref.cpp \
       pim/opimxrefmanager.cpp \
       pim/opimmaintainer.cpp \
       pim/opimnotify.cpp \
       pim/opimnotifymanager.cpp \
       pim/opimmainwindow.cpp \
       pim/opimresolver.cpp \
       pim/oevent.cpp \
       pim/otimezone.cpp \
       pim/odatebookaccess.cpp \
       pim/odatebookaccessbackend.cpp \
       pim/odatebookaccessbackend_xml.cpp \
       orecurrancewidget.cpp \
       oticker.cpp owait.cpp \
#             pim/otodoaccesssql.cpp \

TARGET    = opie
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(OPIEDIR)/lib$(PROJMAK)
#VERSION = 1.0.0

LIBS += -lqpe

# LIBS            += -lopiesql

INTERFACES      = otimepickerbase.ui orecurrancebase.ui

TRANSLATIONS = ../i18n/de/libopie.ts \
	 ../i18n/xx/libopie.ts \
   ../i18n/en/libopie.ts \
   ../i18n/es/libopie.ts \
   ../i18n/fr/libopie.ts \
   ../i18n/hu/libopie.ts \
   ../i18n/ja/libopie.ts \
   ../i18n/ko/libopie.ts \
   ../i18n/no/libopie.ts \
   ../i18n/pl/libopie.ts \
   ../i18n/pt/libopie.ts \
   ../i18n/pt_BR/libopie.ts \
   ../i18n/sl/libopie.ts \
   ../i18n/zh_CN/libopie.ts \
   ../i18n/zh_TW/libopie.ts \
   ../i18n/da/libopie.ts



include ( $(OPIEDIR)/include.pro )
