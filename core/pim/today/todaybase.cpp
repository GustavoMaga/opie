/*
 * todaybase.cpp *
 * ---------------------
 *
 * begin       : Sun 10 17:20:00 CEST 2002
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

#include "todaybase.h"

#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qscrollview.h>

#include <qpe/resource.h>

/* 
 *  Constructs a TodayBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
TodayBase::TodayBase( QWidget* parent,  const char* name, WFlags fl )
  : QWidget( parent, name, fl )
{
  // logo
  QPixmap logo = Resource::loadPixmap( "today/today_logo");
  // datebook
  QPixmap datebook = Resource::loadPixmap("DateBook");
  // todo
  QPixmap todo = Resource::loadPixmap( "TodoList" );
  // config icon
  QPixmap config = Resource::loadPixmap( "today/config" );
  
  if ( !name )
    setName( "TodayBase" );
  resize( 223, 307 ); 
  //setCaption( tr( "My Test App" ) );

  Frame4 = new QFrame( this, "Frame4" );
  Frame4->setGeometry( QRect( -9, 50, 250, 150 ) ); 
  Frame4->setFrameShape( QScrollView::StyledPanel );
  Frame4->setFrameShadow( QScrollView::Sunken );
  Frame4->setBackgroundOrigin( QScrollView::ParentOrigin );

  // hehe, qt is ...
  getridoffuckingstrippeldlinesbutton =  new QPushButton (Frame4, "asdfsad" );
  getridoffuckingstrippeldlinesbutton->setGeometry( QRect( 2, 10, 0, 0 ) );

  DatesButton = new QPushButton (Frame4, "DatesButton" );
  DatesButton->setGeometry( QRect( 10, 10, 36, 32 ) );
  DatesButton->setBackgroundOrigin( QPushButton::WidgetOrigin );
  DatesButton->setPixmap( datebook  );
  DatesButton->setFlat( TRUE );


  //PixmapLabel7 = new QLabel( Frame4, "PixmapLabel7" );
  //PixmapLabel7->setGeometry( QRect( 10, 10, 36, 32 ) ); 
  //PixmapLabel7->setPixmap( datebook );
  //PixmapLabel7->setScaledContents( FALSE );
  
  DatesField = new QLabel( Frame4, "DatesField" );
  DatesField->setGeometry( QRect( 47, 10, 203, 120 ) ); 
  DatesField->setText( tr( "No appointments today" ) );
  DatesField->setAlignment( int( QLabel::AlignTop | QLabel::AlignLeft ) );

  Frame = new QLabel( this, "Frame" );
  Frame->setGeometry( QRect( 0, 0, 240, 50 ) ); 
  QPalette pal;
  QColorGroup cg;
  //cg.setColor( QColorGroup::Foreground, white );
  //  cg.setColor( QColorGroup::Button, QColor( 238, 234, 222) );
  //cg.setColor( QColorGroup::Light, white );
  //cg.setColor( QColorGroup::Midlight, QColor( 246, 244, 238) );
  // cg.setColor( QColorGroup::Dark, QColor( 119, 117, 111) );
  //cg.setColor( QColorGroup::Mid, QColor( 158, 155, 147) );
  cg.setColor( QColorGroup::Text, white );
  //cg.setColor( QColorGroup::BrightText, white );
  //cg.setColor( QColorGroup::ButtonText, black );
  //cg.setColor( QColorGroup::Base, white );
  cg.setBrush( QColorGroup::Background, QBrush( QColor( 238, 238, 230), logo ) );
  //cg.setColor( QColorGroup::Shadow, black );
  //cg.setColor( QColorGroup::Highlight, QColor( 74, 121, 205) );
  //cg.setColor( QColorGroup::HighlightedText, white );
  pal.setActive( cg );

  Frame->setPalette( pal );
  Frame->setFrameShape( QFrame::StyledPanel );
  Frame->setFrameShadow( QFrame::Raised );
  Frame->setLineWidth( 0 );

  // datum
  TextLabel1 = new QLabel( Frame, "TextLabel1" );
  TextLabel1->setPalette( pal );
  TextLabel1->setGeometry( QRect( 10, 35, 168, 12 ) ); 
  QFont TextLabel1_font(  TextLabel1->font() );
  TextLabel1_font.setBold( TRUE );
  TextLabel1->setFont( TextLabel1_font ); 
  TextLabel1->setBackgroundOrigin( QLabel::ParentOrigin );
  //TextLabel1->setText( tr( "" ) );
  TextLabel1->setTextFormat( QLabel::AutoText );
  //QToolTip::add(  TextLabel1, tr( "Test" ) );
  
  
  
  // todo
  Frame15 = new QFrame( this, "Frame15" );
  Frame15->setGeometry( QRect( -9, 200, 250, 130 ) ); 
  Frame15->setFrameShape( QFrame::StyledPanel );
  Frame15->setFrameShadow( QFrame::Sunken );
  //Frame15->setBackgroundMode( PaletteBase );
  //Frame15->setBackgroundMode( PaletteBackground );
  
 
  // PixmapLabel1 = new QLabel( Frame15, "PixmapLabel1" );
  //PixmapLabel1->setGeometry( QRect( 10, 4, 36, 32 ) ); 
  //PixmapLabel1->setBackgroundOrigin( QLabel::ParentOrigin );
  //PixmapLabel1->setPixmap( todo );
  //PixmapLabel1->setScaledContents( FALSE );
  

  TodoField = new QLabel( Frame15, "TodoButtonBack" );
  TodoField->setGeometry( QRect( 10, 4, 36, 32 ) ); 
  TodoField->setFrameShadow( QLabel::Plain );
  TodoField->setAlignment( int( QLabel::AlignTop | QLabel::AlignLeft ) );

  TodoButton = new QPushButton (Frame15, "TodoButton" );
  TodoButton->setGeometry( QRect( 10, 4, 36, 32 ) );
  TodoButton->setBackgroundOrigin( QPushButton::WidgetOrigin );
  TodoButton->setPixmap( todo  );
  TodoButton->setFlat( TRUE );

  TodoField = new QLabel( Frame15, "TodoField" );
  TodoField->setGeometry( QRect( 47, 10, 196, 120 ) ); 
  TodoField->setFrameShadow( QLabel::Plain );
  TodoField->setText( tr( "No current todos" ) );
  TodoField->setAlignment( int( QLabel::AlignTop | QLabel::AlignLeft ) );

  PushButton1 = new QPushButton (Frame15, "PushButton1" );
  PushButton1->setGeometry( QRect( 225, 68, 25, 21 ) ); 
  PushButton1->setBackgroundOrigin( QPushButton::WidgetOrigin );
  PushButton1->setPixmap( config  );
  PushButton1->setAutoDefault( TRUE );
  PushButton1->setFlat( TRUE );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
TodayBase::~TodayBase()
{
  // no need to delete child widgets, Qt does it all for us
}

/*  
 *  Main event handler. Reimplemented to handle application
 *  font changes
 */
bool TodayBase::event( QEvent* ev )
{
  bool ret = QWidget::event( ev ); 
  //if ( ev->type() == QEvent::ApplicationFontChange ) {
  //  QFont TextLabel1_font(  TextLabel1->font() );
  //  TextLabel1_font.setBold( TRUE );
  //  TextLabel1->setFont( TextLabel1_font ); 
  // }
  return ret;
}

