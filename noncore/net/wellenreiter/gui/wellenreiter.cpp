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

// Opie

#ifdef QWS
#include <opie/odevice.h>
using namespace Opie;
#endif

#ifdef QWS
#include <opie2/oapplication.h>
#else
#include <qapplication.h>
#endif
#include <opie2/onetwork.h>
#include <opie2/opcap.h>

// Qt

#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qtoolbutton.h>
#include <qmainwindow.h>

// Standard

#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>

// Local

#include "wellenreiter.h"
#include "scanlist.h"
#include "logwindow.h"
#include "hexwindow.h"
#include "configwindow.h"
#include "statwindow.h"
#include "manufacturers.h"

Wellenreiter::Wellenreiter( QWidget* parent )
    : WellenreiterBase( parent, 0, 0 ),
      sniffing( false ), iface( 0 ), manufacturerdb( 0 ), configwindow( 0 )
{

    //
    // construct manufacturer database
    //

    QString manufile;
    #ifdef QWS
    manufile.sprintf( "%s/share/wellenreiter/manufacturers.dat", (const char*) QPEApplication::qpeDir() );
    #else
    manufile.sprintf( "/usr/local/share/wellenreiter/manufacturers.dat" );
    #endif
    manufacturerdb = new ManufacturerDB( manufile );

    logwindow->log( "(i) Wellenreiter has been started." );

    //
    // detect operating system
    //

    #ifdef QWS
    QString sys;
    sys.sprintf( "(i) Running on '%s'.", (const char*) ODevice::inst()->systemString() );
    _system = ODevice::inst()->system();
    logwindow->log( sys );
    #endif

    // setup GUI
    netview->setColumnWidthMode( 1, QListView::Manual );

    if ( manufacturerdb )
        netview->setManufacturerDB( manufacturerdb );

    pcap = new OPacketCapturer();

}


Wellenreiter::~Wellenreiter()
{
    // no need to delete child widgets, Qt does it all for us

    delete manufacturerdb;
    delete pcap;
}


void Wellenreiter::setConfigWindow( WellenreiterConfigWindow* cw )
{
    configwindow = cw;
}


void Wellenreiter::channelHopped(int c)
{
    QString title = "Wellenreiter II -scan- [";
    QString left;
    if ( c > 1 ) left.fill( '.', c-1 );
    title.append( left );
    title.append( '|' );
    if ( c < iface->channels() )
    {
        QString right;
        right.fill( '.', iface->channels()-c );
        title.append( right );
    }
    title.append( "]" );
    //title.append( QString().sprintf( " %02d", c ) );
    assert( parent() );
    ( (QMainWindow*) parent() )->setCaption( title );
}


void Wellenreiter::receivePacket(OPacket* p)
{
    hexWindow()->log( p->dump( 8 ) );

    // check if we received a beacon frame
    OWaveLanManagementPacket* beacon = static_cast<OWaveLanManagementPacket*>( p->child( "802.11 Management" ) );
    if ( beacon && beacon->managementType() == "Beacon" )
    {
        QString type;
        if ( beacon->canIBSS() )
        {
            type = "adhoc";
        }
        else if ( beacon->canESS() )
        {
            type = "managed";
        }
        else
        {
            qDebug( "Wellenreiter::invalid frame detected: '%s'", (const char*) p->dump( 16 ) );
            return;
        }

        OWaveLanManagementSSID* ssid = static_cast<OWaveLanManagementSSID*>( p->child( "802.11 SSID" ) );
        QString essid = ssid ? ssid->ID() : QString("<unknown>");
        OWaveLanManagementDS* ds = static_cast<OWaveLanManagementDS*>( p->child( "802.11 DS" ) );
        int channel = ds ? ds->channel() : -1;

        OWaveLanPacket* header = static_cast<OWaveLanPacket*>( p->child( "802.11" ) );
        netView()->addNewItem( type, essid, header->macAddress2().toString(), beacon->canPrivacy(), channel, 0 );
        return;
    }

    // check for a data frame
    OWaveLanDataPacket* data = static_cast<OWaveLanDataPacket*>( p->child( "802.11 Data" ) );
    if ( data )
    {
        OWaveLanPacket* wlan = (OWaveLanPacket*) p->child( "802.11" );
        if ( wlan->fromDS() && !wlan->toDS() )
        {
            qDebug( "FromDS traffic: '%s' -> '%s' via '%s'",
                (const char*) wlan->macAddress3().toString(true),
                (const char*) wlan->macAddress1().toString(true),
                (const char*) wlan->macAddress2().toString(true) );
            netView()->traffic( "fromDS", wlan->macAddress3().toString(),
                                        wlan->macAddress1().toString(),
                                        wlan->macAddress2().toString() );
        }
        else
        if ( !wlan->fromDS() && wlan->toDS() )
        {
            qDebug( "ToDS traffic: '%s' -> '%s' via '%s'",
                (const char*) wlan->macAddress2().toString(true),
                (const char*) wlan->macAddress3().toString(true),
                (const char*) wlan->macAddress1().toString(true) );
            netView()->traffic( "toDS", wlan->macAddress2().toString(),
                                      wlan->macAddress3().toString(),
                                      wlan->macAddress1().toString() );
        }
        else
        if ( wlan->fromDS() && wlan->toDS() )
        {
            qDebug( "WSD(bridge) traffic: '%s' -> '%s' via '%s' and '%s'",
                (const char*) wlan->macAddress4().toString(true),
                (const char*) wlan->macAddress3().toString(true),
                (const char*) wlan->macAddress1().toString(true),
                (const char*) wlan->macAddress2().toString(true) );
            netView()->traffic( "WSD", wlan->macAddress4().toString(),
                                     wlan->macAddress3().toString(),
                                     wlan->macAddress1().toString(),
                                     wlan->macAddress2().toString() );
        }
        else
        {
            qDebug( "IBSS(AdHoc) traffic: '%s' -> '%s' (Cell: '%s')'",
                (const char*) wlan->macAddress2().toString(true),
                (const char*) wlan->macAddress1().toString(true),
                (const char*) wlan->macAddress3().toString(true) );
            netView()->traffic( "IBSS", wlan->macAddress2().toString(),
                                        wlan->macAddress1().toString(),
                                        wlan->macAddress3().toString() );
        }
        return;
    }
}


void Wellenreiter::stopClicked()
{
    disconnect( SIGNAL( receivedPacket(OPacket*) ), this, SLOT( receivePacket(OPacket*) ) );
    disconnect( SIGNAL( hopped(int) ), this, SLOT( channelHopped(int) ) );
    iface->setChannelHopping(); // stop hopping channels
    pcap->close();
    sniffing = false;
    #ifdef QWS
    oApp->setTitle();
    #else
    qApp->mainWidget()->setCaption( "Wellenreiter II" );
    #endif

    // get interface name from config window
    const QString& interface = configwindow->interfaceName->currentText();
    ONetwork* net = ONetwork::instance();
    iface = static_cast<OWirelessNetworkInterface*>(net->interface( interface ));

    // switch off monitor mode
    iface->setMonitorMode( false );
    // switch off promisc flag
    iface->setPromiscuousMode( false );

    system( "cardctl reset; sleep 1" ); //FIXME: Use OProcess
    logwindow->log( "(i) Stopped Scanning." );
    assert( parent() );
    ( (QMainWindow*) parent() )->setCaption( "Wellenreiter II" );

    // message the user
    QMessageBox::information( this, "Wellenreiter II", "Your wireless card\nshould now be usable again." );

    sniffing = false;
    emit( stoppedSniffing() );

    // print out statistics
    statwindow->log( "-----------------------------------------" );
    statwindow->log( "- Wellenreiter II Capturing Statistic -" );
    statwindow->log( "-----------------------------------------" );
    statwindow->log( "Packet Type         |   Receive Count" );

    for( QMap<QString,int>::ConstIterator it = pcap->statistics().begin(); it != pcap->statistics().end(); ++it )
    {
        QString left;
        left.sprintf( "%s", (const char*) it.key() );
        left = left.leftJustify( 20 );
        left.append( '|' );
        QString right;
        right.sprintf( "%d", it.data() );
        right = right.rightJustify( 7 );
        statwindow->log( left + right );
    }

}


void Wellenreiter::startClicked()
{
    // get configuration from config window

    const QString& interface = configwindow->interfaceName->currentText();
    const int cardtype = configwindow->daemonDeviceType();
    const int interval = configwindow->daemonHopInterval();

    if ( ( interface == "" ) || ( cardtype == 0 ) )
    {
        QMessageBox::information( this, "Wellenreiter II", "Your device is not\nproperly configured. Please reconfigure!" );
        return;
    }

    // configure device

    ONetwork* net = ONetwork::instance();
    iface = static_cast<OWirelessNetworkInterface*>(net->interface( interface ));

    // set monitor mode

    switch ( cardtype )
    {
        case 1: iface->setMonitoring( new OCiscoMonitoringInterface( iface ) ); break;
        case 2: iface->setMonitoring( new OWlanNGMonitoringInterface( iface ) ); break;
        case 3: iface->setMonitoring( new OHostAPMonitoringInterface( iface ) ); break;
        case 4: iface->setMonitoring( new OOrinocoMonitoringInterface( iface ) ); break;
        default:
            QMessageBox::information( this, "Wellenreiter II", "Bring your device into\nmonitor mode now." );
    }

    if ( cardtype > 0 && cardtype < 5 )
        iface->setMonitorMode( true );

    if ( !iface->monitorMode() )
    {
        QMessageBox::warning( this, "Wellenreiter II", "Can't set device into monitor mode." );
        return;
    }

    // open pcap and start sniffing
    pcap->open( interface );

    if ( !pcap->isOpen() )
    {
        QMessageBox::warning( this, "Wellenreiter II", "Can't open packet capturer:\n" + QString(strerror( errno ) ));
        return;
    }

    // set capturer to non-blocking mode
    pcap->setBlocking( false );

    // start channel hopper
    iface->setChannelHopping( 1000 ); //use interval from config window

    // connect
    connect( pcap, SIGNAL( receivedPacket(OPacket*) ), this, SLOT( receivePacket(OPacket*) ) );
    connect( iface->channelHopper(), SIGNAL( hopped(int) ), this, SLOT( channelHopped(int) ) );

    logwindow->log( "(i) Started Scanning." );
    sniffing = true;
    emit( startedSniffing() );
}
