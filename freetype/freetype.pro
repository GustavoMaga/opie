TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS		= fontfactoryttf_qws.h freetypefactoryimpl.h
SOURCES		= fontfactoryttf_qws.cpp freetypefactoryimpl.cpp
qt2:HEADERS	+= qfontdata_p.h
TARGET		= freetypefactory
DESTDIR		= $(OPIEDIR)/plugins/fontfactories
INCLUDEPATH	+= $(QTDIR)/src/3rdparty/freetype/include $(QTDIR)/src/3rdparty/freetype/include/freetype/config $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lfreetype
VERSION		= 1.0.0



include ( $(OPIEDIR)/include.pro )
