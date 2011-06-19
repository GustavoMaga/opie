/*
 � � � � � � � �             This file is part of the Opie Project
             =.              Copyright (c) 2002 Maximilian Reiss <max.reiss@gmx.de>
            .=l.             Copyright (c) 2011 Paul Eggleton <paul.eggleton@linux.intel.com>
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


#include "bluezapplet.h"

/* OPIE */
#include <opie2/otaskbarapplet.h>
#include <opie2/odebug.h>
#include <opie2/oresource.h>
#include <opie2/oprocess.h>
#include <qpe/version.h>
#include <qpe/applnk.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/config.h>

using namespace Opie::Core;

/* QT */
#include <qapplication.h>
#include <qpoint.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qtimer.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qtextstream.h>

namespace OpieTooth {

BluezApplet::BluezApplet( QWidget *parent, const char *name ) : QWidget( parent, name )
{
    setFixedHeight( AppLnk::smallIconSize() );
    setFixedWidth( AppLnk::smallIconSize() );

    bluezOnPixmap = OResource::loadImage( "bluetoothapplet/bluezon", OResource::SmallIcon );
    bluezOffPixmap = OResource::loadImage( "bluetoothapplet/bluezoff", Opie::Core::OResource::SmallIcon );
    bluezReceiveOnPixmap = OResource::loadImage( "bluetoothapplet/bluezonreceive", Opie::Core::OResource::SmallIcon );

    m_adapter = false;
    m_powered = false;
    m_receive = false;

    // TODO: determine whether this channel has to be closed at destruction time.
    QCopChannel* chan = new QCopChannel("QPE/Bluetooth", this );
    connect(chan, SIGNAL(received(const QCString&,const QByteArray&) ),
            this, SLOT(slotMessage(const QCString&,const QByteArray&) ) );

    QCopChannel* chan2 = new QCopChannel("QPE/BluetoothBack", this );
    connect(chan2, SIGNAL(received(const QCString&,const QByteArray&) ),
            this, SLOT(slotBackMessage(const QCString&,const QByteArray&) ) );

    m_statusRequested = true;
    QCopEnvelope e("QPE/Bluetooth", "sendStatus()");
}

BluezApplet::~BluezApplet()
{
}

int BluezApplet::position()
{
    return 6;
}

void BluezApplet::setBluezStatus( bool enable )
{
    if( enable )
        QCopEnvelope e("QPE/Bluetooth", "enableBluetooth()");
    else
        QCopEnvelope e("QPE/Bluetooth", "disableBluetooth()");
}

int BluezApplet::setObexRecieveStatus(int d)
{
    {
        QCopEnvelope e ( "QPE/Obex", "btreceive(int)" );
        e << ( d ? 1 : 0 );
    }
    m_receive = (bool)(d != 0);
    return d;
}

// receiver for QCopChannel("QPE/Bluetooth") messages.
void BluezApplet::slotMessage( const QCString& str, const QByteArray& data )
{
    QDataStream stream ( data, IO_ReadOnly );
    if( str == "statusChange(int)" || ( str == "status(int)" && m_statusRequested ) ) {
        if(str == "status(int)")
            m_statusRequested = false;
        int status;
        stream >> status;
        // FIXME constants
        m_adapter = (( status & 1 ) == 1);
        m_powered = (( status & 2 ) == 2);
        update();
    }
}

// receiver for QCopChannel("QPE/BluetoothBack") messages.
void BluezApplet::slotBackMessage( const QCString& message, const QByteArray& data )
{
    QDataStream stream ( data, IO_ReadOnly );
    QString msgout;
    if( message == "devicePaired(QString)" ) {
        msgout = "Device paired";
    }
    else if( message == "deviceAlreadyPaired(QString)" ) {
        msgout = "Device already paired";
    }
    else if( message == "error(QString,QString,QString)" ) {
        // Adapter related error
        QString errsrc, errname, errmsg;
        stream >> errsrc;
        stream >> errname;
        stream >> errmsg;
        msgout = errmsg;
    }
    else if( message == "error(QString,QString,QString,QString)" ) {
        // Device related error
        QString addr, errsrc, errname, errmsg;
        stream >> addr;
        stream >> errsrc;
        stream >> errname;
        stream >> errmsg;
        msgout = errmsg;
        if( errsrc == "CreatePairedDevice" )
            msgout = tr("Pairing failed: %1").arg(errmsg);
        else {
            if( errsrc.startsWith("org.bluez") ) {
                int pos = errsrc.findRev('.');
                if( pos > 0 ) {
                    QString op = errsrc.mid(pos+1).lower();
                    errsrc = friendlyServiceName( errsrc.left(pos) );
                    msgout = tr("%1 %2 failed: %3").arg(errsrc).arg(op).arg(errmsg);
                }
                else
                    msgout = tr("%1 failed: %2").arg(errsrc).arg(msgout);
            }
        }
    }
    else if( message == "deviceConnected(QString,QString,QString)" ) {
        QString addr, dbusintf, retval;
        stream >> addr;
        stream >> dbusintf;
        stream >> retval;
        if( retval.isEmpty() )
            msgout = tr("%1 connected").arg(friendlyServiceName(dbusintf));
        else
            msgout = tr("%1 connected (%2)").arg(friendlyServiceName(dbusintf)).arg(retval);
    }
    else if( message == "deviceDisconnected(QString,QString)" ) {
        QString addr, dbusintf;
        stream >> addr;
        stream >> dbusintf;
        msgout = tr("%1 disconnected").arg(friendlyServiceName(dbusintf));
    }

    if( !msgout.isEmpty() ) {
        QCopEnvelope e("QPE/TaskBar", "message(QString,QString)");
        e << msgout;
        e << QString("bluetoothapplet");
    }
}

QString BluezApplet::friendlyServiceName( const QString &dbusintf )
{
    QString friendlyName = dbusintf;
    if( dbusintf == "org.bluez.Input" )
        friendlyName = tr("Input device");
    else if( dbusintf == "org.bluez.Network" )
        friendlyName = tr("Serial device");
    else if( dbusintf == "org.bluez.Network" )
        friendlyName = tr("Network");

    return friendlyName;
}

void BluezApplet::mousePressEvent( QMouseEvent *)
{
    QPopupMenu *menu = new QPopupMenu();
    QPopupMenu *signal = new QPopupMenu();
    int ret=0;

    if (m_powered) {
        menu->insertItem( tr("Disable Bluetooth"), 0 );
    }
    else {
        menu->insertItem( tr("Enable Bluetooth"), 1 );
        menu->setItemEnabled(1, m_adapter);
    }

    menu->insertItem( tr("Launch manager"), 2 );

    if (m_powered) {
        menu->insertSeparator(6);
        if (m_receive)
            menu->insertItem( tr("Disable receive"), 3 );
        else
            menu->insertItem( tr("Enable receive"), 4 );
    }

    QPoint p = mapToGlobal( QPoint(1, -menu->sizeHint().height()-1) );
    ret = menu->exec(p, 0);
    menu->hide();

    switch(ret) {
    case 0:
        setBluezStatus(false);
        break;
    case 1:
        setBluezStatus(true);
        break;
    case 2:
        // start bluetoothmanager
        launchManager();
        break;
    case 3:
        setObexRecieveStatus(0);
        break;
    case 4:
        setObexRecieveStatus(1);
        break;
    }

    delete signal;
    delete menu;
}


/**
 * Launches the bluetooth manager
 */
void BluezApplet::launchManager()
{
    QCopEnvelope e("QPE/System", "execute(QString)");
    e << QString("bluetooth-manager");
}

/**
 * Implementation of the paint event
 * @param the QPaintEvent
 */
void BluezApplet::paintEvent( QPaintEvent* )
{
    QPainter p(this);
    odebug << "paint bluetooth pixmap" << oendl;

    if (m_powered) {
        p.drawPixmap( 0, 0,  bluezOnPixmap );
        if( m_receive )
            p.drawPixmap( 0, 0,  bluezReceiveOnPixmap );
    }
    else
        p.drawPixmap( 0, 0,  bluezOffPixmap );
}

}

EXPORT_OPIE_APPLET_v1( OpieTooth::BluezApplet )

