TARGET = mailit
CONFIG    = qt warn_on  quick-app
HEADERS   = emailclient.h \
      emailhandler.h \
      emaillistitem.h \
      mailitwindow.h \
      md5.h \
      popclient.h \
      readmail.h \
      smtpclient.h \
      writemail.h \
      textparser.h \
      viewatt.h \
      addatt.h \
      editaccount.h \
      maillist.h \
      addresslist.h
SOURCES   = emailclient.cpp \
      emailhandler.cpp \
      emaillistitem.cpp \
      mailitwindow.cpp \
      main.cpp \
      md5.c \
      popclient.cpp \
      readmail.cpp \
      smtpclient.cpp \
      writemail.cpp \
      textparser.cpp \
      viewatt.cpp \
      addatt.cpp \
      editaccount.cpp \
      maillist.cpp \
      addresslist.cpp

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include 
LIBS            += -lqpe -lopie
# -lssl

include ( $(OPIEDIR)/include.pro )
