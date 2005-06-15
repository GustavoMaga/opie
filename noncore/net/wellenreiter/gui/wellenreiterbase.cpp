/**********************************************************************
** Copyright (C) 2002-2004 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Wellenreiter II.
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

#include <qlabel.h>
#include <qlayout.h>

#include "logwindow.h"
#include "packetview.h"
#include "scanlist.h"
#include "statwindow.h"
#include "graphwindow.h"

#ifdef QWS
#include <opie2/oresource.h>
#include <opie2/otabwidget.h>
using namespace Opie;
#else
#include "resource.h"
#include <qtabwidget.h>
#endif

/*
 *  Constructs a WellenreiterBase which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
using namespace Opie::Ui;
using namespace Opie::Ui;
using namespace Opie::Ui;
WellenreiterBase::WellenreiterBase( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    //ani1 = new QPixmap( Opie::Core::OResource::loadPixmap( "wellenreiter/networks_rot0", Opie::Core::OResource::SmallIcon ) );
    //ani2 = new QPixmap( Opie::Core::OResource::loadPixmap( "wellenreiter/networks_rot90", Opie::Core::OResource::SmallIcon ) );
    //ani3 = new QPixmap( Opie::Core::OResource::loadPixmap( "wellenreiter/networks_rot180", Opie::Core::OResource::SmallIcon ) );
    //ani4 = new QPixmap( Opie::Core::OResource::loadPixmap( "wellenreiter/networks_rot270", Opie::Core::OResource::SmallIcon ) );

    if ( !name )
        setName( "WellenreiterBase" );
    resize( 191, 294 );
#ifdef QWS
    setCaption( tr( "Wellenreiter/Opie" ) );
#else
    setCaption( tr( "Wellenreiter/X11" ) );
#endif
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

    //--------- GRAPH TAB --------------

    graphwindow = new MGraphWindow( TabWidget, "Graph" );

    //--------- LOG TAB --------------

    logwindow = new MLogWindow( TabWidget, "Log" );

    //--------- HEX TAB --------------

    hexwindow = new PacketView( TabWidget, "Hex" );

    //--------- STAT TAB --------------

    statwindow = new MStatWindow( TabWidget, "Stat" );

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

    QPixmap logo = Opie::Core::OResource::loadPixmap( "wellenreiter/logo" );
    QPainter draw( &logo );
    draw.setPen( Qt::black );
    draw.setFont( QFont( "Fixed", 8 ) );
    draw.drawText( 5, 10, WELLENREITER_VERSION );

    PixmapLabel1_3_2->setPixmap( logo );
    PixmapLabel1_3_2->setScaledContents( TRUE );
    PixmapLabel1_3_2->setAlignment( int( QLabel::AlignCenter ) );

    aboutLayout->addWidget( PixmapLabel1_3_2, 0, 0 );

    TextLabel1_4_2 = new QLabel( about, "TextLabel1_4_2" );
    QFont TextLabel1_4_2_font(  TextLabel1_4_2->font() );
    TextLabel1_4_2_font.setFamily( "adobe-helvetica" );
    TextLabel1_4_2_font.setPointSize( 10 );
    TextLabel1_4_2->setFont( TextLabel1_4_2_font );
    TextLabel1_4_2->setText(
"<p align=center>"
"<hr>"
"<b>(C) Michael 'Mickey' Lauer</b><br>"
"<hr>"
"mickey@Vanille.de<br>"
"www.Vanille.de/projects/wellenreiter.html<br>"
"www.wellenreiter.net"
"</p>" );
    TextLabel1_4_2->setAlignment( int( QLabel::AlignCenter ) );

    aboutLayout->addWidget( TextLabel1_4_2, 1, 0 );

#ifdef QWS
    TabWidget->addTab( ap, "wellenreiter/networks", tr( "Nets" ) );
    TabWidget->addTab( graphwindow, "wellenreiter/graph", tr( "Graph" ) );
    TabWidget->addTab( logwindow, "wellenreiter/log", tr( "Log" ) );
    TabWidget->addTab( hexwindow, "wellenreiter/hex", tr( "Hex" ) );
    TabWidget->addTab( statwindow, "wellenreiter/stat", tr( "Stat" ) );
    TabWidget->addTab( about, "wellenreiter/about", tr( "About" ) );
#else
    TabWidget->addTab( ap, /* "wellenreiter/networks", */ tr( "Networks" ) );
    TabWidget->addTab( graphwindow, /* "wellenreiter/graph", */ tr( "Graph" ) );
    TabWidget->addTab( logwindow, /* "wellenreiter/log", */ tr( "Log" ) );
    TabWidget->addTab( hexwindow, /* "wellenreiter/hex", */ tr( "Hex" ) );
    TabWidget->addTab( statwindow, /* "wellenreiter/hex", */ tr( "Stat" ) );
    TabWidget->addTab( about, /* "wellenreiter/about", */ tr( "About" ) );
#endif
    WellenreiterBaseLayout->addWidget( TabWidget );

#ifdef QWS
    TabWidget->setCurrentTab( tr( "Nets" ) );
#endif

    connect(TabWidget, SIGNAL(currentChanged(QWidget*)),
            this, SLOT(slotTabChanged(QWidget*)));
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

