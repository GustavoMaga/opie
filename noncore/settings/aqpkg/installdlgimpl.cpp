/***************************************************************************
                          installdlgimpl.cpp  -  description
                             -------------------
    begin                : Mon Aug 26 2002
    copyright            : (C) 2002 by Andy Qua
    email                : andy.qua@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
 
#ifdef QWS
#include <qpe/config.h>
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/storage.h>
#endif

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qgroupbox.h>
#include <qmultilineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>

#include "datamgr.h"
#include "destination.h"
#include "instoptionsimpl.h"
#include "installdlgimpl.h"
#include "ipkg.h"
#include "utils.h"
#include "global.h"

enum {
	MAXLINES = 100,
};

InstallDlgImpl::InstallDlgImpl( QList<InstallData> &packageList, DataManager *dataManager, const char *title )
    : QWidget( 0, 0, 0 )
{
    setCaption( title );
    init( TRUE );
    
    pIpkg = 0;
    upgradePackages = false;
    dataMgr = dataManager;

    QString defaultDest = "root";
#ifdef QWS
    Config cfg( "aqpkg" );
    cfg.setGroup( "settings" );
    defaultDest = cfg.readEntry( "dest", "root" );

    // Grab flags - Turn MAKE_LINKS on by default (if no flags found)
    flags = cfg.readNumEntry( "installFlags", 0 );
#else
	flags = 0;
#endif

    // Output text is read only
    output->setReadOnly( true );
//	QFont f( "helvetica" );
//	f.setPointSize( 10 );
//	output->setFont( f );


    // setup destination data
    int defIndex = 0;
    int i;
    QListIterator<Destination> dit( dataMgr->getDestinationList() );
    for ( i = 0; dit.current(); ++dit, ++i )
    {
        destination->insertItem( dit.current()->getDestinationName() );
        if ( dit.current()->getDestinationName() == defaultDest )
            defIndex = i;
    }

    destination->setCurrentItem( defIndex );

	QListIterator<InstallData> it( packageList );
	// setup package data
	QString remove = tr( "Remove\n" );
	QString install = tr( "Install\n" );
	QString upgrade = tr( "Upgrade\n" );
	for ( ; it.current(); ++it )
	{
		InstallData *item = it.current();
		InstallData *newitem = new InstallData();
        
        newitem->option = item->option;
        newitem->packageName = item->packageName;
        newitem->destination = item->destination;
        newitem->recreateLinks = item->recreateLinks;
        
        if ( item->option == "I" )
		{
			installList.append( newitem );
			install.append( QString( "   %1\n" ).arg( item->packageName ) );
		}
		else if ( item->option == "D" )
		{
			removeList.append( newitem );
			remove.append( QString( "   %1\n" ).arg( item->packageName ) );
		}
		else if ( item->option == "U" || item->option == "R" )
		{
            updateList.append( newitem );
            QString type;
            if ( item->option == "R" )
                type = tr( "(ReInstall)" );
            else
                type = tr( "(Upgrade)" );
            upgrade.append( QString( "   %1 %2\n" ).arg( item->packageName ).arg( type ) );
        }
    }
    output->setText( QString( "%1\n%2\n%3\n" ).arg( remove ).arg( install ).arg( upgrade ) );

    displayAvailableSpace( destination->currentText() );
}

InstallDlgImpl::InstallDlgImpl( Ipkg *ipkg, QString initialText, const char *title )
    : QWidget( 0, 0, 0 )
{
    setCaption( title );
    init( FALSE );
    pIpkg = ipkg;
    output->setText( initialText );
}


InstallDlgImpl::~InstallDlgImpl()
{
    if ( pIpkg )
        delete pIpkg;
}

void InstallDlgImpl :: init( bool displayextrainfo )
{
    QGridLayout *layout = new QGridLayout( this ); 
    layout->setSpacing( 4 );
    layout->setMargin( 4 );

    if ( displayextrainfo )
    {
        QLabel *label = new QLabel( tr( "Destination" ), this );
        layout->addWidget( label, 0, 0 );
        destination = new QComboBox( FALSE, this );
        layout->addWidget( destination, 0, 1 );
        connect( destination, SIGNAL( highlighted( const QString & ) ),
                this, SLOT( displayAvailableSpace( const QString & ) ) );

        QLabel *label2 = new QLabel( tr( "Space Avail" ), this );
        layout->addWidget( label2, 1, 0 );
        txtAvailableSpace = new QLabel( "", this );
        layout->addWidget( txtAvailableSpace, 1, 1 );
    }
    else
    {
        destination = 0x0;
        txtAvailableSpace = 0x0;
    }
    
    QGroupBox *GroupBox2 = new QGroupBox( 0, Qt::Vertical, tr( "Output" ), this );
    GroupBox2->layout()->setSpacing( 0 );
    GroupBox2->layout()->setMargin( 4 );

    QVBoxLayout *GroupBox2Layout = new QVBoxLayout( GroupBox2->layout() );
    output = new QMultiLineEdit( GroupBox2 );
    GroupBox2Layout->addWidget( output );
    layout->addMultiCellWidget( GroupBox2, 2, 2, 0, 1 );

    btnInstall = new QPushButton( Resource::loadPixmap( "aqpkg/apply" ), tr( "Start" ), this );
    layout->addWidget( btnInstall, 3, 0 );
    connect( btnInstall, SIGNAL( clicked() ), this, SLOT( installSelected() ) );
    
    btnOptions = new QPushButton( Resource::loadPixmap( "aqpkg/config" ), tr( "Options" ), this );
    layout->addWidget( btnOptions, 3, 1 );
    connect( btnOptions, SIGNAL( clicked() ), this, SLOT( optionsSelected() ) );
}

void InstallDlgImpl :: optionsSelected()
{
    InstallOptionsDlgImpl opt( flags, this, "Option", true );
    opt.exec();

    // set options selected from dialog
    flags = opt.getFlags();

#ifdef QWS
    Config cfg( "aqpkg" );
    cfg.setGroup( "settings" );
    cfg.writeEntry( "installFlags", flags );
#endif
}

void InstallDlgImpl :: installSelected()
{
    if ( btnInstall->text() == tr( "Abort" ) )
    {
        if ( pIpkg )
        {
            displayText( tr( "\n**** User Clicked ABORT ***" ) );
            pIpkg->abort();
            displayText( tr( "**** Process Aborted ****" ) );
        }
        
        btnInstall->setText( tr( "Close" ) );
        btnInstall->setIconSet( Resource::loadPixmap( "enter" ) );
        return;
    }
    else if ( btnInstall->text() == tr( "Close" ) )
    {
        emit reloadData( this );
        return;
    }
    
    // Disable buttons
    btnOptions->setEnabled( false );
//    btnInstall->setEnabled( false );

    btnInstall->setText( tr( "Abort" ) );
    btnInstall->setIconSet( Resource::loadPixmap( "close" ) );
    if ( pIpkg )
    {
        output->setText( "" );
        connect( pIpkg, SIGNAL(outputText(const QString &)), this, SLOT(displayText(const QString &)));
        pIpkg->runIpkg();
    }
    else
    {
        output->setText( "" );
        Destination *d = dataMgr->getDestination( destination->currentText() );
        QString dest = d->getDestinationName();
        QString destDir = d->getDestinationPath();
        int instFlags = flags;
        if ( d->linkToRoot() )
            instFlags |= MAKE_LINKS;
    
#ifdef QWS
        // Save settings
        Config cfg( "aqpkg" );
        cfg.setGroup( "settings" );
        cfg.writeEntry( "dest", dest );
#endif

        pIpkg = new Ipkg;
        connect( pIpkg, SIGNAL(outputText(const QString &)), this, SLOT(displayText(const QString &)));

        // First run through the remove list, then the install list then the upgrade list
        pIpkg->setOption( "remove" );
        QListIterator<InstallData> it( removeList );
        InstallData *idata;
        for ( ; it.current(); ++it )
        {
            idata = it.current();
            pIpkg->setDestination( idata->destination->getDestinationName() );
            pIpkg->setDestinationDir( idata->destination->getDestinationPath() );
            pIpkg->setPackage( idata->packageName );

            int tmpFlags = flags;
            if ( idata->destination->linkToRoot() )
                tmpFlags |= MAKE_LINKS;
            
            pIpkg->setFlags( tmpFlags );
            pIpkg->runIpkg();
        }

        pIpkg->setOption( "install" );
        pIpkg->setDestination( dest );
        pIpkg->setDestinationDir( destDir );
        pIpkg->setFlags( instFlags );
        QListIterator<InstallData> it2( installList );
        for ( ; it2.current(); ++it2 )
        {
            pIpkg->setPackage( it2.current()->packageName );
            pIpkg->runIpkg();
        }

        flags |= FORCE_REINSTALL;
        QListIterator<InstallData> it3( updateList );
        for ( ; it3.current() ; ++it3 )
        {
            idata = it3.current();
            if ( idata->option == "R" )
                pIpkg->setOption( "reinstall" );
            else
                pIpkg->setOption( "upgrade" );
            pIpkg->setDestination( idata->destination->getDestinationName() );
            pIpkg->setDestinationDir( idata->destination->getDestinationPath() );
            pIpkg->setPackage( idata->packageName );

            int tmpFlags = flags;
            if ( idata->destination->linkToRoot() && idata->recreateLinks )
                tmpFlags |= MAKE_LINKS;
            pIpkg->setFlags( tmpFlags );
            pIpkg->runIpkg();
        }

        delete pIpkg;
        pIpkg = 0;
    }

    btnOptions->setEnabled( true );
//    btnInstall->setEnabled( true );
    btnInstall->setText( tr( "Close" ) );
    btnInstall->setIconSet( Resource::loadPixmap( "enter" ) );

    if ( destination && destination->currentText() != 0 && destination->currentText() != "" )
        displayAvailableSpace( destination->currentText() );
}


void InstallDlgImpl :: displayText(const QString &text )
{
    QString newtext = QString( "%1\n%2" ).arg( output->text() ).arg( text );

    /* Set a max line count for the QMultiLineEdit, as users have reported
     * performance issues when line count gets extreme.
     */
    if(output->numLines() >= MAXLINES)
        output->removeLine(0);
    output->setText( newtext );
    output->setCursorPosition( output->numLines(), 0 );
}


void InstallDlgImpl :: displayAvailableSpace( const QString &text )
{
    Destination *d = dataMgr->getDestination( text );
    QString destDir = d->getDestinationPath();

    long blockSize = 0;
    long totalBlocks = 0;
    long availBlocks = 0;
    QString space;
    if ( Utils::getStorageSpace( (const char *)destDir, &blockSize, &totalBlocks, &availBlocks ) )
    {
        long mult = blockSize / 1024;
        long div = 1024 / blockSize;

        if ( !mult ) mult = 1;
        if ( !div ) div = 1;
//        long total = totalBlocks * mult / div;
        long avail = availBlocks * mult / div;
//        long used = total - avail;

        space.sprintf( "%ld Kb", avail );
    }
    else
        space = tr( "Unknown" );
        
    if ( txtAvailableSpace )
        txtAvailableSpace->setText( space );
}

