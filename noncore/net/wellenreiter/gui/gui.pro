MOC_DIR         = ./tmp
OBJECTS_DIR     = ./tmp
DESTDIR         = $(OPIEDIR)/bin
TEMPLATE        = app
CONFIG          = qt warn_on debug

HEADERS         = wellenreiterbase.h \
                  mainwindow.h \
                  wellenreiter.h \
                  scanlistitem.h \
                  scanlist.h \
                  logwindow.h \
                  hexwindow.h \
                  configwindow.h \
                  wlan.h \
                  cardconfig.h \
                  manufacturers.h

SOURCES         = main.cpp \
                  mainwindow.cpp \
                  wellenreiterbase.cpp \
                  wellenreiter.cpp \
                  scanlistitem.cpp \
                  scanlist.cpp \
                  logwindow.cpp \
                  hexwindow.cpp \
                  configwindow.cpp \
                  wlan.cpp \
                  cardconfig.cpp \
                  manufacturers.cpp

INCLUDEPATH     += $(OPIEDIR)/include ../
DEPENDPATH      += $(OPIEDIR)/include ../
LIBS            += -L. -lwellenreiter
INTERFACES      = configbase.ui
TARGET          = wellenreiter

!contains( platform, x11 ) {
  message( qws )
  include ( $(OPIEDIR)/include.pro )
  LIBS += -lqpe -lopie
}

contains( platform, x11 ) {
  LIBS += -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib
  SOURCES += resource.cpp
  HEADERS += resource.h
}

