# make install

# base opie install path
outdir = /opt/QtPalmtop

contains( TEMPLATE, lib ) {
	target.path = $$outdir/lib
}
!contains( TEMPLATE, lib ) {
	target.path = $$outdir/bin
}
INSTALLS += target

# ipkg control files
control.path = /CONTROL
control.files = control postinst prerm postrm preinst conffiles
INSTALLS += control

# images, default is $$outdir/pics/$$TARGET
pics.path = $$outdir/pics/$$TARGET
pics.files = pics/*
INSTALLS += pics

# sounds, default path is $$outdir/sounds/$$TARGET
sounds.path = $$outdir/sounds/$$TARGET
sounds.files = sounds/*
INSTALLS += sounds

# init scripts, default path is /etc/init.d
init.path = /etc/init.d
init.files = init.d/*
INSTALLS += init

# data, default path is /usr/share/$$TARGET
data.path = /usr/share/$$TARGET
data.files = share/*
INSTALLS += data

etc.path = $$outdir/etc/
etc.files = etc/*
INSTALLS += etc

apps.path = $$outdir/apps/
apps.files = apps/*
INSTALLS += apps

# sounds, default path is $$outdir/sounds/$$TARGET
sounds.path = $$outdir/sounds/$$TARGET
sounds.files = sounds/*
INSTALLS += sounds

# anything in nonstandard paths
root.path = /
root.files = root/*
INSTALLS += root

# new targets

lupdate.target = lupdate
lupdate.commands = lupdate -noobsolete $(PRO)

lrelease.target = lrelease
lrelease.commands = lrelease $(PRO)

ipk.target = ipk
ipk.commands = tmp=`mktemp -d /tmp/ipkg-opie.XXXXXXXXXX` && ( $(MAKE) INSTALL_ROOT="$$$$tmp" install && ipkg-build $$$$tmp; rm -rf $$$$tmp; )

QMAKE_EXTRA_UNIX_TARGETS += lupdate lrelease ipk
QMAKE_LFLAGS += -Wl,-rpath=$$outdir/lib
QMAKE_LIBDIR += $(OPIEDIR)/lib
