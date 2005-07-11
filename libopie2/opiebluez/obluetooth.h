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
�- . � .-<_> � � .<>         Foundation; version 2 of the License.
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

#ifndef OBLUETOOTH_H
#define OBLUETOOTH_H

#include <qobject.h>
#include <qdict.h>

namespace Opie {
namespace Bluez {

class OBluetoothInterface;

/**
 * @brief A container class for all bluetooth interfaces
 *
 * This class provides access to all available bluetooth interfaces of your computer.
 *
 * @author Michael 'Mickey' Lauer <mickey@vanille.de>
 */
class OBluetooth : public QObject
{
  Q_OBJECT

  public:
    typedef QDict<OBluetoothInterface> InterfaceMap;
    typedef QDictIterator<OBluetoothInterface> InterfaceIterator;

  public:
    /**
     * @returns the number of available interfaces
     */
    int count() const;
    /**
     * @returns a pointer to the (one and only) @ref OBluetooth instance.
     */
    static OBluetooth* instance();
    /**
     * @returns an iterator usable for iterating through all network interfaces.
     */
    InterfaceIterator iterator() const;
    /**
     * @returns true, if the @a interface is present.
     */
    bool isPresent( const char* interface ) const;
    /**
     * @returns true, if the @a interface supports the wireless extension protocol.
     */
    bool isWirelessInterface( const char* interface ) const;
    /**
     * @returns a pointer to the @ref OBluetoothInterface object for the specified @a interface or 0, if not found.
     * @see OBluetoothInterface
     */
    OBluetoothInterface* interface( const QString& interface ) const;
    /**
     * @internal Rebuild the internal interface database
     * @note Sometimes it might be useful to call this from client code,
     * e.g. after issuing a cardctl insert
     */
    void synchronize();

  protected:
    OBluetooth();

  private:
    static OBluetooth* _instance;
    InterfaceMap _interfaces;
    class OBluetoothPrivate;
    OBluetoothPrivate *d;
    int _fd;
};

/*======================================================================================
 * OBluetoothInterface
 *======================================================================================*/

/**
 * @brief An bluetooth interface wrapper.
 *
 * This class provides a wrapper for an infrared interface. All the cumbersome details of
 * Linux ioctls are hidden under a convenient high-level interface.
 * @warning Most of the setting methods contained in this class require the appropriate
 * process permissions to work.
 *
 * @author Michael 'Mickey' Lauer <mickey@vanille.de>
 */
class OBluetoothInterface : public QObject
{
  public:
    /**
     * Constructor. Normally you don't create @ref OBluetoothInterface objects yourself,
     * but access them via @ref OBluetooth::interface().
     */
    OBluetoothInterface( QObject* parent, const char* name, void* devinfo );
    /**
     * Destructor.
     */
    virtual ~OBluetoothInterface();
    /**
     * @return the MAC address of the interfaces
     */
    QString macAddress() const;

  private:
    class Private;
    Private *d;
};



}
}
#endif

