TEMPLATE    = lib
CONFIG      += qt warn_on 
DESTDIR     = $(OPIEDIR)/lib

INTERFACES  =
TARGET      = opiepim2
VERSION     = 1.8.6
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lopiecore2

include ( $(OPIEDIR)/gen.pro )
include ( core/core.pro )
include ( backend/backends.pro )
include ( ui/ui.pro )

!contains( platform, x11 ) {
  include ( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        = -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib 
}
