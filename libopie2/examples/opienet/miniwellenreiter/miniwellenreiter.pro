TEMPLATE        = app
CONFIG          = qt warn_on
HEADERS         =
SOURCES         = miniwellenreiter.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lopiecore2 -lopienet2
TARGET          = miniwellenreiter
MOC_DIR         = moc
OBJECTS_DIR     = obj

include( $(OPIEDIR)/include.pro )

!isEmpty( LIBPCAP_INC_DIR ) {
    INCLUDEPATH = $$LIBPCAP_INC_DIR $$INCLUDEPATH
}
!isEmpty( LIBPCAP_LIB_DIR ) {
    LIBS = -L$$LIBPCAP_LIB_DIR $$LIBS
}
