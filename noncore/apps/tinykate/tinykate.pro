TEMPLATE  = app
CONFIG    = qt warn_on release
DESTDIR   = $(OPIEDIR)/bin
HEADERS   = tinykate.h cgotoline.h 
SOURCES   = tinykate.cpp main.cpp cgotoline.cpp
INTERFACES  = cgotobase.ui
INCLUDEPATH += $(OPIEDIR)/include \
      $(OPIEDIR)/noncore/apps/tinykate/libkate \
      $(OPIEDIR)/noncore/apps/tinykate/libkate/microkde \
                        $(OPIEDIR)/noncore/apps/tinykate/libkate/document \
                        $(OPIEDIR)/noncore/apps/tinykate/libkate/view \
                        $(OPIEDIR)/noncore/apps/tinykate/libkate/interfaces \
                        $(OPIEDIR)/noncore/apps/tinykate/libkate/ktexteditor \
                        $(OPIEDIR)/noncore/apps/tinykate/libkate/qt3back

DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -ltinykate -lopie
TARGET    = tinykate



include ( $(OPIEDIR)/include.pro )
