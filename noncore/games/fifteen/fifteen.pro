CONFIG		= qt warn_on  
HEADERS		= fifteen.h \
                  fifteenconfigdialog.h
SOURCES		= fifteen.cpp \ 
                  fifteenconfigdialog.cpp \
		main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2 -lopieui2
TARGET		= fifteen
INTERFACES      = fifteenconfigdialogbase.ui

include ( $(OPIEDIR)/include.pro )
 
