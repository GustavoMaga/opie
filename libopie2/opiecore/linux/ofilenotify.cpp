/*
� � � � � � � �             This file is part of the Opie Project
             =.             Copyright (C) 2004-2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
           .=l.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
 � � � � �.>+-=
_;:, � � .> � �:=|.         This program is free software; you can
.> <`_, � > �. � <=         redistribute it and/or  modify it under
:`=1 )Y*s>-.-- � :          the terms of the GNU Library General Public
.="- .-=="i, � � .._        License as published by the Free Software
- . � .-<_> � � .<>         Foundation; version 2 of the License.
 � �._= =} � � � :
 � .%`+i> � � � _;_.
 � .i_,=:_. � � �-<s.       This program is distributed in the hope that
 � �+ �. �-:. � � � =       it will be useful,  but WITHOUT ANY WARRANTY;
 � : .. � �.:, � � . . .    without even the implied warranty of
 � =_ � � � �+ � � =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:. � � � : � �:=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.= � � � = � � � ;     Library General Public License for more
++= � -. � � .` � � .:      details.
: � � = �...= . :.=-
-. � .:....=;==+<;          You should have received a copy of the GNU
 -_. . . � )=. �=           Library General Public License along with
 � -- � � � �:-=`           this library; see the file COPYING.LIB.
                            If not, write to the Free Software Foundation,
                            Inc., 59 Temple Place - Suite 330,
                            Boston, MA 02111-1307, USA.
*/

#include "ofilenotify.h"
using namespace Opie::Core;

/* OPIE */

/* QT */
#include <qobject.h>
#include <qsocketnotifier.h>
#include <qsignal.h>
#include <qintdict.h>
#include <qdir.h>

/* STD */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

static QIntDict<OFileNotification> notification_list;

QSocketNotifier* OFileNotification::_sn;
int OFileNotification::_fd = -1;

#define INOTIFY_DEVICE "/dev/inotify"

namespace Opie {
namespace Core {

//=================================================================================================
// OFile
//=================================================================================================

OFile::OFile() : QObject( 0, 0 ), QFile()
{
    qDebug( "OFile()" );
}

OFile::OFile( const QString& name ) : QObject( 0, 0 ), QFile( name )
{
    qDebug( "OFile()" );
}

OFile::~OFile()
{
    qDebug( "~OFile()" );
}

void OFile::connectNotify( const char *signal )
{
    QString s = normalizeSignalSlot( signal+1 );
    qDebug( "OFile::connectNotify() signal = '%s'", (const char*) s );

    if ( s.startsWith( "accessed" ) )







    QObject::connectNotify( signal );

/*
    void accessed( const QString& );
    void modified( const QString& );
    void attributed( const QString& );
    void closed( const QString&, bool );
    void opened( const QString& );
    void deleted( const QString& );
    void unmounted( const QString& );
*/

}

void OFile::disconnectNotify( const char* signal )
{
    qDebug( "OFile::disconnectNotify() signal = '%s'", signal );
    QObject::disconnectNotify( signal );
}

int OFile::startWatch( int mode )
{
}

//=================================================================================================
// OFileNotificationEvent
//=================================================================================================
OFileNotificationEvent::OFileNotificationEvent( OFileNotification* parent, int wd, unsigned int mask, unsigned int cookie, const QString& name )
                       :_parent( parent ), _wd( wd ), _mask( mask ), _cookie( cookie ), _name( name )
{
    qDebug( "OFileNotificationEvent()" );
}


OFileNotificationEvent::~OFileNotificationEvent()
{
    qDebug( "~OFileNotificationEvent()" );
}

//=================================================================================================
// OFileNotification
//=================================================================================================
OFileNotification::OFileNotification( QObject* parent, const char* name )
                  :QObject( parent, name ), _active( false ), _multi( true )
{
    qDebug( "OFileNotification::OFileNotification()" );
}


OFileNotification::~OFileNotification()
{
    stop();
    qDebug( "OFileNotification::~OFileNotification()" );
}


bool OFileNotification::isActive() const
{
    return _active;
}


int OFileNotification::watch( const QString& path, bool sshot, OFileNotificationType type )
{
    // check if path exists and is a regular file
    struct stat s;
    if ( ::stat( (const char*) path, &s ) == -1 )
    {
        qWarning( "OFileNotification::watch(): Can't watch '%s': %s.", (const char*) path, strerror( errno ) );
        return -1;
    }
    if ( !S_ISREG( s.st_mode ) )
    {
        qWarning( "OFileNotification::watch(): Can't watch '%s': %s.", (const char*) path, "not a regular file" );
        return -1;
    }

    return startWatching( path, sshot, type );
}


int OFileNotification::startWatching( const QString& path, bool sshot, OFileNotificationType type )
{
    if ( notification_list.isEmpty() )
    {
        OFileNotification::registerEventHandler();
    }

    struct inotify_watch_request iwr;
    ::memset( &iwr, 0, sizeof iwr );
    iwr.name = const_cast<char*>( (const char*) path );
    iwr.mask = type;

    _wd = ::ioctl( OFileNotification::_fd, INOTIFY_WATCH, &iwr );

    if ( _wd < 0 )
    {
        qWarning( "OFileNotification::watch(): inotify can't watch '%s': %s.", (const char*) path, strerror( errno ) );
        return -1;
    }

    notification_list.insert( _wd, this );
    _path = path;
    _multi = !sshot;
    _type = type;
    _active = true;
    qDebug( "OFileNotification::watch(): watching '%s' [wd=%d].", (const char*) path, _wd );
    return _wd;
}


void OFileNotification::stop()
{
    notification_list.remove( _wd );
    _path = QString::null;
    _wd = 0;
    _active = false;
    if ( notification_list.isEmpty() )
    {
        OFileNotification::unregisterEventHandler();
    }
}


OFileNotificationType OFileNotification::type() const
{
    return _type;
}


QString OFileNotification::path() const
{
    return _path;
}


bool OFileNotification::activate( const OFileNotificationEvent* e )
{
    qDebug( "OFileNotification::activate(): e = ( %s, %d, 0x%08x, %d, %s )", (const char*) _path, e->descriptor(), e->mask(), e->cookie(), (const char*) e->name() );

    // dumb signal
    _signal.activate();

    // generic signal
    emit triggered( _path, e->mask(), e->name() );

    // specialized signals
    switch ( e->mask() )
    {
        case Access:        emit accessed( _path );                 break;
        case Modify:        emit modified( _path );                 break;
        case Attrib:        emit attributed( _path);                break;
        case CloseWrite:    emit closed( _path, true );             break;
        case CloseNoWrite:  emit closed( _path, false );            break;
        case Open:          emit opened( _path );                   break;
        case MovedFrom:     emit movedFrom( _path, e->name() );     break;
        case MovedTo:       emit movedTo( _path, e->name() );       break;
        case DeleteSubdir:  emit deletedSubdir( _path, e->name() ); break;
        case DeleteFile:    emit deletedFile( _path, e->name() );   break;
        case CreateSubdir:  emit createdSubdir( _path, e->name() ); break;
        case CreateFile:    emit createdFile( _path, e->name() );   break;
        case DeleteSelf:    emit deleted( _path );                  break;
        case Unmount:       emit unmounted( _path );                break;
        default: assert( 0 );
    }

    if ( !_multi ) stop();

    return true;
}


bool OFileNotification::singleShot( const QString& path, QObject* receiver, const char* member, OFileNotificationType type )
{
    OFileNotification* ofn = new OFileNotification();
    ofn->_signal.connect( receiver, member );
    return ofn->watch( path, true, type ) != -1;
}


void OFileNotification::inotifyEventHandler()
{
    qDebug( "OFileNotification::inotifyEventHandler(): reached." );

    char buffer[16384];
    ssize_t buffer_i;
    struct inotify_event *pevent, *event;
    ssize_t r;
    size_t event_size;
    int count = 0;

    r = ::read(_fd, buffer, 16384);

    if ( r <= 0 )
        return;

    buffer_i = 0;
    while ( buffer_i < r )
    {
        pevent = (struct inotify_event *)&buffer[buffer_i];
        event_size = sizeof(struct inotify_event) + pevent->len;
        OFileNotificationEvent* e = new OFileNotificationEvent( notification_list[ pevent->wd ], pevent->wd, pevent->mask,
                                                                pevent->cookie, pevent->len ? pevent->name : 0 );
        e->activate();
        buffer_i += event_size;
        count++;
    }

    qDebug( "OFileNotification::inotifyEventHandler(): processed %d events", count );
}


bool OFileNotification::registerEventHandler()
{
    OFileNotification::_fd = ::open( INOTIFY_DEVICE, O_RDONLY );
    if ( OFileNotification::_fd < 0 )
    {
        qWarning( "OFileNotification::registerEventHandler(): couldn't register event handler: %s", strerror( errno ) );
        return false;
    }

    OFileNotification::_sn = new QSocketNotifier( _fd, QSocketNotifier::Read, this, "inotify event" );
    connect( OFileNotification::_sn, SIGNAL( activated(int) ), this, SLOT( inotifyEventHandler() ) );

    qDebug( "OFileNotification::registerEventHandler(): done" );
    return true;
}


void OFileNotification::unregisterEventHandler()
{
    if ( _sn ) delete _sn;
    if ( OFileNotification::_fd )
    ::close( OFileNotification::_fd );
    qDebug( "OFileNotification::unregisterEventHandler(): done" );
}

//=================================================================================================
// ODirNotification
//=================================================================================================
ODirNotification::ODirNotification( QObject* parent, const char* name )
                  :QObject( parent, name )
{
    qDebug( "ODirNotification::ODirNotification()" );
}


ODirNotification::~ODirNotification()
{
    qDebug( "ODirNotification::~ODirNotification()" );
}


int ODirNotification::watch( const QString& path, bool sshot, OFileNotificationType type, int recurse )
{
    qDebug( "ODirNotification::watch( %s, %d, 0x%08x, %d )", (const char*) path, sshot, type, recurse );

    OFileNotification* fn = new OFileNotification( this, "ODirNotification delegate" );
    int result = fn->startWatching( path, sshot, type );
    if ( result != -1 )
    {
        connect( fn, SIGNAL( triggered( const QString&, unsigned int, const QString& ) ), this, SIGNAL( triggered( const QString&, unsigned int, const QString& ) ) );
        connect( fn, SIGNAL( accessed( const QString& ) ), this, SIGNAL( accessed( const QString& ) ) );
        connect( fn, SIGNAL( modified( const QString& ) ), this, SIGNAL( modified( const QString& ) ) );
        connect( fn, SIGNAL( attributed( const QString& ) ), this, SIGNAL( attributed( const QString& ) ) );
        connect( fn, SIGNAL( closed( const QString&, bool ) ), this, SIGNAL( closed( const QString&, bool ) ) );
        connect( fn, SIGNAL( opened( const QString& ) ), this, SIGNAL( opened( const QString& ) ) );
        connect( fn, SIGNAL( movedTo( const QString&, const QString& ) ), this, SIGNAL( movedTo( const QString&, const QString& ) ) );
        connect( fn, SIGNAL( movedFrom( const QString&, const QString& ) ), this, SIGNAL( movedFrom( const QString&, const QString& ) ) );
        connect( fn, SIGNAL( deletedSubdir( const QString&, const QString& ) ), this, SIGNAL( deletedSubdir( const QString&, const QString& ) ) );
        connect( fn, SIGNAL( deletedFile( const QString&, const QString& ) ), this, SIGNAL( deletedFile( const QString&, const QString& ) ) );;
        connect( fn, SIGNAL( createdSubdir( const QString&, const QString& ) ), this, SIGNAL( createdSubdir( const QString&, const QString& ) ) );
        connect( fn, SIGNAL( createdFile( const QString&, const QString& ) ), this, SIGNAL( createdFile( const QString&, const QString& ) ) );
        connect( fn, SIGNAL( deleted( const QString& ) ), this, SIGNAL( deleted( const QString& ) ) );
        connect( fn, SIGNAL( unmounted( const QString& ) ), this, SIGNAL( unmounted( const QString& ) ) );

        if ( recurse )
        {
            QDir directory( path );
            QStringList subdirs = directory.entryList( QDir::Dirs );

            for ( QStringList::Iterator it = subdirs.begin(); it != subdirs.end(); ++it )
            {
                if ( (*it) == "." || (*it) == ".." ) continue;
                QString subpath = QString( "%1/%2" ).arg( path ).arg( *it );
                int subresult = watch( subpath, sshot, type, recurse-1 );
                if ( subresult == -1 )
                {
                    qDebug( "ODirNotification::watch(): subresult for '%s' was -1. Interrupting", (const char*) (*it) );
                    return -1;
                }
            }
        }
//connect( fn, SIGNAL( triggered( const QString&, unsigned int, const QString& ) ), this, SIGNAL( triggered( const QString&, unsigned int, const QString& ) ) );
    }
    else return -1;
}


// void ODirNotification::subdirCreated( const QString& name )


/*
  Love-Trowbridge recursive directory scanning algorithm:

        Step 1.  Start at initial directory foo.  Add watch.

        Step 2.  Setup handlers for watch created in Step 1.
                 Specifically, ensure that a directory created
                 in foo will result in a handled CREATE_SUBDIR
                 event.

        Step 3.  Read the contents of foo.

        Step 4.  For each subdirectory of foo read in step 3, repeat
                 step 1.

        Step 5.  For any CREATE_SUBDIR event on bar, if a watch is
                 not yet created on bar, repeat step 1 on bar.
*/


} // namespace Ui

} // namespace Opie
