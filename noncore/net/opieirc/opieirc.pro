CONFIG	    = qt warn_on quick-app
HEADERS     = ircchannel.h ircconnection.h \
              ircmessage.h \
              ircmessageparser.h ircoutput.h \
              ircperson.h ircserver.h ircsession.h \
              mainwindow.h irctab.h ircservertab.h \
              ircchanneltab.h ircchannellist.h \
              ircserverlist.h ircservereditor.h \
              ircquerytab.h ircsettings.h ircmisc.h
SOURCES	    = ircchannel.cpp ircconnection.cpp \
              ircmessage.cpp \
              ircmessageparser.cpp ircoutput.cpp \
              ircperson.cpp ircserver.cpp \
              ircsession.cpp main.cpp mainwindow.cpp \
              irctab.cpp ircservertab.cpp \
              ircchanneltab.cpp ircchannellist.cpp \
              ircserverlist.cpp ircservereditor.cpp \
              ircquerytab.cpp ircsettings.cpp ircmisc.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

PRECOMPILED_HEADER = stable.h

LIBS        += -lqpe -lopiecore2 -lopieui2 -lqtaux2
TARGET      = opieirc


include(





























































































 $(OPIEDIR)/include.pro )
