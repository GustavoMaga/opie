/*
� � � � � � � �=.            This file is part of the OPIE Project
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

#ifndef STYLELISTITEM_H
#define STYLELISTITEM_H

/* OPIE */
#include <qpe/styleinterface.h>
#include <qpe/qlibrary.h>
#include <qpe/resource.h>

/* QT */
#include <qlistbox.h>
#include <qfileinfo.h>

#include <stdio.h>

class StyleListItem : public QListBoxText {
public:
	StyleListItem ( const QString &t, QStyle *sty ) : QListBoxText ( t )
	{
		m_lib = 0;
		m_style_if = 0;
		m_settings_if = 0;
		m_style = sty;
	}

	StyleListItem ( QLibrary *lib, StyleInterface *iface ) : QListBoxText ( iface-> name ( ))
	{
		m_lib = lib;
		m_style_if = iface;
		m_settings_if = 0;
		m_style = iface-> style ( );
		
		iface-> queryInterface ( IID_StyleExtended, (QUnknownInterface **) &m_settings_if );
	}
	
	virtual ~StyleListItem ( )
	{
		delete m_style;
	
		if ( m_settings_if )
			m_settings_if-> release ( );
		if ( m_style_if )
			m_style_if-> release ( );
		delete m_lib;
	}
	
	bool hasSettings ( ) const
	{
		return m_settings_if ? m_settings_if-> hasSettings ( ) : false;
	}

	QWidget *settings ( QWidget *parent )
	{
		return m_settings_if ? m_settings_if-> create ( parent ) : 0;
	}
	
	bool setSettings ( bool accepted ) 
	{
		if ( !m_settings_if )
			return false;
	
		if ( accepted )
			return m_settings_if-> accept ( );
		else {
			m_settings_if-> reject ( );
			return false;
		}
	}

	QString key ( )
	{
		if ( m_lib )
			return QFileInfo ( m_lib-> library ( )). fileName ( );
		else
			return text ( );
	}
	
	QStyle *style ( ) 
	{
		return m_style;
	}

private:
	QLibrary *m_lib;
	QStyle *m_style;
	StyleInterface *m_style_if;
	StyleExtendedInterface *m_settings_if;

};


#endif
