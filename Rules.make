.phony: force
force:

$(configs) :
	$(call makecfg,$@)

$(TOPDIR)/gen.pro : $(TOPDIR)/.config
	echo > $@
# added for threaded version
ifneq ($(CONFIG_THREADED),)
	echo CONFIG += thread >> $@
else
	echo CONFIG -= thread >> $@
endif
ifneq ($(CONFIG_DEBUG),)
	echo CONFIG += debug >> $@
	echo CONFIG -= release >> $@
	echo DEFINES += "QT_COMPONENT_DEBUG=2" >> $@
	echo DEFINES += QT_DEBUG >> $@
else
	echo CONFIG -= debug >> $@
	echo CONFIG += release >> $@
	echo DEFINES += "OPIE_NO_DEBUG" >> $@
endif
ifeq ($(CONFIG_STATIC),y)
	echo !contains\( TARGET,qpe \) \{ >> $@
	echo CONFIG += staticlib >> $@
	echo \} >> $@
endif
ifeq ($(filter 3.%,$(QTE_VERSION)),) # not qt3
	echo CONFIG -= qt3 >> $@
else
	echo CONFIG += qt3 >> $@
endif
ifneq ($(CONFIG_QUICK_LAUNCH),)
	echo contains\( CONFIG, quick-app \) \{ >> $@
	echo CONFIG -= staticlib >> $@
	echo CONFIG -= quick-app >> $@
	echo CONFIG += quick-app-lib >> $@
	echo QUICKLAUNCH_PATH = $(CONFIG_QUICKLAUNCH_PATH) >> $@
	echo \} >> $@
else
	echo contains\( CONFIG, quick-app \) \{ >> $@
	echo CONFIG -= quick-app >> $@
	echo CONFIG += quick-app-bin >> $@
	echo \} >> $@
endif
ifeq ($(CONFIG_SQL_PIM_BACKEND),y)
	echo ENABLE_SQL_PIM_BACKEND=$(CONFIG_SQL_PIM_BACKEND) >> $@
else
	echo ENABLE_SQL_PIM_BACKEND=n >> $@
endif
ifeq ($(CONFIG_OPIE_NO_OVERRIDE_QT),y)
	echo CONFIG += no-override >> $@
endif
ifeq ($(CONFIG_OPIE_NO_ERASERECT_FIX),y)
	echo DEFINES += OPIE_NO_ERASE_RECT_HACKFIX >> $@
endif
ifeq ($(CONFIG_OPIE_NO_BUILTIN_SHUTDOWN),y)
	echo DEFINES += OPIE_NO_BUILTIN_SHUTDOWN >> $@
endif
ifeq ($(CONFIG_OPIE_NO_BUILTIN_CALIBRATE),y)
	echo DEFINES += OPIE_NO_BUILTIN_CALIBRATE >> $@
endif
ifeq ($(CONFIG_USE_REALTIME_AUDIO_THREAD),y)
	echo DEFINES += USE_REALTIME_AUDIO_THREAD >> $@
endif
ifeq ($(CONFIG_USE_FILE_NOTIFICATION),y)
	echo DEFINES += USE_FILE_NOTIFICATION >> $@
endif
ifeq ($(CONFIG_QT_QWS_ALLOW_CLOCK),y)
	echo DEFINES += QT_QWS_ALLOW_OVERCLOCK >> $@
endif
ifeq ($(CONFIG_OPIE_NEW_ALLOC),y)
	echo DEFINES += OPIE_NEW_MALLOC >> $@
endif
ifeq ($(CONFIG_OPIE_NO_SOUND_PCM_READ_BITS),y)
	echo DEFINES += OPIE_NO_SOUND_PCM_READ_BITS >> $@
endif
	echo DEFINES += OPIE_SOUND_FRAGMENT_SHIFT=$(CONFIG_OPIE_SOUND_FRAGMENT_SHIFT) >> $@
ifeq ($(CONFIG_OPIE_WE_VERSION_OVERRIDE),y)
	echo DEFINES += OPIE_WE_VERSION=$(CONFIG_OPIE_WE_VERSION) >> $@
endif
ifeq ($(CONFIG_OPIE_TASKBAR_LOCK_KEY_STATE),y)
	echo DEFINES += OPIE_TASKBAR_LOCK_KEY_STATE >> $@
endif	
# Write LIB dirs and INC dirs...
ifeq ($(CONFIG_LIBETPAN_DEP),y)
	echo LIBETPAN_LIB_DIR = $(CONFIG_LIBETPAN_LIB_DIR) >> $@
	echo LIBETPAN_INC_DIR = $(CONFIG_LIBETPAN_INC_DIR) >> $@
endif
ifeq ($(CONFIG_LIBPCAP_DEP),y)
	echo LIBPCAP_LIB_DIR = $(CONFIG_LIBPCAP_LIB_DIR) >> $@
	echo LIBPCAP_INC_DIR = $(CONFIG_LIBPCAP_INC_DIR) >> $@
endif
ifeq ($(CONFIG_LIBSQLITE_DEP),y)
	echo LIBSQLITE_LIB_DIR = $(CONFIG_LIBSQLITE_LIB_DIR) >> $@
	echo LIBSQLITE_INC_DIR = $(CONFIG_LIBSQLITE_INC_DIR) >> $@
endif
ifeq ($(CONFIG_LIBXINE_DEP),y)
	echo LIBXINE_LIB_DIR = $(CONFIG_LIBXINE_LIB_DIR) >> $@
	echo LIBXINE_INC_DIR = $(CONFIG_LIBXINE_INC_DIR) >> $@
endif
ifeq ($(CONFIG_LIBIPK_DEP),y)
	echo LIBIPK_LIB_DIR = $(CONFIG_LIBIPK_LIB_DIR) >> $@
	echo LIBIPK_INC_DIR = $(CONFIG_LIBIPK_INC_DIR) >> $@
endif
ifeq ($(CONFIG_LIBSDK_DEP),y)
	echo LIBSDL_LIB_DIR = $(CONFIG_LIBSDL_LIB_DIR) >> $@
	echo LIBSDL_INC_DIR = $(CONFIG_LIBSDL_INC_DIR) >> $@
endif
ifeq ($(CONFIG_LIBSWORD_DEP),y)
	echo LIBSWORD_LIB_DIR = $(CONFIG_LIBSWORD_LIB_DIR) >> $@
	echo LIBSWORD_INC_DIR = $(CONFIG_LIBSWORD_INC_DIR) >> $@
endif
$(TOPDIR)/.depends : $(shell if [ -e $(TOPDIR)/config.in ]\; then echo $(TOPDIR)/config.in\; fi\;) $(TOPDIR)/.config $(TOPDIR)/packages
	@echo Generating dependency information...
# add to subdir-y, and add descend rules
	@cat $(TOPDIR)/packages | grep -v '^#' | \
		awk '{print \
			".PHONY : " $$2 "\n" \
			"subdir-$$(" $$1 ") += " $$2 "\n\n"; \
			print $$2 " : " $$2 "/Makefile\n\t$$(call descend,$$@,$(filter-out $$@,$$(filter-out $$@,$$(MAKECMDGOALS))))\n"; }' > $(TOPDIR)/.depends
	cat $(TOPDIR)/packages | grep -v '^#' | \
		perl -ne '($$cfg, $$dir, $$pro) = $$_ =~ /^(\S+)\s+(\S+)\s+(\S+)/; if ( -e "$$dir/$$pro" ) { print "$$dir/Makefile : $$dir/$$pro \$$(QMAKE) \$$(OPIEDIR)/gen.pro \$$(OPIEDIR)/.config\n\t\$$(call makefilegen,\$$@)\n\n"; }' \
			>> $(TOPDIR)/.depends
# interpackage dependency generation
	@cat $(TOPDIR)/packages | \
		$(TOPDIR)/scripts/deps.pl >> $(TOPDIR)/.depends

$(TOPDIR)/.depends.cfgs:
# config.in interdependencies
	@echo $(configs) | sed -e 's,/config.in,,g' | ( for i in `cat`; do echo $$i; done ) > dirs
	@cat dirs | ( for i in `cat`; do if [ "`cat dirs|grep $$i 2>/dev/null|wc -l`" -ne "1" ]; then deps=`cat dirs|grep $$i| grep -v "^$$i$$"|for i in \`cat|sed -e's,^$(TOPDIR)/,$$(TOPDIR)/,g'\`; do echo $$i/config.in; done`; echo `echo $$i/config.in|sed -e 's,^$(TOPDIR)/,$$(TOPDIR)/,'` : $$deps; fi; done ) >> $@
	@-rm -f dirs

$(QTDIR)/stamp-headers :
	@-rm -f $(QTDIR)/stamp-headers*
	( cd $(QTDIR)/include; \
		$(patsubst %,ln -sf ../src/kernel/%;,qgfx_qws.h qwsmouse_qws.h \
		qcopchannel_qws.h qwindowsystem_qws.h \
		qfontmanager_qws.h qwsdefaultdecoration_qws.h))
	touch $@

$(QTDIR)/stamp-headers-x11 :
	@-rm -f $(QTDIR)/stamp-headers*
	cd $(QTDIR)/include; $(patsubst %,ln -sf $(OPIEDIR)/x11/libqpe-x11/qt/%;,qgfx_qws.h qwsmouse_qws.h qcopchannel_qws.h qwindowsystem_qws.h qfontmanager_qws.h qwsdefaultdecoration_qws.h)
	touch $@

$(OPIEDIR)/stamp-headers :
	@-rm -f $(OPIEDIR)/stamp-headers*
	mkdir -p $(TOPDIR)/include/qpe \
		$(TOPDIR)/include/qtopia \
		$(TOPDIR)/include/opie \
		$(TOPDIR)/include/opie2 \
		$(TOPDIR)/include/qtopia/private \
		$(TOPDIR)/include/sl
	( cd include/qpe &&  rm -f *.h; ln -sf ../../library/*.h .; ln -sf ../../library/backend/*.h .; rm -f *_p.h; )
	( cd include/qtopia && rm -f *.h; ln -sf ../../library/*.h .; )
	( cd include/qtopia/private && rm -f *.h; ln -sf ../../../library/backend/*.h .; )
ifeq ($(CONFIG_LIBOPIE),y)
	# libopie1
	( cd include/opie &&  rm -f *.h; ln -sf ../../libopie/*.h .; rm -f *_p.h; )
	( cd include/opie &&  ln -sf ../../libopie/pim/*.h .; )
	( cd include/opie &&  ln -sf ../../libopie/big-screen/*.h .; )
endif
	# libopie2
	( cd include/opie2 && ln -sf ../../libopie2/opiecore/*.h .; )
	( cd include/opie2 && ln -sf ../../libopie2/opiecore/device/*.h .; )
	( cd include/opie2 && ln -sf ../../libopie2/opiemm/*.h .; )
	( cd include/opie2 && ln -sf ../../libopie2/opiedb/*.h .; )
	( cd include/opie2 && ln -sf ../../libopie2/opienet/*.h .; )
	( cd include/opie2 && ln -sf ../../libopie2/opiepim/*.h .; )
	( cd include/opie2 && ln -sf ../../libopie2/opiepim/core/*.h .; )
	( cd include/opie2 && ln -sf ../../libopie2/opiepim/core/backends/*.h .; )
	( cd include/opie2 && ln -sf ../../libopie2/opiepim/ui/*.h .; )
	( cd include/opie2 && ln -sf ../../libopie2/opieui/*.h .; )
	( cd include/opie2 && ln -sf ../../libopie2/opieui/fileselector/*.h .; )
	( cd include/opie2 && ln -sf ../../libopie2/opieui/big-screen/*.h .; )
	( cd include/opie2 && ln -sf ../../libopie2/opiesecurity/*.h .; )
	# auxilliary libraries
	( cd include/opie2 && ln -sf ../../libqtaux/*.h .; )
	( cd include/sl && ln -sf ../../libslcompat/*.h .; )
	( cd include/ && ln -sf ../noncore/net/ftplib/*.h .; )
	( cd include/ && ln -sf ../inputmethods/pickboard/pickboardcfg.h .; )
	( cd include/ && ln -sf ../inputmethods/pickboard/pickboardpicks.h .; )
	# all
ifeq ($(CONFIG_LIBOPIE),y)
	( cd include/opie; for generatedHeader in `cd ../../libopie; ls *.ui | sed -e "s,\.ui,\.h,g"`; do \
	ln -sf ../../libopie/$$generatedHeader $$generatedHeader; done )
endif
	( cd include/opie2; for generatedHeader in `cd ../../libopie2/opieui; ls *.ui | sed -e "s,\.ui,\.h,g"`; do \
	ln -sf ../../libopie2/opieui/$$generatedHeader $$generatedHeader; done )
	( cd include/opie2; for generatedHeader in `cd ../../libopie2/opiepim/ui; ls *.ui | sed -e "s,\.ui,\.h,g"`; do \
	ln -sf ../../libopie2/opiepim/ui/$$generatedHeader $$generatedHeader; done )
	ln -sf ../../library/custom.h $(TOPDIR)/include/qpe/custom.h
	touch $@

$(OPIEDIR)/stamp-headers-x11 :
	@-rm -f $(OPIEDIR)/stamp-headers*
	mkdir -p $(TOPDIR)/include/qpe $(TOPDIR)/include/qtopia \
		$(TOPDIR)/include/opie $(TOPDIR)/include/qtopia/private
	( cd include/qpe &&  rm -f *.h; ln -sf ../../library/*.h .; ln -sf ../../library/backend/*.h .; rm -f *_p.h; )
	( cd include/qtopia && rm -f *.h; ln -sf ../../library/*.h .; )
	( cd include/qtopia/private && rm -f *.h; ln -sf ../../../library/backend/*.h .; )
	( cd include/opie &&  rm -f *.h; ln -sf ../../libopie/*.h .; rm -f *_p.h; )
	( cd include/opie &&  ln -sf ../../libsql/*.h .; )
	( cd include/opie &&  ln -sf ../../libopie/pim/*.h .; )
	( cd include/opie; for generatedHeader in `cd ../../libopie; ls *.ui | sed -e "s,\.ui,\.h,g"`; do \
	ln -sf ../../libopie/$$generatedHeader $$generatedHeader; done )
	ln -sf ../../library/custom.h $(TOPDIR)/include/qpe/custom.h
	( cd include/qpe; ln -sf ../../x11/libqpe-x11/qpe/*.h .; )
	touch $@

$(TOPDIR)/library/custom.h : $(TOPDIR)/.config
	@-rm -f $@
	@$(if $(patsubst "%",%,$(CONFIG_CUSTOMFILE)),\
		ln -sf $(patsubst "%",%,$(CONFIG_CUSTOMFILE)) $@)
	@touch $@

$(TOPDIR)/scripts/lxdialog/lxdialog $(TOPDIR)/scripts/kconfig/mconf $(TOPDIR)/scripts/kconfig/conf $(TOPDIR)/scripts/kconfig/qconf $(TOPDIR)/scripts/kconfig/libkconfig.so $(TOPDIR)/scripts/kconfig/gconf $(TOPDIR)/qmake/qmake:
	@$(call descend,$(shell dirname $@),$(shell basename $@))

menuconfig: $(TOPDIR)/scripts/lxdialog/lxdialog $(TOPDIR)/scripts/kconfig/mconf ./config.in
	$(TOPDIR)/scripts/kconfig/mconf ./config.in
	@touch ./.config.stamp

xconfig: $(TOPDIR)/scripts/kconfig/qconf $(TOPDIR)/scripts/kconfig/libkconfig.so ./config.in
	$(TOPDIR)/scripts/kconfig/qconf ./config.in
	@touch .config.stamp

gconfig: $(TOPDIR)/scripts/kconfig/gconf $(TOPDIR)/scripts/kconfig/libkconfig.so ./config.in
	$(TOPDIR)/scripts/kconfig/gconf ./config.in
	@touch .config.stamp

config: $(TOPDIR)/scripts/kconfig/conf ./config.in
	$(TOPDIR)/scripts/kconfig/conf ./config.in
	@touch .config.stamp

oldconfig: $(TOPDIR)/scripts/kconfig/conf ./config.in
	$(TOPDIR)/scripts/kconfig/conf -o ./config.in
	@touch .config.stamp

randconfig: $(TOPDIR)/scripts/kconfig/conf ./config.in
	$(TOPDIR)/scripts/kconfig/conf -r ./config.in
	@touch .config.stamp

allyesconfig: $(TOPDIR)/scripts/kconfig/conf ./config.in
	$(TOPDIR)/scripts/kconfig/conf -y ./config.in
	@touch .config.stamp

allnoconfig: $(TOPDIR)/scripts/kconfig/conf ./config.in
	$(TOPDIR)/scripts/kconfig/conf -n ./config.in
	@touch .config.stamp

defconfig: $(TOPDIR)/scripts/kconfig/conf ./config.in
	$(TOPDIR)/scripts/kconfig/conf -d ./config.in
	@touch .config.stamp

$(TOPDIR)/qmake/qmake : $(TOPDIR)/mkspecs/default

$(TOPDIR)/mkspecs/default :
	ln -sf linux-g++ $@

$(TOPDIR)/scripts/subst : force
	@( \
		echo 's,\$$QPE_VERSION,$(QPE_VERSION),g'; \
		echo 's,\$$OPIE_VERSION,$(OPIE_VERSION),g'; \
		echo 's,\$$QTE_VERSION,$(QTE_VERSION),g'; \
		echo 's,\$$QTE_REVISION,$(QTE_REVISION),g'; \
		echo 's,\$$SUB_VERSION,$(SUB_VERSION),g'; \
		echo 's,\$$EXTRAVERSION,$(EXTRAVERSION),g'; \
		echo 's,\$$QTE_BASEVERSION,$(QTE_BASEVERSION),g'; \
	) > $@ || ( rm -f $@; exit 1 )

$(TOPDIR)/scripts/filesubst : force
	@( \
		echo 's,\$$OPIEDIR/root/,/,g'; \
		echo 's,$(OPIEDIR)/root/,/,g'; \
		echo 's,\$$OPIEDIR,$(prefix),g'; \
		echo 's,$(OPIEDIR),$(prefix),g'; \
		echo 's,\$$QTDIR,$(prefix),g'; \
		echo 's,$(QTDIR),$(prefix),g'; \
		echo 's,^\(\./\)*root/,/,g'; \
		echo 's,^\(\./\)*etc/,$(prefix)/etc/,g'; \
		echo 's,^\(\./\)*lib/,$(prefix)/lib/,g'; \
		echo 's,^\(\./\)*bin/,$(prefix)/bin/,g'; \
		echo 's,^\(\./\)*pics/,$(prefix)/pics/,g'; \
		echo 's,^\(\./\)*sounds/,$(prefix)/sounds/,g'; \
		echo 's,^\(\./\)*i18n/,$(prefix)/i18n/,g'; \
		echo 's,^\(\./\)*plugins/,$(prefix)/plugins/,g'; \
		echo 's,^\(\./\)*apps/,$(prefix)/apps/,g'; \
		echo 's,^\(\./\)*share/,$(prefix)/share/,g'; \
		echo 's,^\(\./\)*i18n/,$(prefix)/i18n/,g'; \
		echo 's,^\(\./\)*help/,$(prefix)/help/,g'; \
	) > $@ || ( rm -f $@; exit 1 )

## general rules ##

define descend
	$(MAKE) $(if $(QMAKE),QMAKE=$(QMAKE)) -C $(1) $(2)
endef

define makefilegen
	cd $(if $(1),$(shell dirname $(1))); $(TOPDIR)/qmake/qmake $(3) -o $(if $(1),$(shell basename $(1))) `cat $(OPIEDIR)/packages | grep "	\`echo $(1)|sed -e 's,/Makefile$$,,'\`	" | \
		head -1 | awk '{print $$3}'`
endef

define makecfg
	$(TOPDIR)/scripts/makecfg.pl $1 $(OPIEDIR)
endef
