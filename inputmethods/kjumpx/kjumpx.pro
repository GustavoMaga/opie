TEMPLATE        = lib
CONFIG         += qt plugin warn_on release
HEADERS         = keyboard.h \
	          keyboardimpl.h
SOURCES         = keyboard.cpp \
                  keyboardimpl.cpp
TARGET          = qkjumpx
DESTDIR         = ../../plugins/inputmethods
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS           += -lqpe
VERSION         = 1.0.0

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/inputmethods
