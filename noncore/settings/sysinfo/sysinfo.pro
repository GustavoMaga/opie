CONFIG    = qt warn_on  quick-app
HEADERS   = \
      memory.h \
      graph.h \
      load.h \
      storage.h \
      processinfo.h \
      modulesinfo.h \
      detail.h \
      contrib/dhry.h \
      benchmarkinfo.h \
      sysloginfo.h \
      versioninfo.h \
      sysinfo.h
SOURCES   = main.cpp \
      memory.cpp \
      graph.cpp \
      load.cpp \
      storage.cpp \
      processinfo.cpp \
      modulesinfo.cpp \
      detail.cpp \
      contrib/dhry.c contrib/fft.c \
      benchmarkinfo.cpp \
      sysloginfo.cpp \
      versioninfo.cpp \
      sysinfo.cpp

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2 -lopieui2
DEFINES     += UNIX
TARGET    = sysinfo
VERSION   = 1.2.0

include( $(OPIEDIR)/include.pro )
