/*
                             This file is part of the Opie Project
                             Copyright (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
� � � � � �.>+-=
�_;:, � � .> � �:=|.         This program is free software; you can
.> <`_, � > �. � <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.-- � :           the terms of the GNU Library General Public
.="- .-=="i, � � .._         License as published by the Free Software
�- . � .-<_> � � .<>         Foundation; version 2 of the license.
� � �._= =} � � � :
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

#include "obluetooth.h"

/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;

/* STD */
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>

namespace Opie {
namespace Shower {

/*======================================================================================
 * OBluetooth
 *======================================================================================*/

OBluetooth* OBluetooth::_instance = 0;

OBluetooth::OBluetooth()
{
    synchronize();
}

OBluetooth* OBluetooth::instance()
{
    if ( !_instance ) _instance = new OBluetooth();
    return _instance;
}

OBluetooth::InterfaceIterator OBluetooth::iterator() const
{
    return OBluetooth::InterfaceIterator( _interfaces );
}

int OBluetooth::count() const
{
    return _interfaces.count();
}

OBluetoothInterface* OBluetooth::interface( const QString& iface ) const
{
    return _interfaces[iface];
}

void OBluetooth::synchronize()
{
    odebug << "OBluetooth::synchronize() - gathering available HCI devices" << oendl;
    _interfaces.clear();

    _fd = ::socket( AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI );
    if ( _fd == -1 )
    {
        owarn << "OBluetooth::synchronize() - can't open HCI control socket (" << strerror( errno ) << ")" << oendl;
        return;
    }

    struct hci_dev_list_req *dl;
    struct hci_dev_req *dr;
    struct hci_dev_info di;

    if (!(dl = (struct hci_dev_list_req*)malloc(HCI_MAX_DEV * sizeof(struct hci_dev_req) + sizeof(uint16_t))))
    {
        ofatal << "OBluetooth::synchronize() - can't allocate memory for HCI request" << oendl;
        return;
    }

    dl->dev_num = HCI_MAX_DEV;
    dr = dl->dev_req;

    if (ioctl( _fd, HCIGETDEVLIST, (void *) dl) == -1)
    {
        owarn << "OBluetooth::synchronize() - can't complete HCIGETDEVLIST (" << strerror( errno ) << ")" << oendl;
        return;
    }

    for ( int i = 0; i < dl->dev_num; ++i )
    {
        di.dev_id = ( dr + i )->dev_id;
        if ( ioctl( _fd, HCIGETDEVINFO, (void *) &di) < 0 )
            continue;
        if ( hci_test_bit(HCI_RAW, &di.flags) && !bacmp(&di.bdaddr, BDADDR_ANY))
        {
            int dd = hci_open_dev(di.dev_id);
            hci_read_bd_addr(dd, &di.bdaddr, 1000);
            hci_close_dev(dd);
        }
        odebug << "OBluetooth::synchronize() - found device #" << di.dev_id << oendl;
        _interfaces.insert( di.name, new OBluetoothInterface( this, di.name, (void*) &di ) );
    }
}

/*======================================================================================
 * OBluetoothInterface
 *======================================================================================*/

class OBluetoothInterface::Private
{
  public:
    Private( struct hci_dev_info* di )
    {
        ::memcpy( &devinfo, di, sizeof(struct hci_dev_info) );
    }
    struct hci_dev_info devinfo;
};

OBluetoothInterface::OBluetoothInterface( QObject* parent, const char* name, void* devinfo )
{
    d = new OBluetoothInterface::Private( (struct hci_dev_info*) devinfo );
}

OBluetoothInterface::~OBluetoothInterface()
{
}

QString OBluetoothInterface::macAddress() const
{
    char addr[18];
    ba2str( &d->devinfo.bdaddr, addr);
    return addr;
}

}
}

