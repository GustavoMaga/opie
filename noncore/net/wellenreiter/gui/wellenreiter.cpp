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
***********************************************************************/

// Qt

#include <qpushbutton.h>

// Standard

#include <unistd.h>
#include <sys/types.h>

// Local

#include "wellenreiter.h"
#include "scanlistitem.h"
#include "logwindow.h"
#include "hexwindow.h"

#include "../libwellenreiter/source/sock.hh"    // <--- ugly path, FIX THIS!
#include "../libwellenreiter/source/proto.hh"    // <--- ugly path, FIX THIS!
#include "../daemon/source/config.hh"           // <--- ugly path, FIX THIS!

Wellenreiter::Wellenreiter( QWidget* parent, const char* name, WFlags fl )
    : WellenreiterBase( parent, name, fl )
{

    logwindow->log( "(i) Wellenreiter has been started." );
    
    connect( button, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );
    netview->setColumnWidthMode( 1, QListView::Manual );

    //
    // setup socket for daemon communication and start poller
    //

    daemon_fd = commsock( GUIADDR, GUIPORT );
    if ( daemon_fd == -1 )
    {
        logwindow->log( "(E) Couldn't get file descriptor for commsocket." );
        qDebug( "D'oh! Could not get file descriptor for daemon-->gui communication socket." );
    }
    else
        startTimer( 700 );

}

Wellenreiter::~Wellenreiter()
{
    // no need to delete child widgets, Qt does it all for us
}

void Wellenreiter::handleMessage()
{
    // FIXME: receive message and handle it

    qDebug( "received message from daemon." );

    char buffer[128];

    int result = recvcomm( &daemon_fd, (char*) &buffer, sizeof(buffer) );
    qDebug( "received %d from recvcomm", result );

/*
typedef struct {
  int net_type;     1 = Accesspoint ; 2 = Ad-Hoc
  int ssid_len;     Length of SSID
  int channel;      Channel
  int wep;          1 = WEP enabled ; 0 = disabled
  char mac[64];     MAC address of Accesspoint
  char bssid[128];  BSSID of Accesspoint
} wl_network_t;
*/

    qDebug( "Sniffer sent: '%s'", (const char*) &buffer );
    hexwindow->log( (const char*) &buffer );

    if ( result == NETFOUND )  /* new network found */
    {
        logwindow->log( "(i) found new network" );
        qDebug( "Sniffer said: new network found." );
        wl_network_t n;
        get_network_found( &n, (char*) &buffer );
        
        qDebug( "Sniffer said: net_type is %d.", n.net_type );
        qDebug( "Sniffer said: MAC is %s", (const char*) &n.mac );

        //n.bssid[n.ssid_len] = "\0";

        QString type;

        if ( n.net_type == 1 )
            type = "managed";
        else
            type = "adhoc";
            
        addNewItem( type, n.bssid, QString( (const char*) &n.mac ), n.wep, n.channel, 0 );

    }

    else

    {
        qDebug( "unknown sniffer command." );
    }

}


bool Wellenreiter::hasMessage()
{

    // FIXME: do this in libwellenreiter, not here!!!

    fd_set rfds;
    FD_ZERO( &rfds );
    FD_SET( daemon_fd, &rfds );
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 10;
    int result = select( daemon_fd+1, &rfds, NULL, NULL, &tv );
    return FD_ISSET( daemon_fd, &rfds );
}

void Wellenreiter::timerEvent( QTimerEvent* e )
{
    qDebug( "checking for message..." );

    if ( hasMessage() )
    {
        handleMessage();
    }
    else
    {
        qDebug( "no message :(" );
    }
}

void Wellenreiter::addNewItem( QString type, QString essid, QString macaddr, bool wep, int channel, int signal )
{
    // FIXME: this code belongs in customized QListView, not into this class

    // search, if we had an item with this essid once before

    //MScanListItem* item = dynamic_cast<MScanListItem*>( netview->firstChild() );
    MScanListItem* item = static_cast<MScanListItem*>( netview->firstChild() );

    while ( item && ( item->text( 0 ) != essid ) )
    {
        qDebug( "itemtext: %s", (const char*) item->text( 0 ) );
        item = dynamic_cast<MScanListItem*>( item->itemBelow() );
    }
    if ( item )
    {
        qDebug( "found!" );
        
        // check, if it is the same item (based on MACaddr)
        
        if ( macaddr == item->text( 2 ) )
        {
            qDebug( "already had item with mac %s", (const char*) item->text( 2 ) );
            return;
        }
               
        // another item belonging to the same net, so: insert the new item as child
        
        new MScanListItem( item, type, essid, macaddr, wep, channel, signal );
    }
    
    else
    {
        qDebug( "inserting new network" );
        MScanListItem* network = new MScanListItem( netview, "networks", essid, QString::null, 0, 0, 0 );
    
        new MScanListItem( network, type, essid, macaddr, wep, channel, signal );
    }
}

void Wellenreiter::buttonClicked()
{

    // FIXME: communicate with daemon and set button text according to state

    button->setText( "Stop Scanning" );

    // add some test stations, so that we can see if the GUI part works

    addNewItem( "managed", "Vanille", "04:00:20:EF:A6:43", true, 6, 80 );
    addNewItem( "managed", "Vanille", "04:00:20:EF:A6:23", true, 11, 10 );
    addNewItem( "adhoc", "ELAN", "40:03:43:E7:16:22", false, 3, 10 );
    addNewItem( "adhoc", "ELAN", "40:03:53:E7:56:62", false, 3, 15 );
    addNewItem( "adhoc", "ELAN", "40:03:63:E7:56:E2", false, 3, 20 );
    
    QString command ("98");

    //sendcomm( DAEMONADDR, DAEMONPORT, (const char*) command );

}
