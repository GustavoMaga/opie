CONFIG    += qt warn_on release quick-app
HEADERS   = tictac.h
SOURCES   = main.cpp \
      tictac.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2

TARGET		= tictac

include ( $(OPIEDIR)/include.pro )
