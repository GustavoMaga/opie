/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
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

/* LOCAL */
#include "configwindow.h"
#include "mainwindow.h"

/* QT */
#include <qapplication.h>
#include <qcombobox.h>
#include <qfile.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qmap.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qspinbox.h>
#include <qtextstream.h>

/* OPIE */
#include <opie2/onetwork.h>

WellenreiterConfigWindow* WellenreiterConfigWindow::_instance = 0;

WellenreiterConfigWindow::WellenreiterConfigWindow( QWidget * parent, const char * name, WFlags f )
           :WellenreiterConfigBase( parent, name, true, f )
{
    _devicetype[ "cisco" ] = DEVTYPE_CISCO;
    _devicetype[ "wlan-ng" ] = DEVTYPE_WLAN_NG;
    _devicetype[ "hostap" ] = DEVTYPE_HOSTAP;
    _devicetype[ "orinoco" ] = DEVTYPE_ORINOCO;
    _devicetype[ "<manual>" ] = DEVTYPE_MANUAL;
    _devicetype[ "<file>" ] = DEVTYPE_FILE;

    // gather possible interface names from ONetwork
    ONetwork* net = ONetwork::instance();
    ONetwork::InterfaceIterator it = net->iterator();
    while ( it.current() )
    {
        if ( it.current()->isWireless() )
            interfaceName->insertItem( it.current()->name() );
        ++it;
    }

    // try to guess device type
    QFile m( "/proc/modules" );
    if ( m.open( IO_ReadOnly ) )
    {
        int devicetype(0);
        QString line;
        QTextStream modules( &m );
        while( !modules.atEnd() && !devicetype )
        {
            modules >> line;
            if ( line.contains( "cisco" ) ) devicetype = DEVTYPE_CISCO;
            else if ( line.contains( "hostap" ) ) devicetype = DEVTYPE_HOSTAP;
            else if ( line.contains( "prism" ) ) devicetype = DEVTYPE_WLAN_NG;
            else if ( line.contains( "orinoco" ) ) devicetype = DEVTYPE_ORINOCO;
        }
        if ( devicetype )
        {
            deviceType->setCurrentItem( devicetype );
            _guess = devicetype;
            qDebug( "Wellenreiter: guessed device type to be #%d", devicetype );
        }
    }

    #ifdef Q_WS_X11 // We're on X11: adding an Ok-Button for the Dialog here
    QPushButton* okButton = new QPushButton( "ok", this );
    okButton->show();
    Layout5_2->addWidget( okButton, 0, 3 ); //FIXME: rename this in configbase.ui
    connect( okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    #endif

    WellenreiterConfigWindow::_instance = this;

    connect( deviceType, SIGNAL( activated(int) ), this, SLOT( changedDeviceType(int) ) );
    connect( getCaptureFileName, SIGNAL( clicked() ), this, SLOT( getCaptureFileNameClicked() ) );
};


int WellenreiterConfigWindow::daemonDeviceType()
{
    QString name = deviceType->currentText();
    if ( _devicetype.contains( name ) )
    {
        return _devicetype[name];
    }
    else
    {
        return 0;
    }
};


int WellenreiterConfigWindow::daemonHopInterval()
{
    return hopInterval->cleanText().toInt();
}


void WellenreiterConfigWindow::changedDeviceType(int t)
{
    if ( t != DEVTYPE_FILE ) return;
    QString name = ( (WellenreiterMainWindow*) qApp->mainWidget() )->getFileName(false);
    if ( !name.isEmpty() && QFile::exists( name ) )
    {
        interfaceName->insertItem( name );
        interfaceName->setCurrentItem( interfaceName->count()-1 );
    }
    else
    {
        deviceType->setCurrentItem( _guess );
    }

}


void WellenreiterConfigWindow::getCaptureFileNameClicked()
{
    QString name = ( (WellenreiterMainWindow*) qApp->mainWidget() )->getFileName(true);
    qDebug( "name = %s", (const char*) name );
    if ( !name.isEmpty() )
    {
        captureFileName->setText( name );
    }
}


bool WellenreiterConfigWindow::usePrismHeader()
{
    return true;
    //return interface->usePrismHeader();
}
