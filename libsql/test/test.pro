TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= 
SOURCES		= main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopiesql
TARGET		= sqltest




include ( $(OPIEDIR)/include.pro )
