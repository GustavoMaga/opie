/***************************************************************************
   application:             : ODict

   begin                    : December 2002
   copyright                : ( C ) 2002, 2003 by Carsten Niehaus
   email                    : cniehaus@handhelds.org
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/
#include "odict.h"
#include "configdlg.h"
#include "dingwidget.h"

#include <qlayout.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpe/config.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qmainwindow.h>
#include <qstring.h>
#include <qaction.h>
#include <qtextbrowser.h>
#include <qcombobox.h>

#include <qpe/resource.h>


ODict::ODict() : QMainWindow()
{
	activated_name = QString::null;
	
	vbox = new QVBox( this );
	setCaption( tr( "OPIE-Dictionary" ) );
	setupMenus();

	QHBox *hbox = new QHBox( vbox );
	QLabel* query_label = new QLabel( tr( "Query:" ) , hbox ); query_label->show();
	query_le = new QLineEdit( hbox );
	query_co = new QComboBox( hbox );
	connect( query_co , SIGNAL( activated(int) ), this, SLOT( slotMethodChanged(int) ) );
	ok_button = new QPushButton( tr( "&Ok" ), hbox );
	connect( ok_button, SIGNAL( released() ), this, SLOT( slotStartQuery() ) );
	browser_top = new QTextBrowser( vbox );
	browser_bottom = new QTextBrowser( vbox );

	ding = new DingWidget();
	ding->loadValues();

	loadConfig();
	setCentralWidget( vbox );
}

void ODict::loadConfig()
{
	Config cfg ( "odict" );
	cfg.setGroup( "generalsettings" );
	errorTol = cfg.readEntry( "errtol" ).toInt();
	casesens = cfg.readEntry( "casesens" ).toInt();
	regexp = cfg.readEntry( "regexp" ).toInt();
	completewords = cfg.readEntry( "completewords" ).toInt();

	QStringList groupListCfg = cfg.groupList().grep( "Method_" );
	query_co->clear();
	for ( QStringList::Iterator it = groupListCfg.begin() ; it != groupListCfg.end() ; ++it )
	{
		cfg.setGroup( *it );
		query_co->insertItem( cfg.readEntry( "Name" ) );
	}
	slotMethodChanged(1 );           //FIXME: this line should not contain a integer
}


void ODict::saveConfig()
{
	Config cfg ( "odict" );
	cfg.setGroup( "generalsettings" );
	cfg.writeEntry( "errtol" , errorTol );
	cfg.writeEntry( "casesens" , casesens );
	cfg.writeEntry( "regexp" , regexp );
	cfg.writeEntry( "completewords" , completewords );
}

void ODict::slotStartQuery()
{
	QString querystring = query_le->text();
	ding->setCaseSensitive( casesens ); 
	ding->setCompleteWord( completewords ); 
	ding->setDict( activated_name );

	if ( activated_name != ding->loadedDict() )
		ding->loadDict(activated_name);

	BroswerContent test = ding->setText( querystring );

	browser_top->setText( test.top );
	browser_bottom->setText( test.bottom );
}


void ODict::slotSetErrorcount( int count )
{
	errorTol = count;
}

void ODict::slotSettings()
{
	ConfigDlg dlg( this, "Config" , true);
	if ( dlg.exec() == QDialog::Accepted )
	{
		dlg.writeEntries();
		loadConfig();
	}
	else qDebug( "abgebrochen" );
}

void ODict::slotSetParameter( int count )
{
 	if ( count == 0 )
	{
		if ( casesens )
			casesens = false;
		else
			casesens = true;
	}

 	if ( count == 1 )
	{
		if ( completewords )
			completewords = false;
		else
			completewords = true;
	}
 	if ( count == 2 )
	{
		if ( regexp )
			regexp = false;
		else
			regexp = true;
	}
 	else qWarning( "ERROR" );
}

void ODict::slotMethodChanged( int /*methodnumber*/ )
{
	activated_name = query_co->currentText();
}

void ODict::setupMenus()
{
	menu = new QMenuBar( this );
	
	settings = new QPopupMenu( menu );
	setting_a = new QAction(tr( "Configuration" ), Resource::loadPixmap( "new" ), QString::null, 0, this, 0 );
	connect( setting_a, SIGNAL( activated() ), this, SLOT( slotSettings() ) );
	setting_a->addTo( settings );
	setting_b = new QAction(tr(  "Searchmethods" ), Resource::loadPixmap( "edit" ), QString::null, 0, this, 0 );
	
	parameter = new QPopupMenu( menu );
	connect(  parameter, SIGNAL( activated( int ) ), this, SLOT( slotSetParameter( int ) ) );
	parameter->insertItem( tr( "C&ase sensitive" ), 0 ,0 );
	parameter->insertItem( tr( "Only &complete Words" ), 1 , 1) ;
	parameter->insertItem( tr( "Allow &reg. expressions" ), 2 );
	parameter->insertSeparator();
		error_tol_menu = new QPopupMenu( menu );	
	    error_tol_menu->setCheckable(  TRUE );
	    connect(  error_tol_menu, SIGNAL( activated( int ) ), this, SLOT( slotSetErrorcount( int ) ) );
		  
		error_tol_menu->insertItem( tr( "0 Errors" ), 0 );
		error_tol_menu->insertItem( tr( "1 Errors" ), 1 );
		error_tol_menu->insertItem( tr( "2 Errors" ), 2 );
		error_tol_menu->insertItem( tr( "3 Errors" ), 3 );
		error_tol_menu->insertItem( tr( "4 Errors" ), 4 );
		error_tol_menu->insertItem( tr( "Until Hit" ), 5 );
	parameter->insertItem( tr( "&Error tolerance" ), error_tol_menu );
	
	menu->insertItem( tr( "Settings" ) , settings );
	menu->insertItem( tr( "Parameter" ) , parameter );
}
