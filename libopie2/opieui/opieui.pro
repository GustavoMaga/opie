TEMPLATE    = lib
CONFIG      += qt warn_on debug
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = oclickablelabel.h      \
              odialog.h              \
              ofontselector.h        \
              oimageeffect.h         \
              olistview.h            \
              opixmapeffect.h        \
              opopupmenu.h           \
              opixmapprovider.h      \
              oselector.h            \
              otabinfo.h             \
              otabbar.h              \
              otabwidget.h           \
              otimepicker.h          \
              oversatileview.h       \
              oversatileviewitem.h   \
              omessagebox.h          \
              oresource.h            \
              otaskbarapplet.h       \
              oseparator.h

SOURCES     = oclickablelabel.cpp    \
              ofontselector.cpp      \
              oimageeffect.cpp       \
              olistview.cpp          \
              opixmapeffect.cpp      \
              opopupmenu.cpp         \
              opixmapprovider.cpp    \
              oselector.cpp          \
              otabbar.cpp            \
              otabwidget.cpp         \
              otimepicker.cpp        \
              oversatileview.cpp     \
              oversatileviewitem.cpp \
              odialog.cpp            \
              oresource.cpp          \
              otaskbarapplet.cpp     \
              oseparator.cpp

include ( big-screen/big-screen.pro )
include ( fileselector/fileselector.pro )

INTERFACES  = otimepickerbase.ui

TARGET      = opieui2
VERSION     = 1.8.5

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

LIBS        += -lopiecore2

!contains( platform, x11 ) {
  include ( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        += -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib
}

