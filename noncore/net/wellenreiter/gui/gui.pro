DESTDIR         = $(OPIEDIR)/bin
TEMPLATE        = app
CONFIG          = qt warn_on

HEADERS         = wellenreiterbase.h \
                  mainwindow.h \
                  wellenreiter.h \
                  scanlist.h \
                  logwindow.h \
                  hexwindow.h \
                  statwindow.h \
                  configwindow.h \
                  graphwindow.h \
                  packetview.h \
                  protolistview.h \
                  gps.h

SOURCES         = main.cpp \
                  mainwindow.cpp \
                  wellenreiterbase.cpp \
                  wellenreiter.cpp \
                  scanlist.cpp \
                  logwindow.cpp \
                  hexwindow.cpp \
                  statwindow.cpp \
                  configwindow.cpp \
                  graphwindow.cpp \
                  packetview.cpp \
                  protolistview.cpp \
                  gps.cpp

INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
INTERFACES      = configbase.ui
TARGET          = wellenreiter
VERSION         = 1.0.6

DEFINES         += WELLENREITER_VERSION='"$$VERSION (GPL) Opie"'

!contains( platform, x11 ) {
  message( qws )
  include( $(OPIEDIR)/include.pro )
  LIBS += -lqpe -lopiecore2 -lopieui2 -lopienet2
}

contains( platform, x11 ) {
  LIBS += -L$(OPIEDIR)/output/lib -Wl,-rpath,$(OPIEDIR)/output/lib -Wl,-rpath,/usr/local/lib -lwellenreiter
  SOURCES += resource.cpp
  HEADERS += resource.h
  DESTDIR = $(OPIEDIR)/output/bin
}

!isEmpty( LIBPCAP_INC_DIR ) {
    INCLUDEPATH = $$LIBPCAP_INC_DIR $$INCLUDEPATH
}
!isEmpty( LIBPCAP_LIB_DIR ) {
    LIBS = -L$$LIBPCAP_LIB_DIR $$LIBS
}
