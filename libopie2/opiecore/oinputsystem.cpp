/*
 � � � � � � � �             This file is part of the Opie Project
              =.             (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
            .=l.
� � � � � �.>+-=
�_;:, � � .> � �:=|.         This program is free software; you can
.> <`_, � > �. � <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.-- � :           the terms of the GNU Library General Public
.="- .-=="i, � � .._         License as published by the Free Software
�- . � .-<_> � � .<>         Foundation; either version 2 of the License,
� � �._= =} � � � :          or (at your option) any later version.
� � .%`+i> � � � _;_.
� � .i_,=:_. � � �-<s.       This program is distributed in the hope that
� � �+ �. �-:. � � � =       it will be useful,  but WITHOUT ANY WARRANTY;
� � : .. � �.:, � � . . .    without even the implied warranty of
� � =_ � � � �+ � � =;=|`    MERCHANTABILITY or FITNESS FOR A
� _.=:. � � � : � �:=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.= � � � = � � � ;      Library General Public License for more
++= � -. � � .` � � .:       details.
�: � � = �...= . :.=-
�-. � .:....=;==+<;          You should have received a copy of the GNU
� -_. . . � )=. �=           Library General Public License along with
� � -- � � � �:-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "oinputsystem.h"
using namespace Opie::Core;

/* QT */
#include <qfile.h>

/* STD */
#include <errno.h>
#include <string.h>
#include <linux/input.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

#define BUFSIZE 256

/*======================================================================================
 * OInputSystem
 *======================================================================================*/

OInputSystem* OInputSystem::_instance = 0;

OInputSystem::OInputSystem() : QObject()
{
    qDebug( "OInputSystem::OInputSystem()" );
    synchronize();
}


void OInputSystem::synchronize()
{
    qDebug( "OInputSystem::synchronize()" );
    if ( QFile::exists( "/dev/input/event0" ) ) _devices.insert( "0", new OInputDevice( this, "/dev/input/event0" ) );
    if ( QFile::exists( "/dev/input/event1" ) ) _devices.insert( "1", new OInputDevice( this, "/dev/input/event1" ) );
    if ( QFile::exists( "/dev/input/event2" ) ) _devices.insert( "2", new OInputDevice( this, "/dev/input/event2" ) );
    if ( QFile::exists( "/dev/input/event3" ) ) _devices.insert( "3", new OInputDevice( this, "/dev/input/event3" ) );
    qDebug( "OInputSystem::synchronize() done" );
}


OInputSystem::~OInputSystem()
{
    qDebug( "OInputSystem::~OInputSystem()" );
}


int OInputSystem::count() const
{
    return _devices.count();
}


OInputDevice* OInputSystem::device( const QString& device ) const
{
    return _devices[device];
}


OInputSystem* OInputSystem::instance()
{
    if ( !_instance ) _instance = new OInputSystem();
    return _instance;
}


OInputSystem::DeviceIterator OInputSystem::iterator() const
{
    return OInputSystem::DeviceIterator( _devices );
}

/*======================================================================================
 * OInputDevice
 *======================================================================================*/

OInputDevice::OInputDevice( QObject* parent, const char* name ) : QObject( parent, name )
{
    qDebug( "OInputDevice::OInputDevice( '%s' )", name );
    
    _fd = ::open( name, O_RDONLY );
    if ( _fd == -1 )
    {
        qDebug( "OInputDevice::OInputDevice() - Warning: couldn't open %s (%s)", name, strerror( errno ) );
    }
}


OInputDevice::~OInputDevice()
{
    qDebug( "OInputDevice::~OInputDevice()" );
}


QString OInputDevice::identity() const
{
    char buf[BUFSIZE] = "<unknown>";
    ::ioctl( _fd, EVIOCGNAME(sizeof buf), buf );
    return buf;
}


QString OInputDevice::path() const
{
    char buf[BUFSIZE] = "<unknown>";
    ::ioctl( _fd, EVIOCGPHYS(sizeof buf), buf );
    return buf;
}


QString OInputDevice::uniq() const
{
    char buf[BUFSIZE] = "<unknown>";
    ::ioctl( _fd, EVIOCGUNIQ(sizeof buf), buf );
    return buf;
}

