/****************************************************************************
** Form implementation generated from reading ui file 'soundsettingsbase.ui'
**
** Created: Sun May 19 12:11:35 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "soundsettingsbase.h"

#include <qcheckbox.h>
#include <qcombobox.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>

static const char* const image0_data[] = { 
"16 16 3 1",
". c None",
"a c #000000",
"# c #ff0000",
"............#...",
"............##..",
"..........#..#..",
".....a..#..#.#..",
"....aa..##.#..#.",
"...aaa...#.##.#.",
"aaaaaa.#.#..#.##",
"aaaaaa.#.##.#.##",
"aaaaaa.#.##.#.##",
"aaaaaa.#.##.#.##",
"aaaaaa.#.#..#.##",
"...aaa...#.##.#.",
"....aa..#..#..#.",
".....a.....#.##.",
"..........#..#..",
"............##.."};

static const char* const image1_data[] = { 
"16 16 3 1",
". c None",
"# c #000000",
"a c #ff0000",
"................",
"................",
"................",
".....#..........",
"....##..........",
"...###...a...a..",
"######..aaa.aaa.",
"######...aaaaa..",
"######....aaa...",
"######...aaaaa..",
"######..aaa.aaa.",
"...###...a...a..",
"....##..........",
".....#..........",
"................",
"................"};


/* 
 *  Constructs a SoundSettingsBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SoundSettingsBase::SoundSettingsBase( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    QPixmap image0( ( const char** ) image0_data );
    QPixmap image1( ( const char** ) image1_data );
    if ( !name )
  setName( "SoundSettingsBase" );
    //    resize(255,301);
    setCaption( tr( "Vmemo Settings" ) );

    SoundSettingsBaseLayout = new QGridLayout( this ); 
    SoundSettingsBaseLayout->setSpacing( 4 );
    SoundSettingsBaseLayout->setMargin( 4 );

    QHBoxLayout *Layout11;
    Layout11 = new QHBoxLayout; 
    Layout11->setSpacing( 4 );
    Layout11->setMargin( 0 );

    stereoCheckBox = new QCheckBox( this, "stereoCheckBox" );
    stereoCheckBox->setText( tr( "Stereo" ) );
    Layout11->addWidget( stereoCheckBox );

    sixteenBitCheckBox = new QCheckBox( this, "sixteenBitCheckBox" );
    sixteenBitCheckBox->setText( tr( "16 bit" ) );
    Layout11->addWidget( sixteenBitCheckBox );

    AlertCheckBox = new QCheckBox( this, "AlertCheckBox" );
    AlertCheckBox->setText( tr( "Visual Alerts" ) );
    Layout11->addWidget( AlertCheckBox );

    SoundSettingsBaseLayout->addLayout( Layout11, 0, 0);
    
    sampleRateLabel = new QLabel(this, "sampleRateLabel" );
    sampleRateLabel->setText( tr( "Sample Rate:" ) );
    SoundSettingsBaseLayout->addMultiCellWidget(  sampleRateLabel  , 1, 1, 0, 0, 1);

    sampleRate = new QComboBox( FALSE,this, "sampleRate" );
    
    sampleRate->insertItem( tr( "8000" ) );
    sampleRate->insertItem( tr( "11025" ) );
    sampleRate->insertItem( tr( "22050" ) );
    sampleRate->insertItem( tr( "33075" ) );
    sampleRate->insertItem( tr( "44100" ) );
    SoundSettingsBaseLayout->addMultiCellWidget(  sampleRate, 2, 2, 0, 0, 1 );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setText( tr( "Recording Directory:" ) );
    SoundSettingsBaseLayout->addMultiCellWidget( TextLabel1, 3, 3, 0, 0, 1);


    LocationComboBox = new QComboBox( FALSE, this, "LocationComboBox" );
    SoundSettingsBaseLayout->addMultiCellWidget(  LocationComboBox, 4, 4, 0, 8, 1);

    QLabel *TextLabelKey;
    TextLabelKey = new QLabel( this, "TextLabelKey" );
    TextLabelKey->setText( tr( "Recording Key:" ) );
    SoundSettingsBaseLayout->addMultiCellWidget(TextLabelKey , 5, 5, 0, 0, 1);

    keyComboBox = new QComboBox( FALSE, this, "keyComboBox" );
    keyComboBox->insertItem( tr( "" ) );
    keyComboBox->insertItem( tr( "Taskbar Icon" ) );
    keyComboBox->insertItem( tr( "Key_Record" ) );
    keyComboBox->insertItem( tr( "Key_Space" ) );
    keyComboBox->insertItem( tr( "Key_Home" ) );
    keyComboBox->insertItem( tr( "Key_Calender" ) );
    keyComboBox->insertItem( tr( "Key_Contacts" ) );
    keyComboBox->insertItem( tr( "Key_Menu" ) );
    keyComboBox->insertItem( tr( "Key_Mail" ) );
    SoundSettingsBaseLayout->addMultiCellWidget( keyComboBox , 6, 6, 0, 0 ,1);

    keyLabel=  new QLabel( this, "keyLabel" );
    SoundSettingsBaseLayout->addMultiCellWidget( keyLabel , 6, 6, 0, 2, 3);

    QLabel *timeLimitLabel;
    timeLimitLabel= new QLabel( this, "timeLimitLabel" );
    timeLimitLabel->setText( tr( "Recording Limit in seconds:" ) );
    SoundSettingsBaseLayout->addMultiCellWidget(  timeLimitLabel  , 7, 7, 0, 0, 1);

    timeLimitComboBox = new QComboBox( FALSE, this, "timeLimitComboBox" );
    timeLimitComboBox->insertItem( tr( "30" ) );
    timeLimitComboBox->insertItem( tr( "20" ) );
    timeLimitComboBox->insertItem( tr( "15" ) );
    timeLimitComboBox->insertItem( tr( "10" ) );
    timeLimitComboBox->insertItem( tr( "5" ) );
    timeLimitComboBox->insertItem( tr( "Unlimited" ) );

    SoundSettingsBaseLayout->addMultiCellWidget(timeLimitComboBox  , 8, 8, 0, 0, 1);


    restartCheckBox= new QCheckBox( this, "restartCheck" );
    restartCheckBox->setText( tr( "Restart Opie if needed" ) );

    SoundSettingsBaseLayout->addMultiCellWidget( restartCheckBox , 9, 9, 0, 0, 1);

    adpcmCheckBox = new QCheckBox( this, "adpcmCheck" );
    adpcmCheckBox->setText( tr( "Use ADPCM compression" ) );

    SoundSettingsBaseLayout->addMultiCellWidget( adpcmCheckBox , 10, 10, 0, 0, 1);
    
    QSpacerItem* spacer4 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding );
    SoundSettingsBaseLayout->addItem( spacer4, 10, 0 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
SoundSettingsBase::~SoundSettingsBase()
{
    // no need to delete child widgets, Qt does it all for us
}
