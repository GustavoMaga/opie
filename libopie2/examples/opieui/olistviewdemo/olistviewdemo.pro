TEMPLATE        = app
CONFIG          = qt warn_on 
HEADERS         = olistviewdemo.h
SOURCES         = olistviewdemo.cpp \
                  main.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lopieui2 -lopiecore2 -lqpe
TARGET          = olistviewdemo
MOC_DIR     = moc
OBJECTS_DIR = obj




!contains( platform, x11 ) {
  include ( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        += -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib 
}

