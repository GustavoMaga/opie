/* main.cpp
 *
 * ---------------------
 *
 * copyright   : (c) 2002 by Maximilian Rei�
 * email       : max.reiss@gmx.de
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "scandialog.h"

#include <qframe.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qprogressbar.h>

#include <manager.h>
#include <device.h>
#include <remotedevice.h>


namespace OpieTooth {

/*
 */
    ScanDialog::ScanDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
        : QDialog( parent, name, modal, fl ) {

        if ( !name )
            setName( "ScanDialog" );
        resize( 240, 320 );
        setCaption( tr( "Scan for devices" ) );

        Frame7 = new QFrame( this, "Frame7" );
        Frame7->setGeometry( QRect( 0, 0, 240, 331 ) );
        Frame7->setFrameShape( QFrame::StyledPanel );
        Frame7->setFrameShadow( QFrame::Raised );


        QWidget* privateLayoutWidget = new QWidget( Frame7, "Layout11" );
        privateLayoutWidget->setGeometry( QRect( 10, 9, 221, 310 ) );
        Layout11 = new QGridLayout( privateLayoutWidget );
        Layout11->setSpacing( 6 );
        Layout11->setMargin( 0 );


        progress = new QProgressBar(privateLayoutWidget,  "progbar");
        progress->setTotalSteps(20);

        //Layout11->addMultiCellWidget( progress, 1, 1, 0, 1 );
        Layout11->addWidget(progress,  2, 2);

        StartButton = new QPushButton( privateLayoutWidget, "StartButton" );
        StartButton->setText( tr( "Start" ) );

        Layout11->addWidget( StartButton, 2, 0 );

        StopButton = new QPushButton( privateLayoutWidget, "StopButton" );
        StopButton->setText( tr( "Cancel" ) );

        Layout11->addWidget( StopButton, 2, 1 );

        ListView1 = new QListView( privateLayoutWidget, "ListView1" );

        ListView1->addColumn( tr( "Add" ) );
        ListView1->addColumn( tr( "Device Name" ) );
        //ListView1->addColumn( tr( "Type" ) );

        Layout11->addMultiCellWidget( ListView1, 0, 0, 0, 1 );

        connect( (QObject*)StartButton, SIGNAL( clicked() ), this, SLOT( startSearch() ) );
        connect( (QObject*)StopButton, SIGNAL( clicked() ),  this,  SLOT( stopSearch() ) );
        connect( (QObject*)localDevice, SIGNAL( foundDevices( const QString& , RemoteDevices::ValueList ) ),
                 this, SLOT(fillList(const QString& , RemoteDevices::ValueList ) ) ) ;
        progressStat = 0;
    }

// hack, make cleaner later
    void ScanDialog::progressTimer(int maxSeconds) {

        progressStat++;
            if (progressStat++ < maxSeconds) {
                QTimer::singleShot( 1000, this, SLOT(progressTimer ) );
            }

    }

    void ScanDialog::startSearch() {
        progress->setProgress(0);
        progressStat = 0;
         // read it from config later
        localDevice = new Manager( "hci0" );
        progressTimer(20);
        // when finished, it emmite foundDevices()
        // checken ob initialisiert , qcop ans applet.
        localDevice->searchDevices();
        progress->setProgress(20);
    }

    void ScanDialog::stopSearch() {

    }

    void ScanDialog::fillList(const QString& device, RemoteDevices::ValueList deviceList) {

        QListViewItem * deviceItem;

        RemoteDevices::ValueList::Iterator it;
        for( it = deviceList.begin(); it != deviceList.end(); ++it ) {

            deviceItem = new QListViewItem( ListView1, (*it).name() );
        }
    }

/*
 * Cleanup
 */
    ScanDialog::~ScanDialog() {
        delete localDevice;
    }

}
