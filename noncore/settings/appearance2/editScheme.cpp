/*
� � � � � � � �              This file is part of the OPIE Project
               =.            Copyright (c)  2002 Dan Williams <williamsdr@acm.org>
� � � � � � �.=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
� � � � � �.>+-=
�_;:, � � .> � �:=|.         This file is free software; you can
.> <`_, � > �. � <=          redistribute it and/or modify it under
:`=1 )Y*s>-.-- � :           the terms of the GNU General Public
.="- .-=="i, � � .._         License as published by the Free Software
�- . � .-<_> � � .<>         Foundation; either version 2 of the License,
� � �._= =} � � � :          or (at your option) any later version.
� � .%`+i> � � � _;_.
� � .i_,=:_. � � �-<s.       This file is distributed in the hope that
� � �+ �. �-:. � � � =       it will be useful, but WITHOUT ANY WARRANTY;
� � : .. � �.:, � � . . .    without even the implied warranty of
� � =_ � � � �+ � � =;=|`    MERCHANTABILITY or FITNESS FOR A
� _.=:. � � � : � �:=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.= � � � = � � � ;      Public License for more details.
++= � -. � � .` � � .:
�: � � = �...= . :.=-        You should have received a copy of the GNU
�-. � .:....=;==+<;          General Public License along with this file;
� -_. . . � )=. �=           see the file COPYING. If not, write to the
� � -- � � � �:-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "editScheme.h"

#include <opie/ocolorbutton.h>

#include <qaction.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpopupmenu.h>
#include <qscrollview.h>
#include <qtoolbutton.h>
#include <qwhatsthis.h>

EditScheme::EditScheme ( int cnt, const QString *labels, QColor *colors, QWidget* parent,  const char* name, bool modal, WFlags )
	: QDialog ( parent, name, modal, WStyle_ContextHelp )
{
	setCaption ( tr( "Edit scheme" ) );
	QGridLayout *layout = new QGridLayout ( this, 0, 0, 4, 4 );

    m_count = cnt;
    m_buttons = new OColorButton * [cnt];
    m_colors = colors;

    for ( int i = 0; i < cnt; i++ )
    {
        QLabel *l = new QLabel ( labels [i], this );
        layout-> addWidget ( l, i, 0 );
		QWhatsThis::add( l, tr( "Click here to select a color for: " ).arg( labels [i] ) );

        m_buttons [i] = new OColorButton ( this, colors [i] );
        layout-> addWidget ( m_buttons [i], i, 1 );
		QWhatsThis::add( m_buttons [i], tr( "Click here to select a color for: " ).arg( labels [i] ) );
    }
}

EditScheme::~EditScheme ( )
{
	delete [] m_buttons;
}

void EditScheme::accept ( )
{
	for ( int i = 0; i < m_count; i++ ) 
		m_colors [i] = m_buttons [i]-> color ( );
		
	QDialog::accept ( );
}

