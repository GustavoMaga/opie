TEMPLATE        = app
CONFIG          = qt warn_on debug
#CONFIG          = qt warn_on release
HEADERS         = bluebase.h scandialog.h btlistitem.h hciconfwrapper.h 
SOURCES         = main.cpp bluebase.cpp scandialog.cpp btlistitem.cpp hciconfwrapper.cpp
INCLUDEPATH     += $(OPIEDIR)/include
INCLUDEPATH     += $(OPIEDIR)/noncore/net/opietooth/lib
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lopietooth -lopie
INTERFACES      = bluetoothbase.ui  devicedialog.ui  
DESTDIR         = $(OPIEDIR)/bin
TARGET          = bluetooth-manager

TRANSLATIONS = ../../../../i18n/de/bluetooth-manager.ts \
	 ../../../../i18n/en/bluetooth-manager.ts \
	 ../../../../i18n/es/bluetooth-manager.ts \
	 ../../../../i18n/fr/bluetooth-manager.ts \
	 ../../../../i18n/hu/bluetooth-manager.ts \
	 ../../../../i18n/ja/bluetooth-manager.ts \
	 ../../../../i18n/ko/bluetooth-manager.ts \
	 ../../../../i18n/no/bluetooth-manager.ts \
	 ../../../../i18n/pl/bluetooth-manager.ts \
	 ../../../../i18n/pt/bluetooth-manager.ts \
	 ../../../../i18n/pt_BR/bluetooth-manager.ts \
	 ../../../../i18n/sl/bluetooth-manager.ts \
	 ../../../../i18n/zh_CN/bluetooth-manager.ts \
	 ../../../../i18n/zh_TW/bluetooth-manager.ts
