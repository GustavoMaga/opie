/*
� � � � � � � �              This file is part of the OPIE Project
               =.
� � � � � � �.=l.            Copyright (c)  2002 Dan Williams <drw@handhelds.org>
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

#include "mainwindow.h"
#include "cbinfo.h"
#include "configuration.h"
#include "password.h"
#include "checkbook.h"
#include "listedit.h"

#include <qpe/config.h>
#include <qpe/global.h>
#include <qpe/qpeapplication.h>
#include <qpe/qpemessagebox.h>
#include <qpe/qpetoolbar.h>
#include <qpe/resource.h>

#include <qmenubar.h>
#include <qaction.h>
#include <qcheckbox.h>
#include <qdir.h>
#include <qlineedit.h>
#include <qwhatsthis.h>


MainWindow::MainWindow( QWidget* parent, const char* name, WFlags /*fl*/ )
    : QMainWindow( parent, name, WStyle_ContextHelp )
{
    setCaption( tr( "Checkbook" ) );

    cbDir = Global::applicationFileName( "checkbook", "" );
    lockIcon = Resource::loadPixmap( "locked" );

    // Load configuration options
    Config config( "checkbook" );
    _cfg.readConfig( config );


    // Build menu and tool bars
    setToolBarsMovable( FALSE );

    QToolBar *bar = new QToolBar( this );
    bar->setHorizontalStretchable( TRUE );
    QMenuBar *mb = new QMenuBar( bar );
    mb->setMargin( 0 );
    QPopupMenu *popup = new QPopupMenu( this );

    bar = new QToolBar( this );
    QAction *a = new QAction( tr( "New" ), Resource::loadPixmap( "new" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to create a new checkbook.\n\nYou also can select New from the Checkbook menu." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( slotNew() ) );
    a->addTo( popup );
    a->addTo( bar );

    actionOpen = new QAction( tr( "Edit" ), Resource::loadPixmap( "edit" ), QString::null,
                              0, this, 0 );
    actionOpen->setWhatsThis( tr( "Select a checkbook and then click here to edit it.\n\nYou also can select Edit from the Checkbook menu, or click and hold on a checkbook name." ) );
    connect( actionOpen, SIGNAL( activated() ), this, SLOT( slotEdit() ) );
    actionOpen->addTo( popup );
    actionOpen->addTo( bar );

    actionDelete = new QAction( tr( "Delete" ), Resource::loadPixmap( "trash" ), QString::null,
                                0, this, 0 );
    actionDelete->setWhatsThis( tr( "Select a checkbook and then click here delete it.\n\nYou also can select Delete from the Checkbook menu." ) );
    connect( actionDelete, SIGNAL( activated() ), this, SLOT( slotDelete() ) );
    actionDelete->addTo( popup );
    actionDelete->addTo( bar );

    popup->insertSeparator();

    a = new QAction( tr( "Configure" ), Resource::loadPixmap( "SettingsIcon" ), QString::null, 0, this, 0 );
    a->setWhatsThis( tr( "Click here to configure this app." ) );
    connect( a, SIGNAL( activated() ), this, SLOT( slotConfigure() ) );
    a->addTo( popup );
    a->addTo( bar );

    mb->insertItem( tr( "Checkbook" ), popup );

    // Load Checkbook selection list
    checkbooks = new CBInfoList();

    QDir checkdir( cbDir );
    if (checkdir.exists() == true)
    {
        QStringList cblist = checkdir.entryList( "*.qcb", QDir::Files|QDir::Readable|QDir::Writable,
                                                     QDir::Time );
        CBInfo *cb = 0x0;
        QString filename;

        for ( QStringList::Iterator it = cblist.begin(); it != cblist.end(); it++ )
        {
            filename = cbDir;
            filename.append( (*it) );

            cb = new CBInfo( (*it).remove( (*it).find('.'), (*it).length() ), filename );
            checkbooks->inSort( cb );
        }
    }

    // Build Checkbook selection list control
    cbList = 0x0;
    buildList();

    // open last book?
    if( _cfg.isOpenLastBook() ) {
        this->show();
        this->showMaximized();
        QListViewItem *itm=cbList->firstChild();
        while( itm ) {
            if( itm->text(posName)==_cfg.getLastBook() ) {
                openBook( itm );
                break;
            }
            itm=itm->nextSibling();
        }
    }
}


// --- ~MainWindow ------------------------------------------------------------
MainWindow::~MainWindow()
{
    writeConfig();
}


// --- buildList --------------------------------------------------------------
void MainWindow::buildList()
{
    if ( cbList )
            delete cbList;

    cbList = new QListView( this );
    QWhatsThis::add( cbList, tr( "This is a listing of all checkbooks currently available." ) );

    if ( _cfg.getShowLocks() )
    {
        cbList->addColumn( Resource::loadIconSet( "locked" ), "", 24 );
        posName = 1;
    }
    else
    {
        posName = 0;
    }
    cbList->addColumn( tr( "Checkbook Name" ) );
    if ( _cfg.getShowBalances() )
    {
        int colnum = cbList->addColumn( tr( "Balance" ) );
        cbList->setColumnAlignment( colnum, Qt::AlignRight );
    }
    cbList->setAllColumnsShowFocus( TRUE );
    cbList->setSorting( posName );
    QPEApplication::setStylusOperation( cbList->viewport(), QPEApplication::RightOnHold );
    connect( cbList, SIGNAL( rightButtonPressed( QListViewItem *, const QPoint &, int ) ),
             this, SLOT( slotEdit() ) );
    setCentralWidget( cbList );

    for ( CBInfo *cb = checkbooks->first(); cb; cb = checkbooks->next() )
    {
        addCheckbook( cb );
    }
}

void MainWindow::addCheckbook( CBInfo *cb )
{
    QListViewItem *lvi = new QListViewItem( cbList );
    if ( _cfg.getShowLocks() && !cb->password().isNull() )
    {
        lvi->setPixmap( 0, lockIcon );
    }
    lvi->setText( posName, cb->name() );
    if ( _cfg.getShowBalances() )
    {
        QString balance;
        balance.sprintf( "%s%.2f", _cfg.getCurrencySymbol().latin1(), cb->balance() );
        lvi->setText( posName + 1, balance );
    }
}

void MainWindow::buildFilename( const QString &name )
{
    tempFilename = cbDir;
    tempFilename.append( name );
    tempFilename.append( ".qcb" );
}

void MainWindow::slotNew()
{
    CBInfo *cb = new CBInfo();

    Checkbook *currcb = new Checkbook( this, cb, &_cfg );
    if ( QPEApplication::execDialog( currcb ) == QDialog::Accepted )
    {
        // Save new checkbook
        buildFilename( cb->name() );
        _cfg.setLastBook( cb->name() );
        cb->setFilename( tempFilename );
        cb->write();

        // Add to listbox
        checkbooks->inSort( cb );
        addCheckbook( cb );
    }
    delete currcb;
}

// --- slotEdit ---------------------------------------------------------------
void MainWindow::slotEdit()
{
    // get name and open it
    QListViewItem *curritem = cbList->currentItem();
    if ( !curritem )
        return;
    openBook( curritem );
}


// --- openBook ---------------------------------------------------------------
void MainWindow::openBook(QListViewItem *curritem)
{
    // find book in List
    QString currname=curritem->text(posName);
    CBInfo *cb = checkbooks->first();
    while ( cb ) {
        if ( cb->name() == currname )
            break;
        cb = checkbooks->next();
    }
    if ( !cb ) return;

    //
    buildFilename( currname );
    float currbalance = cb->balance();
    bool currlock = !cb->password().isNull();

    if ( currlock )
    {
        Password *pw = new Password( this, tr( "Enter password" ), tr( "Please enter your password:" ) );
        if ( pw->exec() != QDialog::Accepted || pw->password != cb->password()  )
        {
            delete pw;
            return;
        }
        delete pw;
    }

    _cfg.setLastBook( currname );
    Checkbook *currcb = new Checkbook( this, cb, &_cfg );
    if ( QPEApplication::execDialog( currcb ) == QDialog::Accepted )
    {
        QString newname = cb->name();
        if ( currname != newname )
        {
            // Update name if changed
            if( curritem ) {
                curritem->setText( posName, newname );
                cbList->sort();
            }
            _cfg.setLastBook( newname );

            // Remove old file
            QFile f( tempFilename );
            if ( f.exists() )
                f.remove();

            // Get new filename
            buildFilename( newname );
            cb->setFilename( tempFilename );
        }

        cb->write();

        // Update lock if changed
        if ( _cfg.getShowLocks() && !cb->password().isNull() != currlock )
        {
            if ( !cb->password().isNull() )
                curritem->setPixmap( 0, lockIcon );
            else
                curritem->setPixmap( 0, nullIcon );
        }

        // Update balance if changed
        if ( _cfg.getShowBalances() && cb->balance() != currbalance )
        {
            QString tempstr;
            tempstr.sprintf( "%s%.2f", _cfg.getCurrencySymbol().latin1(), cb->balance() );
            curritem->setText( posName + 1, tempstr );
        }

        // write config, if needed
        if( _cfg.isDirty() ) {
            Config config("checkbook");
            _cfg.writeConfig( config );
        }
    }
    delete currcb;
}

// --- slotDelete -------------------------------------------------------------
void MainWindow::slotDelete()
{
    QString currname = cbList->currentItem()->text( posName );

    if ( QPEMessageBox::confirmDelete ( this, tr( "Delete checkbook" ), currname ) )
    {
        buildFilename( currname );
        QFile f( tempFilename );
        if ( f.exists() )
        {
            f.remove();
        }

        delete cbList->currentItem();
    }
}

// --- slotConfigure ----------------------------------------------------------
void MainWindow::slotConfigure()
{
    Configuration *cfgdlg = new Configuration( this, _cfg );
    if ( QPEApplication::execDialog( cfgdlg ) == QDialog::Accepted )
    {
        // read data from config dialog & save it
        cfgdlg->saveConfig( _cfg );
        writeConfig();
        buildList();
    }
    delete cfgdlg;
}


// --- writeConfig --------------------------------------------------------------
void MainWindow::writeConfig()
{
    Config config("checkbook");
    _cfg.writeConfig( config );
}
