/*
 � � � � � � � �             This file is part of the Opie Project

� � � � � � �                (C) 2003 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
              =.
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

#include <opie2/onetutils.h>
#include <opie2/onetwork.h>

#include <net/if.h>

#include <cstdio>
using namespace std;

#define IW_PRIV_TYPE_MASK       0x7000
#define IW_PRIV_TYPE_NONE       0x0000
#define IW_PRIV_TYPE_BYTE       0x1000
#define IW_PRIV_TYPE_CHAR       0x2000
#define IW_PRIV_TYPE_INT        0x4000
#define IW_PRIV_TYPE_FLOAT      0x5000
#define IW_PRIV_TYPE_ADDR       0x6000
#define IW_PRIV_SIZE_FIXED      0x0800
#define IW_PRIV_SIZE_MASK       0x07FF

/*======================================================================================
 * OMacAddress
 *======================================================================================*/

// static initializer for broadcast and unknown MAC Adresses
const unsigned char __broadcast[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
const OMacAddress& OMacAddress::broadcast = OMacAddress( __broadcast );
const unsigned char __unknown[6] = { 0x44, 0x44, 0x44, 0x44, 0x44, 0x44 };
const OMacAddress& OMacAddress::unknown = OMacAddress( __unknown );


//TODO: Incorporate Ethernet Manufacturer database here!

OMacAddress::OMacAddress( unsigned char* p )
{
    memcpy( _bytes, p, 6 );
}


OMacAddress::OMacAddress( const unsigned char* p )
{
    memcpy( _bytes, p, 6 );
}


OMacAddress::OMacAddress( struct ifreq& ifr )
{
    memcpy( _bytes, ifr.ifr_hwaddr.sa_data, 6 );
}


OMacAddress::~OMacAddress()
{
}


//#ifdef QT_NO_DEBUG
//inline
//#endif
const unsigned char* OMacAddress::native() const
{
    return (const unsigned char*) &_bytes;
}


OMacAddress OMacAddress::fromString( const QString& str )
{
    QString addr( str );
    unsigned char buf[6];
    bool ok = true;
    int index = 14;
    for ( int i = 5; i >= 0; --i )
    {
        buf[i] = addr.right( 2 ).toUShort( &ok, 16 );
        if ( !ok ) return OMacAddress::unknown;
        addr.truncate( index );
        index -= 3;
    }
    return (const unsigned char*) &buf;
}


QString OMacAddress::toString() const
{
    QString s;
    s.sprintf( "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
      _bytes[0]&0xff, _bytes[1]&0xff, _bytes[2]&0xff,
      _bytes[3]&0xff, _bytes[4]&0xff, _bytes[5]&0xff );
    return s;
}


bool operator==( const OMacAddress &m1, const OMacAddress &m2 )
{
    return memcmp( &m1._bytes, &m2._bytes, 6 ) == 0;
}


/*======================================================================================
 * OHostAddress
 *======================================================================================*/


/*======================================================================================
 * OPrivateIOCTL
 *======================================================================================*/

OPrivateIOCTL::OPrivateIOCTL( QObject* parent, const char* name, int cmd, int getargs, int setargs )
              :QObject( parent, name ), _ioctl( cmd ), _getargs( getargs ), _setargs( setargs )
{
}


OPrivateIOCTL::~OPrivateIOCTL()
{
}


#ifdef QT_NO_DEBUG
inline
#endif
int OPrivateIOCTL::numberGetArgs() const
{
    return _getargs & IW_PRIV_SIZE_MASK;
}


#ifdef QT_NO_DEBUG
inline
#endif
int OPrivateIOCTL::typeGetArgs() const
{
    return _getargs & IW_PRIV_TYPE_MASK >> 12;
}


#ifdef QT_NO_DEBUG
inline
#endif
int OPrivateIOCTL::numberSetArgs() const
{
    return _setargs & IW_PRIV_SIZE_MASK;
}


#ifdef QT_NO_DEBUG
inline
#endif
int OPrivateIOCTL::typeSetArgs() const
{
    return _setargs & IW_PRIV_TYPE_MASK >> 12;
}


void OPrivateIOCTL::invoke() const
{
    ( (OWirelessNetworkInterface*) parent() )->wioctl( _ioctl );
}


void OPrivateIOCTL::setParameter( int num, u_int32_t value )
{
    u_int32_t* arglist = (u_int32_t*) &( (OWirelessNetworkInterface*) parent() )->_iwr.u.name;
    arglist[num] = value;
}

/*======================================================================================
 * assorted functions
 *======================================================================================*/

void dumpBytes( const unsigned char* data, int num )
{
    printf( "Dumping %d bytes @ %0x", num, data );
    printf( "-------------------------------------------\n" );

    for ( int i = 0; i < num; ++i )
    {
        printf( "%02x ", data[i] );
        if ( !((i+1) % 32) ) printf( "\n" );
    }
    printf( "\n\n" );
}

