/****************************************************************************
** Form implementation generated from reading ui file 'todayconfig.ui'
**
** Created: Thu Feb 14 15:04:33 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "todayconfig.h"

#include <qcheckbox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a todayconfig which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
todayconfig::todayconfig( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "todayconfig" );
    resize( 175, 232 ); 
    setCaption( tr( "Today config" ) );

    TabWidget3 = new QTabWidget( this, "TabWidget3" );
    TabWidget3->setGeometry( QRect( 0, 0, 220, 320 ) ); 
    TabWidget3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, TabWidget3->sizePolicy().hasHeightForWidth() ) );
    TabWidget3->setAutoMask( FALSE );
    TabWidget3->setTabShape( QTabWidget::Rounded );

    tab = new QWidget( TabWidget3, "tab" );

    Frame8 = new QFrame( tab, "Frame8" );
    Frame8->setGeometry( QRect( -10, 0, 200, 300 ) ); 
    Frame8->setFrameShape( QFrame::StyledPanel );
    Frame8->setFrameShadow( QFrame::Raised );

    TextLabel4 = new QLabel( Frame8, "TextLabel4" );
    TextLabel4->setGeometry( QRect( 20, 89, 100, 60 ) ); 
    TextLabel4->setText( tr( "Should the \n"
"location \n"
"be shown?" ) );

    TextLabel5 = new QLabel( Frame8, "TextLabel5" );
    TextLabel5->setGeometry( QRect( 20, 160, 120, 40 ) ); 
    TextLabel5->setText( tr( "Should the notes \n"
"be shown?" ) );

    CheckBox2 = new QCheckBox( Frame8, "CheckBox2" );
    CheckBox2->setGeometry( QRect( 160, 170, 27, 21 ) ); 
    CheckBox2->setText( tr( "" ) );

    CheckBox1 = new QCheckBox( Frame8, "CheckBox1" );
    CheckBox1->setGeometry( QRect( 160, 90, 27, 50 ) ); 
    CheckBox1->setText( tr( "" ) );

    SpinBox1 = new QSpinBox( Frame8, "SpinBox1" );
    SpinBox1->setGeometry( QRect( 120, 20, 58, 25 ) ); 
    SpinBox1->setMaxValue( 10 );
    SpinBox1->setValue( 5 );

    TextLabel3 = new QLabel( Frame8, "TextLabel3" );
    TextLabel3->setGeometry( QRect( 20, 10, 90, 70 ) ); 
    TextLabel3->setText( tr( "How many \n"
"appointment\n"
"should should \n"
"be shown?" ) );
    TabWidget3->insertTab( tab, tr( "Calendar" ) );

    tab_2 = new QWidget( TabWidget3, "tab_2" );

    Frame9 = new QFrame( tab_2, "Frame9" );
    Frame9->setGeometry( QRect( -11, -6, 230, 310 ) ); 
    Frame9->setFrameShape( QFrame::StyledPanel );
    Frame9->setFrameShadow( QFrame::Raised );

    TextLabel6 = new QLabel( Frame9, "TextLabel6" );
    TextLabel6->setGeometry( QRect( 20, 10, 100, 70 ) ); 
    TextLabel6->setText( tr( "How many\n"
"tasks should \n"
"be shown?" ) );

    SpinBox2 = new QSpinBox( Frame9, "SpinBox2" );
    SpinBox2->setGeometry( QRect( 119, 29, 58, 25 ) ); 
    SpinBox2->setMaxValue( 20 );
    SpinBox2->setValue( 5 );
    TabWidget3->insertTab( tab_2, tr( "Tasks" ) );

    tab_3 = new QWidget( TabWidget3, "tab_3" );

    Frame14 = new QFrame( tab_3, "Frame14" );
    Frame14->setGeometry( QRect( -12, -7, 200, 220 ) ); 
    Frame14->setFrameShape( QFrame::StyledPanel );
    Frame14->setFrameShadow( QFrame::Raised );

    TextLabel1 = new QLabel( Frame14, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 20, 30, 100, 30 ) ); 
    TextLabel1->setText( tr( "Clip after how\n"
"many letters" ) );

    SpinBox7 = new QSpinBox( Frame14, "SpinBox7" );
    SpinBox7->setGeometry( QRect( 120, 30, 58, 25 ) ); 
    SpinBox7->setMaxValue( 80 );
    SpinBox7->setValue( 30 );
    TabWidget3->insertTab( tab_3, tr( "All" ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
todayconfig::~todayconfig()
{
    // no need to delete child widgets, Qt does it all for us
}

