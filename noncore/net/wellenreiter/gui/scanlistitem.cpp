/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "scanlistitem.h"
#include <assert.h>
#include <qdatetime.h>
#include <qpixmap.h>

#ifdef QWS
#include <qpe/resource.h>
#else
#include "resource.h"
#endif

const int col_type = 0;
const int col_essid = 0;
const int col_sig = 1;
const int col_ap = 2;
const int col_channel = 3;
const int col_wep = 4;
const int col_traffic = 5;
const int col_manuf = 6;
const int col_firstseen = 7;
const int col_lastseen = 8;

MScanListItem::MScanListItem( QListView* parent, QString type, QString essid, QString macaddr,
                              bool wep, int channel, int signal )
               :OListViewItem( parent, essid, QString::null, macaddr, QString::null, QString::null ),
                _type( type ), _essid( essid ), _macaddr( macaddr ), _wep( wep ),
                _channel( channel ), _signal( signal ), _beacons( 0 )
{
    qDebug( "creating scanlist item" );
    decorateItem( type, essid, macaddr, wep, channel, signal );
}

MScanListItem::MScanListItem( QListViewItem* parent, QString type, QString essid, QString macaddr,
                              bool wep, int channel, int signal )
               :OListViewItem( parent, essid, QString::null, macaddr, QString::null, QString::null )
{
    qDebug( "creating scanlist item" );
    decorateItem( type, essid, macaddr, wep, channel, signal );
}

void MScanListItem::decorateItem( QString type, QString essid, QString macaddr, bool wep, int channel, int signal )
{
    qDebug( "decorating scanlist item %s / %s / %s [%d]",
        (const char*) type,
        (const char*) essid,
        (const char*) macaddr,
        channel );

    // set icon for managed or adhoc mode
    QString name;
    name.sprintf( "wellenreiter/%s", (const char*) type );
    setPixmap( col_type, Resource::loadPixmap( name ) );

    // set icon for wep (wireless encryption protocol)
    if ( wep )
        setPixmap( col_wep, Resource::loadPixmap( "wellenreiter/cracked" ) ); //FIXME: rename the pixmap!

    // set channel and signal text

    if ( signal != -1 )
        setText( col_sig, QString::number( signal ) );
    if ( channel != -1 )
        setText( col_channel, QString::number( channel ) );

    setText( col_firstseen, QTime::currentTime().toString() );
    //setText( col_lastseen, QTime::currentTime().toString() );

    listView()->triggerUpdate();

    this->type = type;

}

void MScanListItem::setManufacturer( const QString& manufacturer )
{
    setText( col_manuf, manufacturer );
}

void MScanListItem::receivedBeacon()
{
    _beacons++;
    #ifdef DEBUG
    qDebug( "MScanListItem %s: received beacon #%d", (const char*) _macaddr, _beacons );
    #endif
    setText( col_sig, QString::number( _beacons ) );
    setText( col_lastseen, QTime::currentTime().toString() );
}
