CONFIG       = qt warn_on quick-app

SOURCES      = mainwindow.cpp    \
               navbar.cpp        \
               searchbar.cpp     \
               opentextdlg.cpp   \
               configuredlg.cpp  \
               textwidget.cpp    \
               main.cpp
HEADERS      = mainwindow.h      \
               navbar.h          \
               searchbar.h       \
               opentextdlg.h     \
               configuredlg.h    \
               textwidget.h      \
               swordoptionlist.h

TARGET       = dagger
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lopiecore2 -lopieui2 -lqpe -lsword

include(











































































































 $(OPIEDIR)/include.pro )

!isEmpty( LIBSWORD_INC_DIR ) {
    INCLUDEPATH = $$LIBSWORD_INC_DIR $$INCLUDEPATH
}
!isEmpty( LIBSWORD_LIB_DIR ) {
    LIBS = -L$$LIBSWORD_LIB_DIR $$LIBS
}
