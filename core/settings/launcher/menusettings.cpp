/*
� � � � � � � �              This file is part of the OPIE Project
               =.            Copyright (c)  2002 Trolltech AS <info@trolltech.com>
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

#include "menusettings.h"

#include <qpe/config.h>
#include <qpe/qlibrary.h>
#include <qpe/qpeapplication.h>
#include <qpe/menuappletinterface.h>
#include <qpe/qcopenvelope_qws.h>

#include <qdir.h>
#include <qlistview.h>
#include <qcheckbox.h>
#include <qheader.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qwhatsthis.h>

#include <stdlib.h>


MenuSettings::MenuSettings ( QWidget *parent, const char *name )
	: QWidget ( parent, name )
{
	m_applets_changed = false;

	QBoxLayout *lay = new QVBoxLayout ( this, 4, 4 );

	QLabel *l = new QLabel ( tr( "Load applets in O-Menu:" ), this );
	lay-> addWidget ( l );

	m_list = new QListView ( this );
	m_list-> addColumn ( "foobar" );
	m_list-> header ( )-> hide ( );

	lay-> addWidget ( m_list );

	m_menutabs = new QCheckBox ( tr( "Show Launcher tabs in O-Menu" ), this );
	lay-> addWidget ( m_menutabs );

	m_menusubpopup = new QCheckBox ( tr( "Show Applications in subpopups" ), this );
	lay-> addWidget ( m_menusubpopup );

	QWhatsThis::add ( m_list, tr( "Check the applets that you want to have included in the O-Menu." ));
	QWhatsThis::add ( m_menutabs, tr( "Adds the contents of the Launcher Tabs as menus in the O-Menu." ));

	connect ( m_list, SIGNAL( clicked ( QListViewItem * )), this, SLOT( appletChanged ( )));

	init ( );
}

void MenuSettings::init ( )
{
	Config cfg ( "StartMenu" );
	cfg. setGroup ( "Applets" );
	QStringList exclude = cfg. readListEntry ( "ExcludeApplets", ',' );

	QString path = QPEApplication::qpeDir ( ) + "/plugins/applets";
#ifdef Q_OS_MACX
	QStringList list = QDir ( path, "lib*.dylib" ). entryList ( );
#else
	QStringList list = QDir ( path, "lib*.so" ). entryList ( );
#endif /* Q_OS_MACX */

	for ( QStringList::Iterator it = list. begin ( ); it != list. end ( ); ++it ) {
		QString name;
		QPixmap icon;
		MenuAppletInterface *iface = 0;

		QLibrary *lib = new QLibrary ( path + "/" + *it );
		lib-> queryInterface ( IID_MenuApplet, (QUnknownInterface**) &iface );
        if ( iface ) {
			QString lang = getenv( "LANG" );
			QTranslator *trans = new QTranslator ( qApp );
			QString type = (*it). left ((*it). find ("."));
			QString tfn = QPEApplication::qpeDir ( ) + "/i18n/" + lang + "/" + type + ".qm";
			if ( trans-> load ( tfn ))
				qApp-> installTranslator ( trans );
			else
				delete trans;
			name = iface-> name ( );
			icon = iface-> icon ( ). pixmap ( QIconSet::Small, QIconSet::Normal );
			iface-> release ( );
			lib-> unload ( );

			QCheckListItem *item;
			item = new QCheckListItem ( m_list, name, QCheckListItem::CheckBox );
			if ( !icon. isNull ( ))
				item-> setPixmap ( 0, icon );
			item-> setOn ( exclude. find ( *it ) == exclude. end ( ));
			m_applets [*it] = item;
		} else {
			delete lib;
		}
	}

	cfg. setGroup ( "Menu" );
	m_menutabs-> setChecked ( cfg. readBoolEntry ( "LauncherTabs", true ));
                m_menusubpopup-> setChecked ( cfg. readBoolEntry ( "LauncherSubPopup", true ));
}

void MenuSettings::appletChanged()
{
	m_applets_changed = true;
}

void MenuSettings::accept ( )
{
	bool apps_changed = false;

	Config cfg ( "StartMenu" );
	cfg. setGroup ( "Applets" );
	if ( m_applets_changed ) {
		QStringList exclude;
		QMap <QString, QCheckListItem *>::Iterator it;
		for ( it = m_applets. begin ( ); it != m_applets. end ( ); ++it ) {
			if ( !(*it)-> isOn ( ))
				exclude << it. key ( );
		}
		cfg. writeEntry ( "ExcludeApplets", exclude, ',' );
	}
	cfg. writeEntry ( "SafeMode", false );

	cfg. setGroup ( "Menu" );

	if ( m_menutabs-> isChecked ( ) != cfg. readBoolEntry ( "LauncherTabs", true )) {
		apps_changed = true;
		cfg. writeEntry ( "LauncherTabs", m_menutabs-> isChecked ( ));
	}

                if ( m_menusubpopup-> isChecked ( ) != cfg. readBoolEntry ( "LauncherSubPopup", true )) {
		 apps_changed = true;
		cfg. writeEntry ( "LauncherSubPopup", m_menusubpopup-> isChecked ( ));
	}

	cfg. write ( );

	if ( m_applets_changed ) {
		QCopEnvelope ( "QPE/TaskBar", "reloadApplets()" );
		m_applets_changed = false;
	}
	if ( apps_changed ) {
                                 // currently use reloadApplets() since reloadApps is now used exclusive for server
                                 // to refresh the tabs. But what we want here is also a refresh of the startmenu entries
		QCopEnvelope ( "QPE/TaskBar", "reloadApps()" );
                                QCopEnvelope ( "QPE/TaskBar", "reloadApplets()" );
	}
}

