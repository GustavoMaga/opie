/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QTOPIA_INTERNAL_FILEOPERATIONS
#define QTOPIA_INTERNAL_FILEOPERATIONS
#endif
#include "server.h"
#include "serverapp.h"
#include "taskbar.h"
#include "stabmon.h"
#include "launcher.h"
#include "firstuse.h"
#include "launcherglobal.h"

#include <qtopia/qpeapplication.h>
#include <qtopia/network.h>
#include <qtopia/config.h>
#include <qtopia/custom.h>
#include <qtopia/global.h>

#include <qfile.h>
#include <qdir.h>
#ifdef QWS
#include <qwindowsystem_qws.h>
#include <qtopia/qcopenvelope_qws.h>
#endif
#include <qtopia/alarmserver.h>

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#ifndef Q_OS_WIN32
#include <unistd.h>
#else
#include <process.h>
#endif

#include "calibrate.h"


#ifdef QT_QWS_LOGIN
#include "../login/qdmdialogimpl.h"
#endif

#ifdef Q_WS_QWS
#include <qkeyboard_qws.h>
#endif

#include <qmessagebox.h>
#include <opie/odevice.h>

using namespace Opie;


static void cleanup()
{
    QDir dir( Opie::Global::tempDir(), "qcop-msg-*" );

    QStringList stale = dir.entryList();
    QStringList::Iterator it;
    for ( it = stale.begin(); it != stale.end(); ++it ) {
	dir.remove( *it );
    }
}

static void refreshTimeZoneConfig()
{
    /* ### FIXME timezone handling */
#if 0
   // We need to help WorldTime in setting up its configuration for
   //   the current translation
    // BEGIN no tr
    const char *defaultTz[] = {
	"America/New_York",
	"America/Los_Angeles",
	"Europe/Oslo",
	"Asia/Tokyo",
	"Asia/Hong_Kong",
	"Australia/Brisbane",
	0
    };
    // END no tr

    TimeZone curZone;
    QString zoneID;
    int zoneIndex;
    Config cfg = Config( "WorldTime" );
    cfg.setGroup( "TimeZones" );
    if (!cfg.hasKey( "Zone0" )){
	// We have no existing timezones use the defaults which are untranslated strings
	QString currTz = TimeZone::current().id();
	QStringList zoneDefaults;
	zoneDefaults.append( currTz );
	for ( int i = 0; defaultTz[i] && zoneDefaults.count() < 6; i++ ) {
	    if ( defaultTz[i] != currTz )
		zoneDefaults.append( defaultTz[i] );
	}
	zoneIndex = 0;
	for (QStringList::Iterator it = zoneDefaults.begin(); it != zoneDefaults.end() ; ++it){
	    cfg.writeEntry( "Zone" + QString::number( zoneIndex ) , *it);
	    zoneIndex++;
	}
    }
    // We have an existing list of timezones refresh the
    //  translations of TimeZone name
    zoneIndex = 0;
    while (cfg.hasKey( "Zone"+ QString::number( zoneIndex ))){
	zoneID = cfg.readEntry( "Zone" + QString::number( zoneIndex ));
	curZone = TimeZone( zoneID );
	if ( !curZone.isValid() ){
	    qDebug( "initEnvironment() Invalid TimeZone %s", zoneID.latin1() );
	    break;
	}
	cfg.writeEntry( "ZoneName" + QString::number( zoneIndex ), curZone.city() );
	zoneIndex++;
    }
#endif
}

void initEnvironment()
{
#ifdef Q_OS_WIN32
    // Config file requires HOME dir which uses QDir which needs the winver
    qt_init_winver();
#endif
    Config config("locale");
    config.setGroup( "Location" );
    QString tz = config.readEntry( "Timezone", getenv("TZ") ).stripWhiteSpace();

    // if not timezone set, pick New York
    if (tz.isNull() || tz.isEmpty())
	tz = "America/New_York";

    setenv( "TZ", tz, 1 );
    config.writeEntry( "Timezone", tz);

    config.setGroup( "Language" );
    QString lang = config.readEntry( "Language", getenv("LANG") ).stripWhiteSpace();
    if( lang.isNull() || lang.isEmpty())
	lang = "en_US";

    setenv( "LANG", lang, 1 );
    config.writeEntry("Language", lang);
    config.write();

    config = Config("qpe");
    config.setGroup( "Rotation" );
    QString dispRep = config.readEntry( "Screen", getenv("QWS_DISPLAY") ).stripWhiteSpace();

/*
    if (!dispRep.isNull() && !dispRep.isEmpty()) {
	setenv( "QWS_DISPLAY", dispRep, 1 );
	config.writeEntry( "Screen", dispRep);
    }
*/
    QString keyOffset = config.readEntry( "Cursor", getenv("QWS_CURSOR_ROTATION") );

    if (keyOffset.isNull())
	keyOffset = "0";

    setenv( "QWS_CURSOR_ROTATION", keyOffset, 1 );
    config.writeEntry( "Cursor", keyOffset);
    config.write();
}

static void initBacklight()
{
#ifndef QT_NO_COP
    QCopEnvelope e("QPE/System", "setBacklight(int)" );
    e << -3; // Forced on
#endif
}

static void initKeyboard()
{
    Config config("qpe");

    config.setGroup( "Keyboard" );

    int ard = config.readNumEntry( "RepeatDelay" );
    int arp = config.readNumEntry( "RepeatPeriod" );
    if ( ard > 0 && arp > 0 )
	qwsSetKeyboardAutoRepeat( ard, arp );

    QString layout = config.readEntry( "Layout", "us101" );
    Server::setKeyboardLayout( layout );
}

static bool firstUse()
{
    bool needFirstUse = FALSE;
    if ( QWSServer::mouseHandler() ->inherits("QCalibratedMouseHandler") ) {
        if ( !QFile::exists( "/etc/pointercal" ) )
            needFirstUse = TRUE;
    }

    {
	Config config( "qpe" );
	config.setGroup( "Startup" );
	needFirstUse |= config.readBoolEntry( "FirstUse", TRUE );
    }

    if ( !needFirstUse )
	return FALSE;

    FirstUse *fu = new FirstUse();
    fu->exec();
    bool rs = fu->restartNeeded();
    delete fu;
    return rs;
}

int initApplication( int argc, char ** argv )
{
    cleanup();


    initEnvironment();

    //Don't flicker at startup:
#ifdef QWS
    QWSServer::setDesktopBackground( QImage() );
#endif
    ServerApplication a( argc, argv, QApplication::GuiServer );

    refreshTimeZoneConfig();

    initKeyboard();

    // Don't use first use under Windows
    if ( firstUse() ) {
	a.restart();
	return 0;
    }

    ODevice::inst ( )-> setSoftSuspend ( true );

    {
        QCopEnvelope e("QPE/System", "setBacklight(int)" );
  	e << -3; // Forced on
    }

    AlarmServer::initialize();



    Server *s = new Server();

    (void)new SysFileMonitor(s);
#ifdef QWS
    Network::createServer(s);
#endif

    s->show();

    /* THE ARM rtc has problem holdings the time on reset */
    if ( QDate::currentDate ( ). year ( ) < 2000 ) {
        if ( QMessageBox::information ( 0, ServerApplication::tr( "Information" ), ServerApplication::tr( "<p>The system date doesn't seem to be valid.\n(%1)</p><p>Do you want to correct the clock ?</p>" ). arg( TimeString::dateString ( QDate::currentDate ( ))), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes ) {
            QCopEnvelope e ( "QPE/Application/systemtime", "setDocument(QString)" );
            e << QString ( );
        }
    }

    int rv =  a.exec();

    qDebug("exiting...");
    delete s;

    ODevice::inst()->setSoftSuspend( false );

    return rv;
}

static const char *pidfile_path = "/var/run/opie.pid";

void create_pidfile ( )
{
	FILE *f;

	if (( f = ::fopen ( pidfile_path, "w" ))) {
		::fprintf ( f, "%d", getpid ( ));
		::fclose ( f );
	}
}

void remove_pidfile ( )
{
	::unlink ( pidfile_path );
}

void handle_sigterm ( int /* sig */ )
{
	if ( qApp )
		qApp-> quit ( );
}

#ifndef Q_OS_WIN32
int main( int argc, char ** argv )
{

    ::signal ( SIGCHLD, SIG_IGN );

    ::signal ( SIGTERM, handle_sigterm );
    ::signal ( SIGINT, handle_sigterm );

    ::setsid ( );
    ::setpgid ( 0, 0 );

    ::atexit ( remove_pidfile );
    create_pidfile ( );

    int retVal = initApplication( argc, argv );

    // Have we been asked to restart?
    if ( ServerApplication::doRestart ) {
	for ( int fd = 3; fd < 100; fd++ )
	    close( fd );

	execl( (QPEApplication::qpeDir()+"bin/qpe").latin1(), "qpe", 0 );
    }

    // Kill them. Kill them all.
    ::kill ( 0, SIGTERM );
    ::sleep ( 1 );
    ::kill ( 0, SIGKILL );

    return retVal;
}
#else

int main( int argc, char ** argv )
{
    int retVal = initApplication( argc, argv );

    if ( DesktopApplication::doRestart ) {
	qDebug("Trying to restart");
	execl( (QPEApplication::qpeDir()+"bin\\qpe").latin1(), "qpe", 0 );
    }

    return retVal;
}

#endif

