/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "tabssettings.h"

#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/applnk.h>
#include <qpe/mimetype.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/config.h>

#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qwhatsthis.h>

#include "tabdialog.h"

#include <stdlib.h>
#include <qmessagebox.h>



TabsSettings::TabsSettings ( QWidget *parent, const char *name )
	: QWidget ( parent, name )
{
	QGridLayout *lay = new QGridLayout ( this, 0, 0, 4, 4 );

	QLabel *l = new QLabel ( tr( "Launcher Tabs:" ), this );
	lay-> addMultiCellWidget ( l, 0, 0, 0, 1 );

	m_list = new QListBox ( this );
	lay-> addMultiCellWidget ( m_list, 1, 4, 0, 0 );

	QWhatsThis::add ( m_list, tr( "foobar" ));
	
	QPushButton *p;
	p = new QPushButton ( tr( "New" ), this );
	lay-> addWidget ( p, 1, 1 );
	connect ( p, SIGNAL( clicked ( )), this, SLOT( newClicked ( )));

	p = new QPushButton ( tr( "Edit" ), this );
	lay-> addWidget ( p, 2, 1 );
	connect ( p, SIGNAL( clicked ( )), this, SLOT( editClicked ( )));

	p = new QPushButton ( tr( "Delete" ), this );
	lay-> addWidget ( p, 3, 1 );
	connect ( p, SIGNAL( clicked ( )), this, SLOT( deleteClicked ( )));

	lay-> setRowStretch ( 4, 10 );

	init ( );
}

void TabsSettings::init ( )
{
	AppLnkSet rootFolder( MimeType::appsFolderName ( ));
	QStringList types = rootFolder. types ( );
	
	for ( QStringList::Iterator it = types. begin ( ); it != types. end ( ); ++it ) {
		m_list-> insertItem ( rootFolder. typePixmap ( *it ), rootFolder. typeName ( *it ));
		m_ids << *it;
	}
	QImage img ( Resource::loadImage ( "DocsIcon" ));
	QPixmap pix;
	pix = img. smoothScale ( AppLnk::smallIconSize ( ), AppLnk::smallIconSize ( ));
	m_list-> insertItem ( pix, tr( "Documents" ));
	m_ids += "Documents"; // No tr

	readTabSettings ( );
}

void TabsSettings::readTabSettings ( )
{
	Config cfg ( "Launcher" );
	QString grp ( "Tab %1" ); // No tr
	m_tabs. clear ( );

	for ( QStringList::Iterator it = m_ids. begin ( ); it != m_ids. end ( ); ++it ) {
		TabConfig tc;
		tc. m_view    = TabConfig::Icon;
		tc. m_bg_type = TabConfig::Ruled;
		tc. m_changed = false;

		cfg. setGroup ( grp. arg ( *it ));

		QString view = cfg. readEntry ( "View", "Icon" );
		if ( view == "List" ) // No tr
			tc. m_view = TabConfig::List;

		QString bgType = cfg. readEntry ( "BackgroundType", "Ruled" );
		if ( bgType == "SolidColor" )
			tc. m_bg_type = TabConfig::SolidColor;
		else if ( bgType == "Image" ) // No tr
			tc. m_bg_type = TabConfig::Image;

		tc. m_bg_image = cfg. readEntry ( "BackgroundImage", "wallpaper/opie" );
		tc. m_bg_color = cfg. readEntry ( "BackgroundColor" );
		tc. m_text_color = cfg. readEntry ( "TextColor" );
		QStringList f = cfg. readListEntry ( "Font", ',' );
		if ( f. count ( ) == 4 ) {
			tc. m_font_family = f [0];
			tc. m_font_size = f [1]. toInt ( );
		} else {
			tc. m_font_family = font ( ). family ( );
			tc. m_font_size = font ( ). pointSize ( );
		}
		m_tabs [*it] = tc;
	}
}


void TabsSettings::accept ( )
{
	Config cfg ( "Launcher" );

	// Launcher Tab
	QString grp ( "Tab %1" ); // No tr

	for ( QStringList::Iterator it = m_ids. begin ( ); it != m_ids. end ( ); ++it ) {
		TabConfig &tc = m_tabs [*it];

		cfg. setGroup ( grp. arg ( *it ));
		if ( !tc. m_changed )
			continue;
		switch ( tc. m_view ) {
			case TabConfig::Icon:
				cfg.writeEntry ( "View", "Icon" );
				break;
			case TabConfig::List:
				cfg.writeEntry ( "View", "List" );
				break;
		}

		QCopEnvelope e ( "QPE/Launcher", "setTabView(QString,int)" );
		e << *it << tc. m_view;

		cfg. writeEntry ( "BackgroundImage", tc. m_bg_image );
		cfg. writeEntry ( "BackgroundColor", tc. m_bg_color );
		cfg. writeEntry ( "TextColor", tc. m_text_color );

		QString f = tc. m_font_family + "," + QString::number ( tc. m_font_size ) + ",50,0";
		cfg. writeEntry ( "Font", f );
		QCopEnvelope be ( "QPE/Launcher", "setTabBackground(QString,int,QString)" );

		switch ( tc. m_bg_type ) {
			case TabConfig::Ruled:
				cfg.writeEntry( "BackgroundType", "Ruled" );
				be << *it << tc. m_bg_type << QString("");
				break;
			case TabConfig::SolidColor:
				cfg.writeEntry( "BackgroundType", "SolidColor" );
				be << *it << tc. m_bg_type << tc. m_bg_color;
				break;
			case TabConfig::Image:
				cfg.writeEntry( "BackgroundType", "Image" );
				be << *it << tc. m_bg_type << tc. m_bg_image;
				break;
		}

		QCopEnvelope te( "QPE/Launcher", "setTextColor(QString,QString)" );
		te << *it << tc. m_text_color;

		QCopEnvelope fe ( "QPE/Launcher", "setFont(QString,QString,int,int,int)" );
		fe << *it << tc. m_font_family;
		fe << tc. m_font_size;
		fe << 50 << 0;

		tc. m_changed = false;
	}
}

void TabsSettings::newClicked ( )
{
	QMessageBox::information ( this, tr( "Error" ), tr( "Not implemented yet" ));
}

void TabsSettings::deleteClicked ( )
{
	int ind = m_list-> currentItem ( );
	
	if ( ind < 0 )
		return;

	QMessageBox::information ( this, tr( "Error" ), tr( "Not implemented yet" ));
}

void TabsSettings::editClicked ( )
{
	int ind = m_list-> currentItem ( );
	
	if ( ind < 0 )
		return;

	TabConfig tc = m_tabs [m_ids [ind]];
		
	TabDialog *d = new TabDialog ( m_list-> pixmap ( ind ), m_list-> text ( ind ), tc, this, "TabDialog", true );

	d-> showMaximized ( );	
	if ( d-> exec ( ) == QDialog::Accepted ) {
		tc. m_changed = true;
		m_tabs [m_ids [ind]] = tc;
	}
	
	delete d;
}
