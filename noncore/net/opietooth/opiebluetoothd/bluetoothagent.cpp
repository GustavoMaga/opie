/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2011 Paul Eggleton <bluelightning@bluelightning.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this file; see the file COPYING.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

// local includes
#include "bluetoothagent.h"
#include "bluetoothpindlg.h"

// Qt includes
#include <qvaluelist.h>
#include <qstringlist.h>
#include <qtimer.h>
#include <qmessagebox.h>

// Qt DBUS includes
#include <dbus/qdbusdatalist.h>
#include <dbus/qdbusdatamap.h>
#include <dbus/qdbuserror.h>
#include <dbus/qdbusmessage.h>
#include <dbus/qdbusproxy.h>
#include <dbus/qdbusobjectpath.h>
#include <dbus/qdbusvariant.h>

// Qtopia includes
#include <qpe/qpeapplication.h>

// Opie includes
#include <opie2/odebug.h>

#define AGENT_OBJECT_PATH   "/org/opie/bluetooth/agent"

OBluetoothAgent::OBluetoothAgent(const QDBusConnection& connection, QDBusProxy *adapterProxy)
    : m_connection(connection)
{
    m_pinDlg = NULL;
    m_connection.registerObject(AGENT_OBJECT_PATH, this);

    odebug << "Object registered for path " << AGENT_OBJECT_PATH << " on unique name " <<
           m_connection.uniqueName().local8Bit().data() << oendl;

    // Register agent
    QValueList<QDBusData> parameters;
    parameters << QDBusData::fromObjectPath(QDBusObjectPath(AGENT_OBJECT_PATH));
    parameters << QDBusData::fromString("DisplayYesNo");
    adapterProxy->sendWithAsyncReply("RegisterAgent", parameters);
}

OBluetoothAgent::~OBluetoothAgent()
{
    m_connection.unregisterObject(AGENT_OBJECT_PATH);
    delete m_pinDlg;
}

bool OBluetoothAgent::handleMethodCall(const QDBusMessage& message)
{
    odebug << "OBluetoothAgent::handleMethodCall: interface='" << message.interface().latin1() << "', member='" << message.member().latin1() << "'" << oendl;

    if (message.interface() != "org.bluez.Agent")
        return false;

    if (message.member() == "RequestPinCode") {
        destroyDialog();
        m_pinDlg = new PinDlg(0, "pindlg", FALSE);
        m_authMsg = new QDBusMessage(message);
        connect( m_pinDlg, SIGNAL(dialogClosed(bool)), this, SLOT(pinDialogClosed(bool)) );

        // Get the device's name and address
        QDBusProxy proxy;
        proxy.setService("org.bluez");
        proxy.setPath(message[0].toObjectPath());
        proxy.setInterface("org.bluez.Device");
        proxy.setConnection(m_connection);
        QDBusMessage reply = proxy.sendWithReply("GetProperties", QValueList<QDBusData>());
        QString name = tr("Unknown Bluetooth device");
        QString bdaddr = message[0].toObjectPath();
        if (reply.type() == QDBusMessage::ReplyMessage && reply.count() == 1) {
            if (reply[0].type() == QDBusData::Map) {
                if( reply[0].keyType() == QDBusData::String ) {
                    const QDBusDataMap<QString> map = reply[0].toStringKeyMap();
                    QDBusDataMap<QString>::ConstIterator it = map.begin();
                    for (; it != map.end(); ++it) {
                        if( it.key() == "Address" )
                            bdaddr = it.data().toVariant().value.toString();
                        else if( it.key() == "Name" )
                            name = it.data().toVariant().value.toString();
                    }
                }
            }
        }
        m_pinDlg->setRemoteName(name);
        m_pinDlg->setBdAddr(bdaddr);
        m_pinDlg->showMaximized();
        return true;
    }
    else if (message.member() == "Cancel") {
        destroyDialog();
        QMessageBox::message(tr("Bluetooth"),tr("Authentication was cancelled"));
        return true;
    }

    return false;
}

void OBluetoothAgent::pinDialogClosed( bool accepted )
{
    if( accepted ) {
        QDBusMessage reply = QDBusMessage::methodReply(*m_authMsg);
        reply << QDBusData::fromString(m_pinDlg->pin());
        m_connection.send(reply);
    }
    else {
        QDBusError error("org.bluez.Error.Canceled", tr("User cancelled"));
        QDBusMessage reply = QDBusMessage::methodError(*m_authMsg, error);
        m_connection.send(reply);
    }
    destroyDialog();
}

void OBluetoothAgent::destroyDialog()
{
    if( m_pinDlg ) {
        delete m_pinDlg;
        m_pinDlg = NULL;
        delete m_authMsg;
        m_authMsg = NULL;
    }
}
