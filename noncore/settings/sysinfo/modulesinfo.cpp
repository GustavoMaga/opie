/**********************************************************************
** ModulesInfo
**
** Display Modules information
**
** Copyright (C) 2002, Michael Lauer
**                    mickey@tm.informatik.uni-frankfurt.de
**                    http://www.Vanille.de
**
** Based on ProcessInfo by Dan Williams <williamsdr@acm.org>
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

#include "modulesinfo.h"
#include "detail.h"

/* OPIE */
#include <opie2/olistview.h>
#include <qpe/qpeapplication.h>

/* QT */
#include <qcombobox.h>
#include <qfile.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtextview.h>
#include <qtimer.h>
#include <qwhatsthis.h>

using namespace Opie::Ui;
ModulesInfo::ModulesInfo( QWidget* parent,  const char* name, WFlags fl )
        : QWidget( parent, name, fl )
{
    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( 4 );
    layout->setMargin( 4 );

    ModulesView = new OListView( this );
    int colnum = ModulesView->addColumn( tr( "Module" ) );
    colnum = ModulesView->addColumn( tr( "Size" ) );
    ModulesView->setColumnAlignment( colnum, Qt::AlignRight );
    colnum = ModulesView->addColumn( tr( "Use#" ) );
    ModulesView->setColumnAlignment( colnum, Qt::AlignRight );
    colnum = ModulesView->addColumn( tr( "Used by" ) );
    ModulesView->setAllColumnsShowFocus( TRUE );
    layout->addMultiCellWidget( ModulesView, 0, 0, 0, 1 );
    QWhatsThis::add( ModulesView, tr( "This is a list of all the kernel modules currently loaded on this handheld device.\n\nClick and hold on a module to see additional information about the module, or to unload it." ) );

    // Test if we have /sbin/modinfo, and if so, allow module detail window
    if ( QFile::exists( "/sbin/modinfo" ) )
    {
        QPEApplication::setStylusOperation( ModulesView->viewport(), QPEApplication::RightOnHold );
        connect( ModulesView, SIGNAL( rightButtonPressed(QListViewItem*,const QPoint&,int) ),
                 this, SLOT( viewModules(QListViewItem*) ) );
    }

    CommandCB = new QComboBox( FALSE, this );
    CommandCB->insertItem( "modprobe -r" );
    CommandCB->insertItem( "rmmod" );
    // I can't think of other useful commands yet. Anyone?
    layout->addWidget( CommandCB, 1, 0 );
    QWhatsThis::add( CommandCB, tr( "Select a command here and then click the Send button to the right to send the command to module selected above." ) );

    QPushButton *btn = new QPushButton( this );
    btn->setMinimumSize( QSize( 50, 24 ) );
    btn->setMaximumSize( QSize( 50, 24 ) );
    btn->setText( tr( "Send" ) );
    connect( btn, SIGNAL( clicked() ), this, SLOT( slotSendClicked() ) );
    layout->addWidget( btn, 1, 1 );
    QWhatsThis::add( btn, tr( "Click here to send the selected command to the module selected above." ) );

    QTimer *t = new QTimer( this );
    connect( t, SIGNAL( timeout() ), this, SLOT( updateData() ) );
    t->start( 5000 );

    updateData();

    ModulesDtl = new Detail();
    QWhatsThis::add( ModulesDtl->detailView, tr( "This area shows detailed information about this module." ) );
}

ModulesInfo::~ModulesInfo()
{}

void ModulesInfo::updateData()
{
    char modname[64];
    char usage[200];
    int modsize, usecount;

    QString selectedmod;
    OListViewItem *curritem = static_cast<OListViewItem*>( ModulesView->currentItem() );
    if ( curritem )
    {
        selectedmod = curritem->text( 0 );
    }

    ModulesView->clear();

    FILE *procfile = fopen( ( QString ) ( "/proc/modules"), "r");

    if ( procfile )
    {
        OListViewItem *newitem;
        OListViewItem *selecteditem = 0x0;
        while ( true )
        {
            modname[0] = '\0';
            usage[0] = '\0';
            int success = fscanf( procfile, "%s%d%d%[^\n]", modname, &modsize, &usecount, usage );

            if ( success == EOF )
                break;

            QString qmodname = QString( modname );
            QString qmodsize = QString::number( modsize ).rightJustify( 6, ' ' );
            QString qusecount = QString::number( usecount ).rightJustify( 2, ' ' );
            QString qusage = QString( usage );

            newitem = new OListViewItem( ModulesView, qmodname, qmodsize, qusecount, qusage );
            if ( qmodname == selectedmod )
            {
                selecteditem = newitem;
            }
        }
        ModulesView->setCurrentItem( selecteditem );

        fclose( procfile );
    }
}

void ModulesInfo::slotSendClicked()
{
    if ( !ModulesView->currentItem() )
    {
        return;
    }

    QString capstr = tr( "You really want to execute\n%1 for this module?" ).arg( CommandCB->currentText() );

    QString modname = ModulesView->currentItem()->text( 0 );

    if ( QMessageBox::warning( this, modname, capstr,
                               QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape ) == QMessageBox::Yes )
    {
        QString command = "/sbin/";
        command.append( CommandCB->currentText() );
        command.append( " " );
        command.append( modname );

        FILE* stream = popen( command, "r" );
        if ( stream )
            pclose( stream );
    }

}

void ModulesInfo::viewModules( QListViewItem *module ) {
    if ( !module )
        return;
    viewModules(  static_cast<OListViewItem*>( module ) );
}
void ModulesInfo::viewModules( OListViewItem *modules )
{
    QString modname = modules->text( 0 );
    QString capstr = "Module: ";
    capstr.append( modname );
    ModulesDtl->setCaption( capstr );
    QString command = "/sbin/modinfo ";
    command.append( modname );
    FILE* modinfo = popen( command, "r" );

    if ( modinfo )
    {
        char line[200];
        ModulesDtl->detailView->setText( " Details:\n------------\n" );

        while( true )
        {
            int success = fscanf( modinfo, "%[^\n]\n", line );
            if ( success == EOF )
                break;
            ModulesDtl->detailView->append( line );
        }

        pclose( modinfo );
    }

    QPEApplication::showWidget( ModulesDtl );
}

