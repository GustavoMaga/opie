/**********************************************************************
** Copyright (C) 2002-2004 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Wellenreiter II.
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

#include "packetview.h"

/* OPIE */
#include <opie2/opcap.h>
#include <opie2/odebug.h>
#include <opie2/olistview.h>
#include <opie2/oapplication.h>

/* QT */
#include <qfont.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlist.h>
#include <qlistview.h>
#include <qobjectlist.h>
#include <qspinbox.h>
#include <qtextview.h>

using namespace Opie::Net;
using namespace Opie::Core;
using namespace Opie::Ui;

PacketView::PacketView( QWidget * parent, const char * name, WFlags f )
           :QFrame( parent, name, f )
{
    _number = new QSpinBox( this );
    _number->setPrefix( "Pkt# " );
    _label = new QLabel( this );
    _list = new OListView( this );
    _list->addColumn( "#" );
    _list->addColumn( "Packet Type" );
    _list->setColumnAlignment( 0, Qt::AlignCenter );
    _list->setColumnAlignment( 1, Qt::AlignLeft );
    _list->setAllColumnsShowFocus( true );
    _list->setFont( QFont( "Fixed", 8 ) );

    _hex = new QTextView( this );
    _hex->setMargin( 0 );
    _hex->setFont( QFont( "Fixed", 8 ) );

    QVBoxLayout* vb = new QVBoxLayout( this, 2, 2 );
    QHBoxLayout* hb = new QHBoxLayout( vb, 2 );
    hb->addWidget( _label, 5 );
    hb->addWidget( _number, 2 );
    vb->addWidget( _list, 3 );
    vb->addWidget( _hex, 4 ); // allow a bit (4/3) more space

    _packets.setAutoDelete( true );

    connect( _number, SIGNAL( valueChanged( int ) ), this, SLOT( showPacket( int ) ) );
    connect( parent, SIGNAL( currentChanged( QWidget *) ), this, SLOT( activated( QWidget* ) ) );

    clear();

}

void PacketView::add( const OPacket* p, int size )
{
    /*
     * don't scroll away when somebody views packages
     * while scanning
     */
    int  value = _number->value();
    bool last  = (value == static_cast<int>( _packets.count() ) );

    odebug << "PacketView::add() size = " << size << oendl;
    if ( size == -1 ) // unlimited buffer
    {
        _packets.append( p );
    }
    else
    {   // limited buffer, limit = size
        while ( _packets.count() >= static_cast<uint>( size ) )
        {
            _packets.removeFirst();
            --value;
        }

        /* check if we lost our packet */
        last = ( value < 1 );
        _packets.append( p );
    }

    _number->setMinValue( 1 );
    _number->setMaxValue( _packets.count() );
    _number->setValue( last ? _packets.count() : value );
}

void PacketView::showPacket( int number )
{
    _list->clear();
    _hex->setText("");
    const OPacket* p = _packets.at( number-1 );

    if ( p )
    {
        _doSubPackets( const_cast<QObjectList*>( p->children() ), 0 );
        _doHexPacket( p );
        QDateTime dt; dt.setTime_t( p->timeval().tv_sec );
        _label->setText( dt.toString() + QString().sprintf( " Len=%d", p->len() ) );
    }
    else
    {
        odebug << "D'oh! No packet!" << oendl;
    }
}

void PacketView::activated( QWidget* w )
{
    if ( ( this == w ) && _packets.count() )
    {
        _number->setValue( 1 );
    }
}

void PacketView::_doSubPackets( QObjectList* l, int counter )
{
    if (!l) return;
    QObject* o = l->first();
    while ( o )
    {
        new OListViewItem( _list, QString::number( counter++ ), o->name() );
        _doSubPackets( const_cast<QObjectList*>( o->children() ), counter );
        o = l->next();
    }
}

void PacketView::_doHexPacket( const OPacket* p )
{
    if ( oApp->desktop()->width() > 320 )
        _hex->setText( p->dump( 16 ) );
    else
        _hex->setText( p->dump( 8 ) );
}

const QString PacketView::getLog() const
{
    return QString::null;
}

void PacketView::clear()
{
    _packets.clear();
    _number->setMinValue( 0 );
    _number->setMaxValue( 0 );
    _label->setText( "---" );
    _list->clear();
    _hex->setText( " <center><i>-- no Packet available --</i></center> " );
}

