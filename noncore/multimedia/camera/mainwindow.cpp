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

#include "mainwindow.h"
#include "previewwidget.h"
#include "zcameraio.h"

#include <qapplication.h>
#include <qaction.h>
#include <qvbox.h>
#include <qcombobox.h>
#include <qcursor.h>
#include <qdatastream.h>
#include <qfile.h>
#include <qimage.h>
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qdirectpainter_qws.h>
#include <qpe/global.h>
#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>
#include <opie/ofiledialog.h>
#include <opie/odevice.h>
using namespace Opie;

#include <opie2/odebug.h>

#include <assert.h>

CameraMainWindow::CameraMainWindow( QWidget * parent, const char * name, WFlags f )
           :QMainWindow( parent, name, f ), _pics( 0 )
{
    #ifdef QT_NO_DEBUG
    if ( !ZCameraIO::instance()->isOpen() )
    {
        QVBox* v = new QVBox( this );
        v->setMargin( 10 );
        QLabel* l1 = new QLabel( v );
        l1->setPixmap( Resource::loadPixmap( "camera/error" ) );
        QLabel* l2 = new QLabel( v );
        l2->setText( "<b>Sorry. could not detect your camera :-(</b><p>"
        "* Is the sharpzdc_cs module loaded ?<br>"
        "* Is /dev/sharpzdc read/writable ?<p>" );
        connect( new QPushButton( "Exit", v ), SIGNAL( clicked() ), this, SLOT( close() ) );
        setCentralWidget( v );
        return;
    }
    #endif

    init();

    _rotation = 270; //TODO: grab these from the actual settings

    preview = new PreviewWidget( this, "camera preview widget" );
    //setCentralWidget( preview ); <--- don't do this!
    preview->resize( QSize( 240, 288 ) );
    preview->show();

    // construct a System Channel to receive setRotation messages
    _sysChannel = new QCopChannel( "QPE/System", this );
    connect( _sysChannel, SIGNAL( received( const QCString&, const QByteArray& ) ),
             this, SLOT( systemMessage( const QCString&, const QByteArray& ) ) );

    connect( preview, SIGNAL( contextMenuRequested() ), this, SLOT( showContextMenu() ) );

    connect( ZCameraIO::instance(), SIGNAL( shutterClicked() ), this, SLOT( shutterClicked() ) );
};


CameraMainWindow::~CameraMainWindow()
{
}


void CameraMainWindow::init()
{
    // TODO: Save this stuff in config
    quality = 50;
    zoom = 1;
    captureX = 640;
    captureY = 480;
    captureFormat = "JPEG";

    resog = new QActionGroup( 0, "reso", true );
    resog->setToggleAction( true );
    new QAction( " 64 x  48", 0, 0, resog, 0, true );
    new QAction( "128 x  96", 0, 0, resog, 0, true );
    new QAction( "192 x 144", 0, 0, resog, 0, true );
    new QAction( "256 x 192", 0, 0, resog, 0, true );
    new QAction( "320 x 240", 0, 0, resog, 0, true );
    new QAction( "384 x 288", 0, 0, resog, 0, true );
    new QAction( "448 x 336", 0, 0, resog, 0, true );
    new QAction( "512 x 384", 0, 0, resog, 0, true );
    new QAction( "576 x 432", 0, 0, resog, 0, true );
    ( new QAction( "640 x 480", 0, 0, resog, 0, true ) )->setOn( true );

    qualityg = new QActionGroup( 0, "quality", true );
    qualityg->setToggleAction( true );
    new QAction( "  0 (minimal)", 0, 0, qualityg, 0, true );
    new QAction( " 25 (low)", 0, 0, qualityg, 0, true );
    ( new QAction( " 50 (good)", 0, 0, qualityg, 0, true ) )->setOn( true );
    new QAction( " 75 (better)", 0, 0, qualityg, 0, true );
    new QAction( "100 (best)", 0, 0, qualityg, 0, true );

    zoomg = new QActionGroup( 0, "zoom", true );
    zoomg->setToggleAction( true );
    ( new QAction( "x 1", 0, 0, zoomg, 0, true ) )->setOn( true );
    new QAction( "x 2", 0, 0, zoomg, 0, true );

    outputg = new QActionGroup( 0, "output", true );
    outputg->setToggleAction( true );
    ( new QAction( "JPEG", 0, 0, outputg, 0, true ) )->setOn( true );
    new QAction( "PNG", 0, 0, outputg, 0, true );
    new QAction( "BMP", 0, 0, outputg, 0, true );

    connect( resog, SIGNAL( selected(QAction*) ), this, SLOT( resoMenuItemClicked(QAction*) ) );
    connect( qualityg, SIGNAL( selected(QAction*) ), this, SLOT( qualityMenuItemClicked(QAction*) ) );
    connect( zoomg, SIGNAL( selected(QAction*) ), this, SLOT( zoomMenuItemClicked(QAction*) ) );
    connect( outputg, SIGNAL( selected(QAction*) ), this, SLOT( outputMenuItemClicked(QAction*) ) );
}


void CameraMainWindow::systemMessage( const QCString& msg, const QByteArray& data )
{
    QDataStream stream( data, IO_ReadOnly );
    odebug << "received system message: " << msg << oendl;
    if ( msg == "setCurrentRotation(int)" )
    {
        stream >> _rotation;
        odebug << "received setCurrentRotation(" << _rotation << ")" << oendl;

        switch ( _rotation )
        {
            case 270: preview->resize( QSize( 240, 288 ) ); break;
            case 180: preview->resize( QSize( 320, 208 ) ); break;
            default: QMessageBox::warning( this, "opie-camera",
                "This rotation is not supported.\n"
                "Supported are 180� and 270�" );
        }
    }
}


void CameraMainWindow::changeZoom( int zoom )
{
    int z;
    switch ( zoom )
    {
        case 0: z = 128; break;
        case 1: z = 256; break;
        case 2: z = 512; break;
        default: assert( 0 ); break;
    }

    ZCameraIO::instance()->setCaptureFrame( 240, 160, z );
}

void CameraMainWindow::showContextMenu()
{
    QPopupMenu reso;
    reso.setCheckable( true );
    resog->addTo( &reso );

    QPopupMenu quality;
    quality.setCheckable( true );
    qualityg->addTo( &quality );

    QPopupMenu zoom;
    zoom.setCheckable( true );
    zoomg->addTo( &zoom );

    QPopupMenu output;
    output.setCheckable( true );
    outputg->addTo( &output );

    QPopupMenu m( this );
    m.insertItem( "&Resolution", &reso );
    m.insertItem( "&Zoom", &zoom );
    m.insertItem( "&Quality", &quality );
    m.insertItem( "&Output As", &output );
    m.exec( QCursor::pos() );
}


void CameraMainWindow::resoMenuItemClicked( QAction* a )
{
    captureX = a->text().left(3).toInt();
    captureY = a->text().right(3).toInt();
    odebug << "Capture Resolution now: " << captureX << ", " << captureY << oendl;
}


void CameraMainWindow::qualityMenuItemClicked( QAction* a )
{
    quality = a->text().left(3).toInt();
    odebug << "Quality now: " << quality << oendl;
}


void CameraMainWindow::zoomMenuItemClicked( QAction* a )
{
    zoom = QString( a->text()[2] ).toInt();
    odebug << "Zoom now: " << zoom << oendl;
    ZCameraIO::instance()->setZoom( zoom );
}


void CameraMainWindow::outputMenuItemClicked( QAction* a )
{
    captureFormat = a->text();
    odebug << "Output format now: " << captureFormat << oendl;
}


void CameraMainWindow::shutterClicked()
{
    Global::statusMessage( "CAPTURING..." );
    qApp->processEvents();

    odebug << "Shutter has been pressed" << oendl;
    ODevice::inst()->touchSound();
    QString name;
    name.sprintf( "/tmp/image-%d_%d_%d_q%d.%s", _pics++, captureX, captureY, quality, (const char*) captureFormat.lower() );
    QImage i;
    ZCameraIO::instance()->captureFrame( captureX, captureY, zoom, &i );
    QImage im = i.convertDepth( 32 );
    bool result = im.save( name, captureFormat, quality );
    if ( !result )
    {
        oerr << "imageio-Problem while writing." << oendl;
        Global::statusMessage( "Error!" );
    }
    else
    {
        odebug << captureFormat << "-image has been successfully captured" << oendl;
        Global::statusMessage( "Ok." );
    }
}

