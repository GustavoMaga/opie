/*
� � � � � � � �=.            This file is part of the OPIE Project
� � � � � � �.=l.            Copyright (c)  2003 Maximilian Reiss <harlekin@handhelds.org>
� � � � � �.>+-=
�_;:, � � .> � �:=|.         This library is free software; you can
.> <, � > �. � <=          redistribute it and/or  modify it under
:=1 )Y*s>-.-- � :           the terms of the GNU Library General Public
.="- .-=="i, � � .._         License as published by the Free Software
�- . � .-<_> � � .<>         Foundation; either version 2 of the License,
� � �._= =} � � � :          or (at your option) any later version.
� � .%+i> � � � _;_.
� � .i_,=:_. � � �-<s.       This library is distributed in the hope that
� � �+ �. �-:. � � � =       it will be useful,  but WITHOUT ANY WARRANTY;
� � : .. � �.:, � � . . .    without even the implied warranty of
� � =_ � � � �+ � � =;=|    MERCHANTABILITY or FITNESS FOR A
� _.=:. � � � : � �:=>:     PARTICULAR PURPOSE. See the GNU
..}^=.= � � � = � � � ;      Library General Public License for more
++= � -. � � . � � .:       details.
�: � � = �...= . :.=-
�-. � .:....=;==+<;          You should have received a copy of the GNU
� -_. . . � )=. �=           Library General Public License along with
� � -- � � � �:-=           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/


#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>

#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qiconset.h>
#include <qpopupmenu.h>

#include "rotate.h"


RotateApplet::RotateApplet ( )
	: QObject ( 0, "RotateApplet" ), ref ( 0 ), m_flipped( false )
{
}

RotateApplet::~RotateApplet ( )
{
}

int RotateApplet::position ( ) const
{
    return 3;
}

QString RotateApplet::name ( ) const
{
	return tr( "Rotate shortcut" );
}

QString RotateApplet::text ( ) const
{
	return tr( "Rotate" );
}

QString RotateApplet::tr( const char* s ) const
{
    return qApp->translate( "RotateApplet", s, 0 );
}

QString RotateApplet::tr( const char* s, const char* p ) const
{
    return qApp->translate( "RotateApplet", s, p );
}

QIconSet RotateApplet::icon ( ) const
{
	QPixmap pix;
	QImage img = Resource::loadImage ( "Rotation" );

	if ( !img. isNull ( ))
		pix. convertFromImage ( img. smoothScale ( 14, 14 ));
	return pix;
}

QPopupMenu *RotateApplet::popup ( QWidget * ) const
{
	return 0;
}

void RotateApplet::activated ( )
{
    int defaultRotation = QPEApplication::defaultRotation();

    int newRotation;

    Config cfg( "qpe" );
    cfg.setGroup( "Appearance" );

    // 0 -> 90� clockwise, 1 -> 90� counterclockwise
    int rotDirection = cfg.readNumEntry( "rotatedir", 0 );

    // hide inputs methods before rotation
    QCopEnvelope en( "QPE/TaskBar", "hideInputMethod()" );

    if ( m_flipped )  {
	// if flipped, flip back to the original state,
	// regardless of rotation direction
        newRotation = defaultRotation;
    } else {
        if ( rotDirection == 1 )  {
            newRotation = ( defaultRotation + 90 ) % 360;
        } else if ( rotDirection == 0 ) {
            newRotation = ( defaultRotation + 270 ) % 360;
        } else {
            newRotation = ( defaultRotation + 180 ) % 360;
	}
    }

    QCopEnvelope env( "QPE/System", "setCurrentRotation(int)" );
    env << newRotation;

    m_flipped = !m_flipped;
}


QRESULT RotateApplet::queryInterface ( const QUuid &uuid, QUnknownInterface **iface )
{
	*iface = 0;
	if ( uuid == IID_QUnknown )
		*iface = this;
	else if ( uuid == IID_MenuApplet )
		*iface = this;

	if ( *iface )
		(*iface)-> addRef ( );
	return QS_OK;
}

Q_EXPORT_INTERFACE( )
{
	Q_CREATE_INSTANCE( RotateApplet )
}


