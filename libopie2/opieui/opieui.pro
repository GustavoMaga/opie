TEMPLATE    = lib
CONFIG      += qt warn_on 
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = oclickablelabel.h      \
              odialog.h              \
              ofontselector.h        \
              oimageeffect.h         \
	      okeyconfigwidget.h     \
              olistview.h            \
              opixmapeffect.h        \
              opopupmenu.h           \
              opixmapprovider.h      \
              oselector.h            \
              oseparator.h           \
              otabinfo.h             \
              otabbar.h              \
              otabwidget.h           \
              otaskbarapplet.h       \
              otimepicker.h          \
              oversatileview.h       \
              oversatileviewitem.h   \
              owait.h

SOURCES     = oclickablelabel.cpp    \
              odialog.cpp            \
              ofontselector.cpp      \
              oimageeffect.cpp       \
	      okeyconfigwidget.cpp   \
              olistview.cpp          \
              opixmapeffect.cpp      \
              opopupmenu.cpp         \
              opixmapprovider.cpp    \
              oselector.cpp          \
              oseparator.cpp         \
              otabbar.cpp            \
              otabwidget.cpp         \
              otaskbarapplet.cpp     \
              otimepicker.cpp        \
              oversatileview.cpp     \
              oversatileviewitem.cpp \
              owait.cpp

include( big-screen/big-screen.pro )
include( fileselector/fileselector.pro )

INTERFACES  = otimepickerbase.ui

TARGET      = opieui2
VERSION     = 1.8.5

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

LIBS        += -lopiecore2

!contains( platform, x11 ) {
  include( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        += -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib
}

