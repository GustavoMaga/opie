CONFIG          += qt warn_on quick-app
HEADERS		= mindbreaker.h
SOURCES		= main.cpp \
              mindbreaker.cpp
TARGET          = mindbreaker
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2


include( $(OPIEDIR)/include.pro )
