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
***********************************************************************/

#include "wellenreiterbase.h"

#include <qheader.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "logwindow.h"
#include "hexwindow.h"
#include "configwindow.h"
#include "scanlist.h"

#ifdef QWS
#include <qpe/resource.h>
#include <opie/otabwidget.h>
#else
#include "resource.h"
#include <qtabwidget.h>
#endif


/* 
 *  Constructs a WellenreiterBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
WellenreiterBase::WellenreiterBase( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    ani1 = new QPixmap( Resource::loadPixmap( "wellenreiter/networks_rot0" ) );
    ani2 = new QPixmap( Resource::loadPixmap( "wellenreiter/networks_rot90" ) );
    ani3 = new QPixmap( Resource::loadPixmap( "wellenreiter/networks_rot180" ) );
    ani4 = new QPixmap( Resource::loadPixmap( "wellenreiter/networks_rot270" ) );
    
    if ( !name )
        setName( "WellenreiterBase" );
    resize( 191, 294 ); 
    setCaption( tr( "Wellenreiter" ) );
    WellenreiterBaseLayout = new QVBoxLayout( this ); 
    WellenreiterBaseLayout->setSpacing( 2 );
    WellenreiterBaseLayout->setMargin( 0 );
#ifdef QWS
    TabWidget = new OTabWidget( this, "TabWidget", OTabWidget::Global );
#else
    TabWidget = new QTabWidget( this, "TabWidget" );
#endif
    ap = new QWidget( TabWidget, "ap" );
    apLayout = new QVBoxLayout( ap ); 
    apLayout->setSpacing( 2 );
    apLayout->setMargin( 2 );

    //--------- NETVIEW TAB --------------    
    
    netview = new MScanListView( ap );    
    apLayout->addWidget( netview );
    

    //--------- LOG TAB --------------
    
    logwindow = new MLogWindow( TabWidget, "Log" );
    

    //--------- HEX TAB --------------
    
    hexwindow = new MHexWindow( TabWidget, "Hex" );

    //--------- CONFIG TAB --------------
    
    configwindow = new WellenreiterConfigWindow( TabWidget, "Config" );
    
    //--------- ABOUT TAB --------------
    
    about = new QWidget( TabWidget, "about" );
    aboutLayout = new QGridLayout( about ); 
    aboutLayout->setSpacing( 6 );
    aboutLayout->setMargin( 11 );

    PixmapLabel1_3_2 = new QLabel( about, "PixmapLabel1_3_2" );
    PixmapLabel1_3_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, PixmapLabel1_3_2->sizePolicy().hasHeightForWidth() ) );
    PixmapLabel1_3_2->setFrameShape( QLabel::Panel );
    PixmapLabel1_3_2->setFrameShadow( QLabel::Sunken );
    PixmapLabel1_3_2->setLineWidth( 2 );
    PixmapLabel1_3_2->setMargin( 0 );
    PixmapLabel1_3_2->setMidLineWidth( 0 );
    PixmapLabel1_3_2->setPixmap(  Resource::loadPixmap( "wellenreiter/logo" ) );
    PixmapLabel1_3_2->setScaledContents( TRUE );
    PixmapLabel1_3_2->setAlignment( int( QLabel::AlignCenter ) );

    aboutLayout->addWidget( PixmapLabel1_3_2, 0, 0 );

    TextLabel1_4_2 = new QLabel( about, "TextLabel1_4_2" );
    QFont TextLabel1_4_2_font(  TextLabel1_4_2->font() );
    TextLabel1_4_2_font.setFamily( "adobe-helvetica" );
    TextLabel1_4_2_font.setPointSize( 10 );
    TextLabel1_4_2->setFont( TextLabel1_4_2_font ); 
    TextLabel1_4_2->setText( tr( "<p align=center>\n"
"<hr>\n"
"Max Moser<br>\n"
"Martin J. Muench<br>\n"
"Michael Lauer<br><hr>\n"
"<b>www.remote-exploit.org</b>\n"
"</p>" ) );
    TextLabel1_4_2->setAlignment( int( QLabel::AlignCenter ) );

    aboutLayout->addWidget( TextLabel1_4_2, 1, 0 );
   
    button = new QPushButton( this, "button" );
    button->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, button->sizePolicy().hasHeightForWidth() ) );
    button->setText( tr( "Start Scanning" ) );

#ifdef QWS    
    TabWidget->addTab( ap, "wellenreiter/networks", tr( "Networks" ) );
    TabWidget->addTab( logwindow, "wellenreiter/log", tr( "Log" ) );
    TabWidget->addTab( hexwindow, "wellenreiter/hex", tr( "Hex" ) );
    TabWidget->addTab( configwindow, "wellenreiter/config", tr( "Config" ) );
    TabWidget->addTab( about, "wellenreiter/about", tr( "About" ) );
#else
    TabWidget->addTab( ap, /* "wellenreiter/networks", */ tr( "Networks" ) );
    TabWidget->addTab( logwindow, /* "wellenreiter/log", */ tr( "Log" ) );
    TabWidget->addTab( hexwindow, /* "wellenreiter/hex", */ tr( "Hex" ) );
    TabWidget->addTab( configwindow, /* "wellenreiter/config", */ tr( "Config" ) );
    TabWidget->addTab( about, /* "wellenreiter/about", */ tr( "About" ) );
#endif   
    WellenreiterBaseLayout->addWidget( TabWidget );
    WellenreiterBaseLayout->addWidget( button );
   
#ifdef QWS   
    TabWidget->setCurrentTab( tr( "Networks" ) );
#endif

}

/*  
 *  Destroys the object and frees any allocated resources
 */
WellenreiterBase::~WellenreiterBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*  
 *  Main event handler. Reimplemented to handle application
 *  font changes
 */
bool WellenreiterBase::event( QEvent* ev )
{
    bool ret = QWidget::event( ev ); 
    if ( ev->type() == QEvent::ApplicationFontChange ) {
	//QFont Log_2_font(  Log_2->font() );
	//Log_2_font.setFamily( "adobe-courier" );
	//Log_2_font.setPointSize( 8 );
	//Log_2->setFont( Log_2_font ); 
	QFont TextLabel1_4_2_font(  TextLabel1_4_2->font() );
	TextLabel1_4_2_font.setFamily( "adobe-helvetica" );
	TextLabel1_4_2_font.setPointSize( 10 );
	TextLabel1_4_2->setFont( TextLabel1_4_2_font ); 
    }
    return ret;
}

