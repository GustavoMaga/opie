/*
 * Copyright (c) 2002 Michael R. Crawford <mike@tuxnami.org>
 * Copyright (C) 2002 by Stefan Eilers (eilers.stefan@epost.de)
 *
 * This file is an add-on for the OPIE Palmtop Environment
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the pacakaging
 * of this file.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 * This is a rewrite of the abeditor.h file, modified to provide a more
 * intuitive interface to TrollTech's original Address Book editor.  This
 * is made to operate exactly in interface with the exception of name.
 *
 */

#include "contacteditor.h"

#include <qpe/categoryselect.h>
#include <qpe/qpeapplication.h>
#include <qpe/qpedialog.h>
#include <opie/ocontact.h>
#include <qpe/resource.h>

#include <qlabel.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qtoolbutton.h>
#include <qlistbox.h> 
#include <qmessagebox.h> 
#include <qwhatsthis.h>

#include <assert.h>

static inline bool containsAlphaNum( const QString &str );
static inline bool constainsWhiteSpace( const QString &str );

// helper functions, convert our comma delimited list to proper
// file format...
void parseEmailFrom( const QString &txt, QString &strDefaultEmail,
		     QString &strAll );

// helper convert from file format to comma delimited...
void parseEmailTo( const QString &strDefaultEmail,
		   const QString &strOtherEmail, QString &strBack );

ContactEditor::ContactEditor(	const OContact &entry,
				QWidget *parent,
				const char *name,
				WFlags )
	: QDialog( parent, name, TRUE, WStyle_ContextHelp ),
	  defaultEmailChooserPosition( -1 ),
	  m_personalView ( false ),
	  cmbDefaultEmail( 0 ),
	  initializing ( false )
{

	initializing = true;

	init();
	setEntry( entry );
// 	cmbDefaultEmail = 0;
// 	defaultEmailChooserPosition = -1;

	initializing = false;
}

ContactEditor::~ContactEditor() {
}

void ContactEditor::init() {
	qWarning("init() START");
	
	uint i = 0;

	QStringList trlChooserNames;

	for (i = 0; i <= 6; i++) {
		slHomeAddress.append( "" );
		slBusinessAddress.append( "" );
	}

	trlChooserNames   = OContactFields::trphonefields( false );
	slChooserNames    = OContactFields::untrphonefields( false );
	slDynamicEntries  = OContactFields::untrdetailsfields( false );
	trlDynamicEntries = OContactFields::trdetailsfields( false );

	// Ok, we have to remove elements from the list of dynamic entries
	// which are now stored in special (not dynamic) widgets..
	// Otherwise we will get problems with field assignments! (se)
	slDynamicEntries.remove("Anniversary");
	slDynamicEntries.remove("Birthday");
	slDynamicEntries.remove("Gender");

	// The same with translated fields.. But I will
	// use the translation map to avoid mismatches..
	QMap<int, QString> translMap = OContactFields::idToTrFields();
	trlDynamicEntries.remove( translMap[Qtopia::Anniversary] );
	trlDynamicEntries.remove( translMap[Qtopia::Birthday] );
	trlDynamicEntries.remove( translMap[Qtopia::Gender] );

	// Last Check to be sure..
	assert( slDynamicEntries.count() == trlDynamicEntries.count() );
	assert( slChooserNames.count() == trlChooserNames.count() );

	for (i = 0; i < slChooserNames.count(); i++)
	  slChooserValues.append("");


	QVBoxLayout *vb = new QVBoxLayout( this );

	tabMain = new QTabWidget( this );
	vb->addWidget( tabMain );

	QWidget *tabViewport = new QWidget ( tabMain );

	vb = new QVBoxLayout( tabViewport );

	svGeneral = new QScrollView( tabViewport );
	vb->addWidget( svGeneral, 0, 0 );
	svGeneral->setResizePolicy( QScrollView::AutoOneFit );
	// svGeneral->setHScrollBarMode( QScrollView::AlwaysOff );
	// svGeneral->setVScrollBarMode( QScrollView::AlwaysOff );
	svGeneral->setFrameStyle( QFrame::NoFrame );

	QWidget *container = new QWidget( svGeneral->viewport() );
	svGeneral->addChild( container );

	QGridLayout *gl = new QGridLayout( container, 1, 1, 2, 4 );
	gl->setResizeMode( QLayout::FreeResize );

	btnFullName = new QPushButton( tr( "Full Name..." ), container );
	QWhatsThis::add( btnFullName, tr( "Press to enter last- middle and firstname" ) );
	gl->addWidget( btnFullName, 0, 0 );
	txtFullName = new QLineEdit( container );
	QWhatsThis::add( txtFullName, tr( "Enter fullname directly ! If you have a lastname with multiple words ( for instance \"de la Guerra\"), please write <lastname>,<firstnames> like this: \"de la Guerra, Carlos Pedro\"" ) );
	gl->addWidget( txtFullName, 0, 1 );

	QLabel *l = new QLabel( tr( "Job Title" ), container );
	QWhatsThis::add( l, tr( "The jobtitle.." ) );
	gl->addWidget( l, 1, 0 );
	txtJobTitle = new QLineEdit( container );
	QWhatsThis::add( txtJobTitle, tr( "The jobtitle.." ) );
	gl->addWidget( txtJobTitle, 1, 1 );

	l = new QLabel( tr("Suffix"), container );
	QWhatsThis::add( l, tr( "Something like \"jr.\".." ) );
	gl->addWidget( l, 2, 0 );
	txtSuffix = new QLineEdit( container );
	QWhatsThis::add( txtSuffix, tr( "Something like \"jr.\".." ) );
	gl->addWidget( txtSuffix, 2, 1 );

	l = new QLabel( tr( "Organization" ), container );
	QWhatsThis::add( l, tr( "The working place of the contact" ) );
	gl->addWidget( l, 3, 0 );
	txtOrganization = new QLineEdit( container );
	QWhatsThis::add( txtOrganization, tr( "The working place of the contact" ) );
	gl->addWidget( txtOrganization, 3, 1 );

	// Chooser 1 
	cmbChooserField1 = new QComboBox( FALSE, container );
	QWhatsThis::add( cmbChooserField1, tr( "Press to select attribute to change" ) );
	cmbChooserField1->setMaximumWidth( 90 );
	gl->addWidget( cmbChooserField1, 4, 0 );
	// Textfield for chooser 1.
	// Now use Widgetstack to contain the textfield and the default-email combo !
	m_widgetStack1 = new QWidgetStack( container );
	txtChooserField1 = new QLineEdit( m_widgetStack1 );
	m_widgetStack1 -> addWidget( txtChooserField1, TextField );
	gl->addWidget( m_widgetStack1, 4, 1 );
	m_widgetStack1 -> raiseWidget( TextField );

	// Chooser 2
	cmbChooserField2 = new QComboBox( FALSE, container );
	QWhatsThis::add( cmbChooserField2, tr( "Press to select attribute to change" ) );
	cmbChooserField2->setMaximumWidth( 90 );
	gl->addWidget( cmbChooserField2, 5, 0 );
	// Textfield for chooser 2
	// Now use WidgetStack to contain the textfield and the default-email combo!
	m_widgetStack2 = new QWidgetStack( container );
	txtChooserField2 = new QLineEdit( m_widgetStack2 );
	m_widgetStack2 -> addWidget( txtChooserField2, TextField );
	gl->addWidget( m_widgetStack2, 5, 1 );
	m_widgetStack2 -> raiseWidget( TextField );

	// Chooser 3
	cmbChooserField3 = new QComboBox( FALSE, container );
	QWhatsThis::add( cmbChooserField3, tr( "Press to select attribute to change" ) );
	cmbChooserField3->setMaximumWidth( 90 );
	gl->addWidget( cmbChooserField3, 6, 0 );
	// Textfield for chooser 2
	// Now use WidgetStack to contain the textfield and the default-email combo!
	m_widgetStack3 = new QWidgetStack( container );
	txtChooserField3 = new QLineEdit( m_widgetStack3 );
	m_widgetStack3 -> addWidget( txtChooserField3, TextField );
	gl->addWidget( m_widgetStack3, 6, 1 );
	m_widgetStack3 -> raiseWidget( TextField );

	l = new QLabel( tr( "File As" ), container );
	QWhatsThis::add( l, tr( "Press to select how to store the name (and howto show it in the listview)" ) );
	gl->addWidget( l, 7, 0 );
	cmbFileAs = new QComboBox( TRUE, container );
	gl->addWidget( cmbFileAs, 7, 1 );

	labCat = new QLabel( tr( "Category" ), container );
	gl->addWidget( labCat, 8, 0 );
	cmbCat = new CategorySelect( container );
	gl->addWidget( cmbCat, 8, 1 );
	labCat->show();
	cmbCat->show();

	btnNote = new QPushButton( tr( "Notes..." ), container );
	gl->addWidget( btnNote, 9, 1 );

	tabMain->insertTab( tabViewport, tr( "General" ) );

	tabViewport = new QWidget ( tabMain );

	vb = new QVBoxLayout( tabViewport );

	svAddress = new QScrollView( tabViewport );
	vb->addWidget( svAddress, 0, 0 );
	svAddress->setResizePolicy( QScrollView::AutoOneFit );
	svAddress->setFrameStyle( QFrame::NoFrame );

	container = new QWidget( svAddress->viewport() );
	svAddress->addChild( container );

	gl = new QGridLayout( container, 8, 3, 2, 4 ); // row 7 QSpacerItem

	cmbAddress = new QComboBox( FALSE, container );
	cmbAddress->insertItem( tr( "Business" ) );
	cmbAddress->insertItem( tr( "Home" ) );
	gl->addMultiCellWidget( cmbAddress, 0, 0, 0, 1 );

	l = new QLabel( tr( "Address" ), container );
	gl->addWidget( l, 1, 0 );
	txtAddress = new QLineEdit( container );
	gl->addMultiCellWidget( txtAddress, 1, 1, 1, 2 );

	l = new QLabel( tr( "City" ), container );
	gl->addWidget( l, 2, 0 );
	txtCity = new QLineEdit( container );
	gl->addMultiCellWidget( txtCity, 2, 2, 1, 2 );

	l = new QLabel( tr( "State" ), container );
	gl->addWidget( l, 3, 0 );
	txtState = new QLineEdit( container );
	gl->addMultiCellWidget( txtState, 3, 3, 1, 2 );

	l = new QLabel( tr( "Zip Code" ), container );
	gl->addWidget( l, 4, 0 );
	txtZip = new QLineEdit( container );
	gl->addMultiCellWidget( txtZip, 4, 4, 1, 2 );

	l = new QLabel( tr( "Country" ), container );
	gl->addWidget( l, 5, 0 );
	cmbCountry = new QComboBox( TRUE, container );
	cmbCountry->insertItem( tr( "" ) );
	cmbCountry->insertItem( tr ( "United States" ) );
	cmbCountry->insertItem( tr ( "United Kingdom" ) );
	cmbCountry->insertItem( tr ( "Afghanistan" ) );
	cmbCountry->insertItem( tr ( "Albania" ) );
	cmbCountry->insertItem( tr ( "Algeria" ) );
	cmbCountry->insertItem( tr ( "American Samoa" ) );
	cmbCountry->insertItem( tr ( "Andorra" ) );
	cmbCountry->insertItem( tr ( "Angola" ) );
	cmbCountry->insertItem( tr ( "Anguilla" ) );
	cmbCountry->insertItem( tr ( "Antarctica" ) );
	cmbCountry->insertItem( tr ( "Argentina" ) );
	cmbCountry->insertItem( tr ( "Armenia" ) );
	cmbCountry->insertItem( tr ( "Aruba" ) );
	cmbCountry->insertItem( tr ( "Australia" ) );
	cmbCountry->insertItem( tr ( "Austria" ) );
	cmbCountry->insertItem( tr ( "Azerbaijan" ) );
	cmbCountry->insertItem( tr ( "Bahamas" ) );
	cmbCountry->insertItem( tr ( "Bahrain" ) );
	cmbCountry->insertItem( tr ( "Bangladesh" ) );
	cmbCountry->insertItem( tr ( "Barbados" ) );
	cmbCountry->insertItem( tr ( "Belarus" ) );
	cmbCountry->insertItem( tr ( "Belgium" ) );
	cmbCountry->insertItem( tr ( "Belize" ) );
	cmbCountry->insertItem( tr ( "Benin" ) );
	cmbCountry->insertItem( tr ( "Bermuda" ) );
	cmbCountry->insertItem( tr ( "Bhutan" ) );
	cmbCountry->insertItem( tr ( "Bolivia" ) );
	cmbCountry->insertItem( tr ( "Botswana" ) );
	cmbCountry->insertItem( tr ( "Bouvet Island" ) );
	cmbCountry->insertItem( tr ( "Brazil" ) );
	cmbCountry->insertItem( tr ( "Brunei Darussalam" ) );
	cmbCountry->insertItem( tr ( "Bulgaria" ) );
	cmbCountry->insertItem( tr ( "Burkina Faso" ) );
	cmbCountry->insertItem( tr ( "Burundi" ) );
	cmbCountry->insertItem( tr ( "Cambodia" ) );
	cmbCountry->insertItem( tr ( "Cameroon" ) );
	cmbCountry->insertItem( tr ( "Canada" ) );
	cmbCountry->insertItem( tr ( "Cape Verde" ) );
	cmbCountry->insertItem( tr ( "Cayman Islands" ) );
	cmbCountry->insertItem( tr ( "Chad" ) );
	cmbCountry->insertItem( tr ( "Chile" ) );
	cmbCountry->insertItem( tr ( "China" ) );
	cmbCountry->insertItem( tr ( "Christmas Island" ) );
	cmbCountry->insertItem( tr ( "Colombia" ) );
	cmbCountry->insertItem( tr ( "Comoros" ) );
	cmbCountry->insertItem( tr ( "Congo" ) );
	cmbCountry->insertItem( tr ( "Cook Island" ) );
	cmbCountry->insertItem( tr ( "Costa Rica" ) );
	cmbCountry->insertItem( tr ( "Cote d'Ivoire" ) );
	cmbCountry->insertItem( tr ( "Croatia" ) );
	cmbCountry->insertItem( tr ( "Cuba" ) );
	cmbCountry->insertItem( tr ( "Cyprus" ) );
	cmbCountry->insertItem( tr ( "Czech Republic" ) );
	cmbCountry->insertItem( tr ( "Denmark" ) );
	cmbCountry->insertItem( tr ( "Djibouti" ) );
	cmbCountry->insertItem( tr ( "Dominica" ) );
	cmbCountry->insertItem( tr ( "Dominican Republic" ) );
	cmbCountry->insertItem( tr ( "East Timor" ) );
	cmbCountry->insertItem( tr ( "Ecuador" ) );
	cmbCountry->insertItem( tr ( "Egypt" ) );
	cmbCountry->insertItem( tr ( "El Salvador" ) );
	cmbCountry->insertItem( tr ( "Equatorial Guinea" ) );
	cmbCountry->insertItem( tr ( "Eritrea" ) );
	cmbCountry->insertItem( tr ( "Estonia" ) );
	cmbCountry->insertItem( tr ( "Ethiopia" ) );
	cmbCountry->insertItem( tr ( "Falkland Islands" ) );
	cmbCountry->insertItem( tr ( "Faroe Islands" ) );
	cmbCountry->insertItem( tr ( "Fiji" ) );
	cmbCountry->insertItem( tr ( "Finland" ) );
	cmbCountry->insertItem( tr ( "France" ) );
	cmbCountry->insertItem( tr ( "French Guiana" ) );
	cmbCountry->insertItem( tr ( "French Polynesia" ) );
	cmbCountry->insertItem( tr ( "Gabon" ) );
	cmbCountry->insertItem( tr ( "Gambia" ) );
	cmbCountry->insertItem( tr ( "Georgia" ) );
	cmbCountry->insertItem( tr ( "Germany" ) );
	cmbCountry->insertItem( tr ( "Ghana" ) );
	cmbCountry->insertItem( tr ( "Gibraltar" ) );
	cmbCountry->insertItem( tr ( "Greece" ) );
	cmbCountry->insertItem( tr ( "Greenland" ) );
	cmbCountry->insertItem( tr ( "Grenada" ) );
	cmbCountry->insertItem( tr ( "Guadeloupe" ) );
	cmbCountry->insertItem( tr ( "Guam" ) );
	cmbCountry->insertItem( tr ( "Guatemala" ) );
	cmbCountry->insertItem( tr ( "Guinea" ) );
	cmbCountry->insertItem( tr ( "Guinea-Bissau" ) );
	cmbCountry->insertItem( tr ( "Guyana" ) );
	cmbCountry->insertItem( tr ( "Haiti" ) );
	cmbCountry->insertItem( tr ( "Holy See" ) );
	cmbCountry->insertItem( tr ( "Honduras" ) );
	cmbCountry->insertItem( tr ( "Hong Kong" ) );
	cmbCountry->insertItem( tr ( "Hungary" ) );
	cmbCountry->insertItem( tr ( "Iceland" ) );
	cmbCountry->insertItem( tr ( "India" ) );
	cmbCountry->insertItem( tr ( "Indonesia" ) );
	cmbCountry->insertItem( tr ( "Ireland" ) );
	cmbCountry->insertItem( tr ( "Israel" ) );
	cmbCountry->insertItem( tr ( "Italy" ) );
	cmbCountry->insertItem( tr ( "Jamaica" ) );
	cmbCountry->insertItem( tr ( "Japan" ) );
	cmbCountry->insertItem( tr ( "Jordan" ) );
	cmbCountry->insertItem( tr ( "Kazakhstan" ) );
	cmbCountry->insertItem( tr ( "Kenya" ) );
	cmbCountry->insertItem( tr ( "Kiribati" ) );
	cmbCountry->insertItem( tr ( "Korea" ) );
	cmbCountry->insertItem( tr ( "Kuwait" ) );
	cmbCountry->insertItem( tr ( "Kyrgyzstan" ) );
	cmbCountry->insertItem( tr ( "Laos" ) );
	cmbCountry->insertItem( tr ( "Latvia" ) );
	cmbCountry->insertItem( tr ( "Lebanon" ) );
	cmbCountry->insertItem( tr ( "Lesotho" ) );
	cmbCountry->insertItem( tr ( "Liberia" ) );
	cmbCountry->insertItem( tr ( "Liechtenstein" ) );
	cmbCountry->insertItem( tr ( "Lithuania" ) );
	cmbCountry->insertItem( tr ( "Luxembourg" ) );
	cmbCountry->insertItem( tr ( "Macau" ) );
	cmbCountry->insertItem( tr ( "Macedonia" ) );
	cmbCountry->insertItem( tr ( "Madagascar" ) );
	cmbCountry->insertItem( tr ( "Malawi" ) );
	cmbCountry->insertItem( tr ( "Malaysia" ) );
	cmbCountry->insertItem( tr ( "Maldives" ) );
	cmbCountry->insertItem( tr ( "Mali" ) );
	cmbCountry->insertItem( tr ( "Malta" ) );
	cmbCountry->insertItem( tr ( "Martinique" ) );
	cmbCountry->insertItem( tr ( "Mauritania" ) );
	cmbCountry->insertItem( tr ( "Mauritius" ) );
	cmbCountry->insertItem( tr ( "Mayotte" ) );
	cmbCountry->insertItem( tr ( "Mexico" ) );
	cmbCountry->insertItem( tr ( "Micronesia" ) );
	cmbCountry->insertItem( tr ( "Moldova" ) );
	cmbCountry->insertItem( tr ( "Monaco" ) );
	cmbCountry->insertItem( tr ( "Mongolia" ) );
	cmbCountry->insertItem( tr ( "Montserrat" ) );
	cmbCountry->insertItem( tr ( "Morocco" ) );
	cmbCountry->insertItem( tr ( "Mozambique" ) );
	cmbCountry->insertItem( tr ( "Myanmar" ) );
	cmbCountry->insertItem( tr ( "Namibia" ) );
	cmbCountry->insertItem( tr ( "Nauru" ) );
	cmbCountry->insertItem( tr ( "Nepal" ) );
	cmbCountry->insertItem( tr ( "Netherlands" ) );
	cmbCountry->insertItem( tr ( "New Caledonia" ) );
	cmbCountry->insertItem( tr ( "New Zealand" ) );
	cmbCountry->insertItem( tr ( "Nicaragua" ) );
	cmbCountry->insertItem( tr ( "Niger" ) );
	cmbCountry->insertItem( tr ( "Nigeria" ) );
	cmbCountry->insertItem( tr ( "Niue" ) );
	cmbCountry->insertItem( tr ( "Norway" ) );
	cmbCountry->insertItem( tr ( "Oman" ) );
	cmbCountry->insertItem( tr ( "Pakistan" ) );
	cmbCountry->insertItem( tr ( "Palau" ) );
	cmbCountry->insertItem( tr ( "Palestinian Sovereign Areas" ) );
	cmbCountry->insertItem( tr ( "Panama" ) );
	cmbCountry->insertItem( tr ( "Papua New Guinea" ) );
	cmbCountry->insertItem( tr ( "Paraguay" ) );
	cmbCountry->insertItem( tr ( "Peru" ) );
	cmbCountry->insertItem( tr ( "Philippines" ) );
	cmbCountry->insertItem( tr ( "Pitcairn Islands" ) );
	cmbCountry->insertItem( tr ( "Poland" ) );
	cmbCountry->insertItem( tr ( "Portugal" ) );
	cmbCountry->insertItem( tr ( "Puerto Rico" ) );
	cmbCountry->insertItem( tr ( "Qatar" ) );
	cmbCountry->insertItem( tr ( "Reunion" ) );
	cmbCountry->insertItem( tr ( "Romania" ) );
	cmbCountry->insertItem( tr ( "Russia" ) );
	cmbCountry->insertItem( tr ( "Rwanda" ) );
	cmbCountry->insertItem( tr ( "Saint Lucia" ) );
	cmbCountry->insertItem( tr ( "Samoa" ) );
	cmbCountry->insertItem( tr ( "San Marino" ) );
	cmbCountry->insertItem( tr ( "Saudi Arabia" ) );
	cmbCountry->insertItem( tr ( "Senegal" ) );
	cmbCountry->insertItem( tr ( "Seychelles" ) );
	cmbCountry->insertItem( tr ( "Sierra Leone" ) );
	cmbCountry->insertItem( tr ( "Singapore" ) );
	cmbCountry->insertItem( tr ( "Slovakia" ) );
	cmbCountry->insertItem( tr ( "Slovenia" ) );
	cmbCountry->insertItem( tr ( "Solomon Islands" ) );
	cmbCountry->insertItem( tr ( "Somalia" ) );
	cmbCountry->insertItem( tr ( "South Africa" ) );
	cmbCountry->insertItem( tr ( "Spain" ) );
	cmbCountry->insertItem( tr ( "Sri Lanka" ) );
	cmbCountry->insertItem( tr ( "St. Helena" ) );
	cmbCountry->insertItem( tr ( "Sudan" ) );
	cmbCountry->insertItem( tr ( "Suriname" ) );
	cmbCountry->insertItem( tr ( "Swaziland" ) );
	cmbCountry->insertItem( tr ( "Sweden" ) );
	cmbCountry->insertItem( tr ( "Switzerland" ) );
	cmbCountry->insertItem( tr ( "Taiwan" ) );
	cmbCountry->insertItem( tr ( "Tajikistan" ) );
	cmbCountry->insertItem( tr ( "Tanzania" ) );
	cmbCountry->insertItem( tr ( "Thailand" ) );
	cmbCountry->insertItem( tr ( "Togo" ) );
	cmbCountry->insertItem( tr ( "Tokelau" ) );
	cmbCountry->insertItem( tr ( "Tonga" ) );
	cmbCountry->insertItem( tr ( "Tunisia" ) );
	cmbCountry->insertItem( tr ( "Turkey" ) );
	cmbCountry->insertItem( tr ( "Turkmenistan" ) );
	cmbCountry->insertItem( tr ( "Tuvalu" ) );
	cmbCountry->insertItem( tr ( "Uganda" ) );
	cmbCountry->insertItem( tr ( "Ukraine" ) );
	cmbCountry->insertItem( tr ( "Uruguay" ) );
	cmbCountry->insertItem( tr ( "Uzbekistan" ) );
	cmbCountry->insertItem( tr ( "Vanuatu" ) );
	cmbCountry->insertItem( tr ( "Venezuela" ) );
	cmbCountry->insertItem( tr ( "Vietnam" ) );
	cmbCountry->insertItem( tr ( "Virgin Islands" ) );
	cmbCountry->insertItem( tr ( "Western Sahara" ) );
	cmbCountry->insertItem( tr ( "Yemen" ) );
	cmbCountry->insertItem( tr ( "Yugoslavia" ) );
	cmbCountry->insertItem( tr ( "Zambia" ) );
	cmbCountry->insertItem( tr ( "Zimbabwe" ) );
	if (cmbCountry->listBox()!=0)
 	  cmbCountry->listBox()->sort();

	cmbCountry->setMaximumWidth( 135 );

	gl->addMultiCellWidget( cmbCountry, 5, 5, 1, 2 );

	// Chooser 4
	cmbChooserField4 = new QComboBox( FALSE, container );
	cmbChooserField4->setMaximumWidth( 90 );
	gl->addWidget( cmbChooserField4, 6, 0 );
	// Textfield for chooser 2
	// Now use WidgetStack to contain the textfield and the default-email combo!
	m_widgetStack4 = new QWidgetStack( container );
	txtChooserField4 = new QLineEdit( m_widgetStack4 );
	m_widgetStack4 -> addWidget( txtChooserField4, TextField );
	gl->addMultiCellWidget( m_widgetStack4, 6, 6, 1, 2 );
	m_widgetStack4 -> raiseWidget( TextField );

	QSpacerItem *space = new QSpacerItem(1,1,
                                             QSizePolicy::Maximum,
                                             QSizePolicy::MinimumExpanding );
	gl->addItem( space, 7, 0 );

	tabMain->insertTab( tabViewport, tr( "Address" ) );

	tabViewport = new QWidget ( tabMain );

	vb = new QVBoxLayout( tabViewport );

	svDetails = new QScrollView( tabViewport );
	vb->addWidget( svDetails, 0, 0 );
	svDetails->setResizePolicy( QScrollView::AutoOneFit );
	svDetails->setFrameStyle( QFrame::NoFrame );

	container = new QWidget( svDetails->viewport() );
	svDetails->addChild( container );

	gl = new QGridLayout( container, 1, 2, 2, 4 );

	int counter = 0;

	// Birthday
	QHBox* hBox = new QHBox( container );
	l = new QLabel( tr("Birthday"), container );
	gl->addWidget( l, counter, 0 );

 	QPopupMenu* m1 = new QPopupMenu( container );
 	birthdayPicker = new DateBookMonth( m1, 0, TRUE );
 	m1->insertItem( birthdayPicker );

	birthdayButton= new QToolButton( hBox, "buttonStart" );
 	birthdayButton->setPopup( m1 );
	birthdayButton->setPopupDelay(0);

	QPushButton* deleteButton = new QPushButton( QIconSet( Resource::loadPixmap( "trash" ) ), 
						     tr( "Delete" ),
						     hBox, 0 );

	gl->addWidget( hBox, counter , 1  );

	connect( birthdayPicker, SIGNAL( dateClicked( int, int, int ) ),
		 this, SLOT( slotBirthdayDateChanged( int, int, int ) ) );
	connect( deleteButton, SIGNAL( clicked() ), this, SLOT( slotRemoveBirthday() ) );

	++counter;

	// Anniversary
	hBox = new QHBox( container );
	l = new QLabel( tr("Anniversary"), container );
	gl->addWidget( l, counter, 0 );

 	m1 = new QPopupMenu( container );
 	anniversaryPicker = new DateBookMonth( m1, 0, TRUE );
 	m1->insertItem( anniversaryPicker );

	anniversaryButton= new QToolButton( hBox, "buttonStart" );
 	anniversaryButton->setPopup( m1 );
	anniversaryButton->setPopupDelay(0);

	deleteButton = new QPushButton( QIconSet( Resource::loadPixmap( "trash" ) ), 
						     tr( "Delete" ),
						     hBox, 0 );
	gl->addWidget( hBox, counter , 1  );

	connect( anniversaryPicker, SIGNAL( dateClicked( int, int, int ) ),
		 this, SLOT( slotAnniversaryDateChanged( int, int, int ) ) );
	connect( deleteButton, SIGNAL( clicked() ), this, SLOT( slotRemoveAnniversary() ) );

	++counter;

	// Gender
	l = new QLabel( tr("Gender"), container );
	gl->addWidget( l, counter, 0 );
	cmbGender = new QComboBox( container );
	cmbGender->insertItem( "", 0 );
	cmbGender->insertItem( tr("Male"), 1);
	cmbGender->insertItem( tr("Female"), 2);
	gl->addWidget( cmbGender, counter, 1 );

	++counter;

	// Create Labels and lineedit fields for every dynamic entry
	QStringList::ConstIterator it = slDynamicEntries.begin();
	QMap<QString, int> mapStrToID = OContactFields::untrFieldsToId();
	QMap<int, QString> mapIdToStr = OContactFields::idToTrFields();
	for (i = counter; it != slDynamicEntries.end(); i++, ++it ) {
	  
	  if (((*it) == "Anniversary") || 
	      ((*it) == "Birthday")|| ((*it) == "Gender")) continue;

	  l = new QLabel( mapIdToStr[mapStrToID[*it]], container );
		listName.append( l );
		gl->addWidget( l, i, 0 );
		QLineEdit *e = new QLineEdit( container );
		listValue.append( e );
		gl->addWidget( e, i, 1);
	}
	// Fill labels with names..
	//	loadFields();


	tabMain->insertTab( tabViewport, tr( "Details" ) );

	dlgNote = new QDialog( this, "Note Dialog", TRUE );
	dlgNote->setCaption( tr("Enter Note") );
	QVBoxLayout *vbNote = new QVBoxLayout( dlgNote );
	txtNote = new QMultiLineEdit( dlgNote );
	vbNote->addWidget( txtNote );
	connect( btnNote, SIGNAL(clicked()), this, SLOT(slotNote()) );

	dlgName = new QDialog( this, "Name Dialog", TRUE );
	dlgName->setCaption( tr("Edit Name") );
	gl = new QGridLayout( dlgName, 5, 2, 2, 3 );

	l = new QLabel( tr("First Name"), dlgName );
	gl->addWidget( l, 0, 0 );
	txtFirstName = new QLineEdit( dlgName );
	gl->addWidget( txtFirstName, 0, 1 );

	l = new QLabel( tr("Middle Name"), dlgName );
	gl->addWidget( l, 1, 0 );
	txtMiddleName = new QLineEdit( dlgName );
	gl->addWidget( txtMiddleName, 1, 1 );

	l = new QLabel( tr("Last Name"), dlgName );
	gl->addWidget( l, 2, 0 );
	txtLastName = new QLineEdit( dlgName );
	gl->addWidget( txtLastName, 2, 1 );

// 	l = new QLabel( tr("Suffix"), dlgName );
// 	gl->addWidget( l, 3, 0 );
// 	txtSuffix = new QLineEdit( dlgName );
// 	gl->addWidget( txtSuffix, 3, 1 );
        space = new QSpacerItem(1,1,
                                QSizePolicy::Maximum,
                                QSizePolicy::MinimumExpanding );
        gl->addItem( space, 4,  0 );

	cmbChooserField1->insertStringList( trlChooserNames );
	cmbChooserField2->insertStringList( trlChooserNames );
	cmbChooserField3->insertStringList( trlChooserNames );
	cmbChooserField4->insertStringList( trlChooserNames );

 	cmbChooserField1->setCurrentItem( 0 );
 	cmbChooserField2->setCurrentItem( 1 );
 	cmbChooserField3->setCurrentItem( 2 );

	connect( btnFullName, SIGNAL(clicked()), this, SLOT(slotName()) );

	connect( txtFullName, SIGNAL(textChanged(const QString &)), 
		 this, SLOT(slotFullNameChange(const QString &)) );
	connect( txtSuffix, SIGNAL(textChanged(const QString &)), 
		 this, SLOT(slotSuffixChange(const QString &)) );
 	connect( txtOrganization, SIGNAL(textChanged(const QString &)), 
 		 this, SLOT(slotOrganizationChange(const QString &)) );
	connect( txtChooserField1, SIGNAL(textChanged(const QString &)),
                 this, SLOT(slotChooser1Change(const QString &)) );
	connect( txtChooserField2, SIGNAL(textChanged(const QString &)),
                 this, SLOT(slotChooser2Change(const QString &)) );
	connect( txtChooserField3, SIGNAL(textChanged(const QString &)),
                 this, SLOT(slotChooser3Change(const QString &)) );
	connect( txtChooserField4, SIGNAL(textChanged(const QString &)),
                 this, SLOT(slotChooser4Change(const QString &)) );
	connect( txtAddress, SIGNAL(textChanged(const QString &)),
                 this, SLOT(slotAddressChange(const QString &)) );
	connect( txtCity, SIGNAL(textChanged(const QString &)),
                 this, SLOT(slotCityChange(const QString &)) );
	connect( txtState, SIGNAL(textChanged(const QString &)),
                 this, SLOT(slotStateChange(const QString &)) );
	connect( txtZip, SIGNAL(textChanged(const QString &)),
                 this, SLOT(slotZipChange(const QString &)) );
	connect( cmbCountry, SIGNAL(textChanged(const QString &)),
                 this, SLOT(slotCountryChange(const QString &)) );
	connect( cmbCountry, SIGNAL(activated(const QString &)),
                 this, SLOT(slotCountryChange(const QString &)) );
	connect( cmbChooserField1, SIGNAL(activated(int)),
                 this, SLOT(slotCmbChooser1Change(int)) );
	connect( cmbChooserField2, SIGNAL(activated(int)),
                 this, SLOT(slotCmbChooser2Change(int)) );
	connect( cmbChooserField3, SIGNAL(activated(int)),
                 this, SLOT(slotCmbChooser3Change(int)) );
	connect( cmbChooserField4, SIGNAL(activated(int)),
                 this, SLOT(slotCmbChooser4Change(int)) );
	connect( cmbAddress, SIGNAL(activated(int)),
                 this, SLOT(slotAddressTypeChange(int)) );

	new QPEDialogListener(this);

	setPersonalView ( m_personalView );

	qWarning("init() END");
}

void ContactEditor::defaultEmailChanged(int i){
	qDebug("defaultEmailChanged");

	// was sollte das ? (se)
// 	int index = cmbChooserField1->currentItem();
// 	slChooserValues[index] = cmbDefaultEmail->text(i);

	defaultEmail = cmbDefaultEmail->text(i);
	qDebug ("Changed to: %s", defaultEmail.latin1());

}

void ContactEditor::populateDefaultEmailCmb(){       

	// if the default-email combo was not selected and therfore not created
	// we get a lot of trouble.. Therfore create an invisible one..
	if ( !cmbDefaultEmail ){
		cmbDefaultEmail = new QComboBox(this);	
		cmbDefaultEmail -> hide();
	}
	cmbDefaultEmail->clear();    
	cmbDefaultEmail->insertStringList( emails );
	// cmbDefaultEmail->show();

	// Select default email in combo..
	bool found = false;
	for ( int i = 0; i < cmbDefaultEmail->count(); i++){
		qDebug(" populateDefaultEmailCmb text >%s< defaultEmail >%s<",
		       cmbDefaultEmail->text( i ).latin1(), defaultEmail.latin1());

		if ( cmbDefaultEmail->text( i ).stripWhiteSpace() == defaultEmail.stripWhiteSpace() ){
			cmbDefaultEmail->setCurrentItem( i );
			qDebug("set");
			found = true;
		}
	}
	
	// If the current default email is not found in the list, we choose the
	// first one..
	if ( !found )
		defaultEmail = cmbDefaultEmail->text(0);
}

// Called when any combobox was changed.
// "true" returned if the change was chandled by this function, else it should
// be handled by something else..
bool ContactEditor::cmbChooserChange( int index, QWidgetStack* inputStack, int widgetPos ) {
	QString type = slChooserNames[index];
	qWarning("ContactEditor::cmbChooserChange -> Type: %s, WidgetPos: %d", type.latin1(), widgetPos );

	if ( !initializing )
		contactfields.setFieldOrder( widgetPos-1, index );

	// Create and connect combobox for selecting the default email 
        if ( type == "Default Email"){         
		qWarning("Choosing default-email (defaultEmailChooserPosition= %d) ", defaultEmailChooserPosition);
		
		// More than one default-email chooser is not allowed !
		if ( ( defaultEmailChooserPosition != -1 ) && 
		     defaultEmailChooserPosition != widgetPos && !initializing){
			chooserError( widgetPos );
			return true;
		}

		QComboBox* cmbo = ( QComboBox* ) inputStack -> widget( Combo );
		if ( cmbo ){  
			inputStack->raiseWidget( TextField );
			inputStack -> removeWidget( cmbo );
			delete cmbo;
		}
		cmbo = new QComboBox( inputStack );
		cmbo -> insertStringList( emails );

		inputStack -> addWidget( cmbo, Combo );
		inputStack -> raiseWidget( Combo );

		defaultEmailChooserPosition = widgetPos;
		cmbDefaultEmail = cmbo;

		connect( cmbo,SIGNAL( activated(int) ),
			 SLOT( defaultEmailChanged(int) ) ); 

		// Set current default email
		populateDefaultEmailCmb();

		
	} else {
		// Something else was selected: Hide combo..
		qWarning(" Hiding default-email combo" );
		if ( defaultEmailChooserPosition == widgetPos ){
			defaultEmailChooserPosition = -1;
		}
		QComboBox* cmbo = ( QComboBox* ) inputStack -> widget( Combo );
		if ( cmbo ){  
			inputStack->raiseWidget( TextField );
			inputStack -> removeWidget( cmbo );
			cmbDefaultEmail = 0l;
			delete cmbo;
		}

		// Caller should initialize the responsible textfield, therefore
		// "false" is returned
		return false;
	}

	// Everything is worked off ..
	return true;

}

// Currently accessed when we select default-email more than once !
void ContactEditor::chooserError( int index )
{
	qWarning("ContactEditor::chooserError( %d )", index); 
	QMessageBox::warning( this, "Chooser Error",
			      "Multiple selection of this\n"
			      "Item is not allowed !\n\n"
			      "First deselect the previous one !",
			      "&OK", 0, 0,
			      0, 0 );

	// Reset the selected Chooser. Unfortunately the chooser
	// generates no signal, therfore we have to 
	// call the cmbChooserChange function manually..
	switch( index ){
	case 1:
		cmbChooserField1 -> setCurrentItem( 0 );
		slotCmbChooser1Change( 0 );
		break;
	case 2: 
		cmbChooserField2 -> setCurrentItem( 0 );
		slotCmbChooser2Change( 0 );
		break;
	case 3: 
		cmbChooserField3 -> setCurrentItem( 0 );
		slotCmbChooser3Change( 0 );
		break;
	case 4: 
		cmbChooserField4 -> setCurrentItem( 0 );
		slotCmbChooser4Change( 0 );
		break;
	}
}

// Called when something was changed in a textfield (shouldn't it called textchanged? (se))
void ContactEditor::chooserChange( const QString &textChanged, int index, 
				   QLineEdit* , int widgetPos ) {

	QString type = slChooserNames[index]; // :SX
	qDebug("ContactEditor::chooserChange( type=>%s<, textChanged=>%s< index=%i, widgetPos=%i",
	       type.latin1(),textChanged.latin1(), index,  widgetPos );

        if ( type == "Default Email"){         
		qWarning ("??? Wozu??: %s", textChanged.latin1());
		defaultEmail = textChanged;

		populateDefaultEmailCmb();

        }else if (type == "Emails"){
		qDebug("emails");

		QString de;
		emails = QStringList::split (",", textChanged );

		populateDefaultEmailCmb();
	}
	
	slChooserValues[index] = textChanged;

}

void ContactEditor::slotChooser1Change( const QString &textChanged ) {
	qWarning("ContactEditor::slotChooser1Change( %s )", textChanged.latin1());
	chooserChange( textChanged, cmbChooserField1->currentItem(), txtChooserField1, 1);
}

void ContactEditor::slotChooser2Change( const QString &textChanged ) {
	qWarning("ContactEditor::slotChooser2Change( %s )", textChanged.latin1());
	chooserChange( textChanged, cmbChooserField2->currentItem(), txtChooserField2, 2);

}

void ContactEditor::slotChooser3Change( const QString &textChanged ) {
	qWarning("ContactEditor::slotChooser3Change( %s )", textChanged.latin1());
	chooserChange( textChanged, cmbChooserField3->currentItem(), txtChooserField3, 3);
}

void ContactEditor::slotChooser4Change( const QString &textChanged ) {
	qWarning("ContactEditor::slotChooser4Change( %s )", textChanged.latin1());
	chooserChange( textChanged, cmbChooserField4->currentItem(), txtChooserField4, 4);
}

void ContactEditor::slotAddressChange( const QString &textChanged ) {

	if ( cmbAddress->currentItem() == 0 ) {
		slBusinessAddress[0] = textChanged;
	} else {
		slHomeAddress[0] = textChanged;
	}
}

void ContactEditor::slotAddress2Change( const QString &textChanged ) {

	if ( cmbAddress->currentItem() == 0 ) {
		slBusinessAddress[1] = textChanged;
	} else {
		slHomeAddress[1] = textChanged;
	}
}

void ContactEditor::slotPOBoxChange( const QString &textChanged ) {

	if ( cmbAddress->currentItem() == 0 ) {
		slBusinessAddress[2] = textChanged;
	} else {
		slHomeAddress[2] = textChanged;
	}
}

void ContactEditor::slotCityChange( const QString &textChanged ) {

	if ( cmbAddress->currentItem() == 0 ) {
		slBusinessAddress[3] = textChanged;
	} else {
		slHomeAddress[3] = textChanged;
	}
}

void ContactEditor::slotStateChange( const QString &textChanged ) {


	if ( cmbAddress->currentItem() == 0 ) {
		slBusinessAddress[4] = textChanged;
	} else {
		slHomeAddress[4] = textChanged;
	}
}

void ContactEditor::slotZipChange( const QString &textChanged ) {

	if ( cmbAddress->currentItem() == 0 ) {
		slBusinessAddress[5] = textChanged;
	} else {
		slHomeAddress[5] = textChanged;
	}
}

void ContactEditor::slotCountryChange( const QString &textChanged ) {

	if ( cmbAddress->currentItem() == 0 ) {
		slBusinessAddress[6] = textChanged;
	} else {
		slHomeAddress[6] = textChanged;
	}
}


void ContactEditor::slotCmbChooser1Change( int index ) {
	qWarning("ContactEditor::slotCmbChooser1Change( %d )", index);
	if ( !cmbChooserChange( cmbChooserField1->currentItem(), m_widgetStack1, 1) ){
		
		txtChooserField1->setText( slChooserValues[index] );
		txtChooserField1->setFocus();

	}

}

void ContactEditor::slotCmbChooser2Change( int index ) {
	qWarning("ContactEditor::slotCmbChooser2Change( %d )", index);

	if ( !cmbChooserChange( cmbChooserField2->currentItem(), m_widgetStack2, 2) ){

		txtChooserField2->setText( slChooserValues[index] );
		txtChooserField2->setFocus();

	}
}

void ContactEditor::slotCmbChooser3Change( int index ) {
	qWarning("ContactEditor::slotCmbChooser3Change( %d )", index);

	if ( !cmbChooserChange( cmbChooserField3->currentItem(), m_widgetStack3, 3) ){

		txtChooserField3->setText( slChooserValues[index] );
		txtChooserField3->setFocus();

	}
}

void ContactEditor::slotCmbChooser4Change( int index ) {
	qWarning("ContactEditor::slotCmbChooser4Change( %d )", index);

	if ( !cmbChooserChange( cmbChooserField4->currentItem(), m_widgetStack4, 4) ){

		txtChooserField4->setText( slChooserValues[index] );
		txtChooserField4->setFocus();

	}
}

void ContactEditor::slotAddressTypeChange( int index ) {


	if ( !initializing )
		contactfields.setFieldOrder( 4, index );


	if ( index == 0 ) {

		txtAddress->setText( slBusinessAddress[0] );
		//txtAddress2->setText( (*slBusinessAddress)[1] );
		//txtPOBox->setText( (*slBusinessAddress)[2] );
		txtCity->setText( slBusinessAddress[3] );
		txtState->setText( slBusinessAddress[4] );
		txtZip->setText( slBusinessAddress[5] );
		QLineEdit *txtTmp = cmbCountry->lineEdit();
		txtTmp->setText( slBusinessAddress[6] );

	} else {

		txtAddress->setText( slHomeAddress[0] );
		//txtAddress2->setText( (*slHomeAddress)[1] );
		//txtPOBox->setText( (*slHomeAddress)[2] );
		txtCity->setText( slHomeAddress[3] );
		txtState->setText( slHomeAddress[4] );
		txtZip->setText( slHomeAddress[5] );
		QLineEdit *txtTmp = cmbCountry->lineEdit();
		txtTmp->setText( slHomeAddress[6] );

	}

}

void ContactEditor::slotFullNameChange( const QString &textChanged ) {

	qWarning( "ContactEditor::slotFullNameChange( %s )", textChanged.latin1() );

	int index = cmbFileAs->currentItem();

	cmbFileAs->clear();

	cmbFileAs->insertItem( parseName( textChanged, NAME_LF ) );
	cmbFileAs->insertItem( parseName( textChanged, NAME_LFM ) );
	cmbFileAs->insertItem( parseName( textChanged, NAME_FL ) );
	cmbFileAs->insertItem( parseName( textChanged, NAME_FML ) );
	if ( ! txtSuffix->text().isEmpty() )
		cmbFileAs->insertItem( parseName( textChanged, NAME_FML ) + " " + txtSuffix->text() );

	cmbFileAs->setCurrentItem( index );


}

void ContactEditor::slotSuffixChange( const QString& ) {
	// Just want to update the FileAs combo if the suffix was changed..
	slotFullNameChange( txtFullName->text() );
}

void ContactEditor::slotOrganizationChange( const QString &textChanged ){
	qWarning( "ContactEditor::slotOrganizationChange( %s )", textChanged.latin1() );
	// Special handling for storing Companies: 
	// If no Fullname is given, we store the Company-Name as lastname
	// to handle it like a person..
	if ( txtFullName->text() == txtOrganization->text().left( txtFullName->text().length() ) )
		txtFullName->setText( textChanged );
	
}

void ContactEditor::accept() {

	if ( isEmpty() ) {
		cleanupFields();
		reject();
	} else {
		saveEntry();
		cleanupFields();
		QDialog::accept();
	}

}

void ContactEditor::slotNote() {

    if ( ! QPEApplication::execDialog( dlgNote ) ) {
		txtNote->setText( ent.notes() );
	}
}

void ContactEditor::slotName() {

	QString tmpName;

	txtFirstName->setText( parseName(txtFullName->text(), NAME_F) );
	txtMiddleName->setText( parseName(txtFullName->text(), NAME_M) );
	txtLastName->setText( parseName(txtFullName->text(), NAME_L) );
	// txtSuffix->setText( parseName(txtFullName->text(), NAME_S) );

	if ( QPEApplication::execDialog( dlgName ) ) {
		 if ( txtLastName->text().contains( ' ', TRUE ) )
			 tmpName =  txtLastName->text() + ", " + txtFirstName->text() + " " + txtMiddleName->text();
		 else
			 tmpName = txtFirstName->text() + " " + txtMiddleName->text() + " " + txtLastName->text();

		txtFullName->setText( tmpName.simplifyWhiteSpace() );
		slotFullNameChange( txtFullName->text() );
	}

}

void ContactEditor::setNameFocus() {

	txtFullName->setFocus();

}

bool ContactEditor::isEmpty() {
	// Test and see if the record should be saved.
	// More strict than the original qtopia, needs name or fileas to save

	QString t = txtFullName->text();
	if ( !t.isEmpty() && containsAlphaNum( t ) )
		return false;

	t = cmbFileAs->currentText();
	if ( !t.isEmpty() && containsAlphaNum( t ) )
		return false;

	return true;

}

QString ContactEditor::parseName( const QString fullName, int type ) {

	QString simplifiedName( fullName.simplifyWhiteSpace() );
	QString strFirstName;
	QString strMiddleName;
	QString strLastName;
	QString strTitle;
	int commapos;
	bool haveLastName = false;

	qWarning("Fullname: %s", simplifiedName.latin1());

	commapos = simplifiedName.find( ',', 0, TRUE);
	if ( commapos >= 0 ) {
		qWarning(" Commapos: %d", commapos );

		// A comma (",") separates the lastname from one or
		// many first names. Thus, remove the lastname from the 
		// String and parse the firstnames.

		strLastName = simplifiedName.left( commapos );
		simplifiedName= simplifiedName.mid( commapos + 1 );
		haveLastName = true;
		qWarning("Fullname without ',': %s", simplifiedName.latin1());

		// If we have any lastname, we should now split all first names.
		// The first one will be the used as first, the rest as "middle names"

		QStringList allFirstNames = QStringList::split(" ", simplifiedName);
		QStringList::Iterator it = allFirstNames.begin();
		strFirstName = *it++;
		QStringList allSecondNames;
		for ( ; it != allFirstNames.end(); ++it )
			allSecondNames.append( *it );
		
		strMiddleName = allSecondNames.join(" ");

	} else {

		// No comma separator used: We use the first word as firstname, the 
		// last as second/lastname and everything in the middle as middlename

		QStringList allNames = QStringList::split(" ", simplifiedName);
		QStringList::Iterator it = allNames.begin();
		strFirstName = *it++;
		QStringList allSecondNames;
		for ( ; it != --allNames.end(); ++it )
			allSecondNames.append( *it );
		
		strMiddleName = allSecondNames.join(" ");
		strLastName = *(--allNames.end());
		
	}

	if ( strFirstName == strLastName )
		strFirstName = "";

	qWarning(" strFirstName: %s",  strFirstName.latin1());
	qWarning(" strMiddleName: %s",  strMiddleName.latin1());
	qWarning(" strLastName: %s",  strLastName.latin1());
	qWarning(" strTitle: %s",  strTitle.latin1());

	switch (type) {
	case NAME_FL:
		return strFirstName + " " + strLastName;
		
	case NAME_LF:
		return strLastName + ", " + strFirstName;
		
	case NAME_LFM:
		return strLastName + ", " + strFirstName + " " + strMiddleName;
		
	case NAME_FML:
		return strFirstName + " " + strMiddleName + " " + strLastName ;
		
	case NAME_F:
		return strFirstName;
		
	case NAME_M:
		return strMiddleName;
		
	case NAME_L:
		return strLastName;
		
	case NAME_S:
		return txtSuffix->text();
		
	}
	return QString::null;
}

void ContactEditor::cleanupFields() {
	QStringList::Iterator it = slChooserValues.begin();
	
	for ( int i = 0; it != slChooserValues.end(); i++, ++it ) {
		(*it) = "";
	}
	
	for ( int i = 0; i < 7; i++ ) {
		slHomeAddress[i] = "";
		slBusinessAddress[i] = "";
	}
	
	QListIterator<QLineEdit> itLV( listValue );
	for ( ; itLV.current(); ++itLV ) {
		(*itLV)->setText( "" );
	} 
        
	txtFirstName->setText("");
	txtMiddleName->setText("");
	txtLastName->setText("");
	txtSuffix->setText("");
	txtNote->setText("");
	txtFullName->setText("");
	txtJobTitle->setText("");
	txtOrganization->setText("");
	txtChooserField1->setText("");
	txtChooserField2->setText("");
	txtChooserField3->setText("");
	txtAddress->setText("");
	txtCity->setText("");
	txtState->setText("");
	txtZip->setText("");
	QLineEdit *txtTmp = cmbCountry->lineEdit();
	txtTmp->setText("");
	txtTmp = cmbFileAs->lineEdit();
	txtTmp->setText("");
	
}

void ContactEditor::setEntry( const OContact &entry ) {
	
	initializing = true;
	
	// Cleanup and activate the general Page ..
	cleanupFields();
	tabMain->setCurrentPage( 0 );
	
	ent = entry;
	
	emails = QStringList(ent.emailList());
	defaultEmail = ent.defaultEmail();
	if (defaultEmail.isEmpty()) defaultEmail = emails[0];
	qDebug("default email=%s",defaultEmail.latin1());
	
	txtFirstName->setText( ent.firstName() );
	txtMiddleName->setText( ent.middleName() );
	txtLastName->setText( ent.lastName() );
	txtSuffix->setText( ent.suffix() );
	
// 	QString *tmpString = new QString;
// 	*tmpString = ent.firstName() + " " + ent.middleName() +
// 		+ " " + ent.lastName() + " " + ent.suffix();
//	txtFullName->setText( tmpString->simplifyWhiteSpace() );

	if ( !ent.isEmpty() ){
		// Lastnames with multiple words need to be protected by a comma !
		if ( ent.lastName().contains( ' ', TRUE ) )
			txtFullName->setText( ent.lastName() + ", " + ent.firstName() + " " + ent.middleName() );
		else
			txtFullName->setText( ent.firstName() + " " + ent.middleName() + " " + ent.lastName() );
	}
	
	cmbFileAs->setEditText( ent.fileAs() );
	
	//	if (hasTitle)
	txtJobTitle->setText( ent.jobTitle() );
	
	//	if (hasCompany)
	txtOrganization->setText( ent.company() );
	
	//	if (hasNotes)
	txtNote->setText( ent.notes() );
	
	//	if (hasStreet) {
	slHomeAddress[0] = ent.homeStreet();
	slBusinessAddress[0] = ent.businessStreet();
	//	}
	
	//	if (hasCity) {
	slHomeAddress[3] = ent.homeCity();
	slBusinessAddress[3] = ent.businessCity();
	//}
	
	//if (hasState) {
	slHomeAddress[4] = ent.homeState();
	slBusinessAddress[4] = ent.businessState();
	//}
	
	//if (hasZip) {
	slHomeAddress[5] = ent.homeZip();
	slBusinessAddress[5] = ent.businessZip();
	//}
	
	//if (hasCountry) {
	slHomeAddress[6] = ent.homeCountry();
	slBusinessAddress[6] = ent.businessCountry();
	//}
	
	QStringList::ConstIterator it;
	QListIterator<QLineEdit> itLE( listValue );
	for ( it = slDynamicEntries.begin(); itLE.current()/* != slDynamicEntries.end()*/; ++it, ++itLE) {
		
		qWarning(" Filling dynamic Field: %s", (*it).latin1() );
		
		if ( *it ==  "Department"  )
			(*itLE)->setText( ent.department() );
		
		if ( *it == "Company" )
			(*itLE)->setText( ent.company() );
		
		if ( *it ==  "Office" )
			(*itLE)->setText( ent.office() );
		
		if ( *it ==  "Profession" )
			(*itLE)->setText( ent.profession() );
		
		if ( *it == "Assistant" )
			(*itLE)->setText( ent.assistant() );
		
		if ( *it == "Manager" )
			(*itLE)->setText( ent.manager() );
		
		if ( *it == "Spouse" )
			(*itLE)->setText( ent.spouse() );
		
		if ( *it == "Nickname" ){
			qWarning("**** Nichname: %s", ent.nickname().latin1() );
			(*itLE)->setText( ent.nickname() );
		}
		
		if ( *it == "Children" )
			(*itLE)->setText( ent.children() );
		
	}
	
	QStringList::Iterator itV;
	for ( it = slChooserNames.begin(), itV = slChooserValues.begin(); it != slChooserNames.end(); ++it, ++itV ) {
		
		if ( ( *it == "Business Phone") || ( *it == "Work Phone" ) )
			*itV = ent.businessPhone();
		/*
		  if ( *it == "Business 2 Phone" )
		  *itV = ent.business2Phone();
		  */
		if ( ( *it == "Business Fax") || ( *it == "Work Fax" ) )
			*itV = ent.businessFax();
		
		if ( ( *it == "Business Mobile" ) || ( *it == "work Mobile" ) )
			*itV = ent.businessMobile();
		/*
		  if ( *it == "Company Phone" )
		  *itV = ent.companyPhone();
		  */
		if ( *it == "Default Email" )
			*itV = ent.defaultEmail();
		
		if ( *it == "Emails" )
			*itV = ent.emailList().join(", ");  // :SX
		
		if ( *it == "Home Phone" )
			*itV = ent.homePhone();
		/*
		  if ( *it == "Home 2 Phone" )
		  *itV = ent.home2Phone();
		  */
		if ( *it == "Home Fax" )
			*itV = ent.homeFax();
		
		if ( *it == "Home Mobile" )
			*itV = ent.homeMobile();
		/*
		  if ( *it == "Car Phone" )
		  *itV = ent.carPhone();
		  
		  if ( *it == "ISDN Phone" )
		  *itV = ent.ISDNPhone();
		  
		  if ( *it == "Other Phone" )
		  *itV = ent.otherPhone();
		  */
		if ( ( *it == "Business Pager" ) || ( *it == "Work Pager" ) )
			*itV = ent.businessPager();
		/*
		  if ( *it == "Home Pager")
		  *itV = ent.homePager();
		  
		  if ( *it == "AIM IM" )
		  *itV = ent.AIMIM();
		  
		  if ( *it == "ICQ IM" )
		  *itV = ent.ICQIM();
		  
		  if ( *it == "Jabber IM" )
		  *itV = ent.jabberIM();
		  
		  if ( *it == "MSN IM" )
		  *itV = ent.MSNIM();
		  
		  if ( *it == "Yahoo IM" )
		  *itV = ent.yahooIM();
		  */
		if ( *it == "Home Web Page" )
			*itV = ent.homeWebpage();
		
		if ( ( *it == "Business WebPage" ) || ( *it == "Work Web Page" ) )
			*itV = ent.businessWebpage();
		
		
	}
	
	
	cmbCat->setCategories( ent.categories(), "Contacts", tr("Contacts") );
	
	QString gender = ent.gender();
	cmbGender->setCurrentItem( gender.toInt() );
	
	txtNote->setText( ent.notes() );
	
	slotAddressTypeChange( cmbAddress->currentItem() );
	
	// Get combo-settings from contact and set preset..
	contactfields.loadFromRecord( ent );
	cmbChooserField1->setCurrentItem( contactfields.getFieldOrder(0, 7) );
	cmbChooserField2->setCurrentItem( contactfields.getFieldOrder(1, 9) );
	cmbChooserField3->setCurrentItem( contactfields.getFieldOrder(2, 0) );
	cmbChooserField4->setCurrentItem( contactfields.getFieldOrder(3, 6) );
	cmbAddress->setCurrentItem( contactfields.getFieldOrder(4, 1) );
	slotCmbChooser1Change( cmbChooserField1->currentItem() );
	slotCmbChooser2Change( cmbChooserField2->currentItem() );
	slotCmbChooser3Change( cmbChooserField3->currentItem() );
	slotCmbChooser4Change( cmbChooserField4->currentItem() );
	slotAddressTypeChange( cmbAddress->currentItem() );
	
	updateDatePicker();
	
	initializing = false;
}
void ContactEditor::updateDatePicker()
{
	// Set DatePicker
	if ( !ent.birthday().isNull() ){
		birthdayButton->setText( TimeString::numberDateString( ent.birthday() ) );
		birthdayPicker->setDate( ent.birthday() );
	} else
		birthdayButton->setText( tr ("Unknown") );
	
	if ( !ent.anniversary().isNull() ){
		anniversaryButton->setText( TimeString::numberDateString( ent.anniversary() ) );
		anniversaryPicker->setDate( ent.anniversary() );
	} else
		anniversaryButton->setText( tr ("Unknown") );
	
}

void ContactEditor::saveEntry() {
	
	// Store current combo into contact
	contactfields.saveToRecord( ent );
	
	txtFirstName->setText( parseName( txtFullName->text(), NAME_F ) );
	txtMiddleName->setText( parseName( txtFullName->text(), NAME_M ) );
	txtLastName->setText( parseName( txtFullName->text(), NAME_L ) );
	// txtSuffix->setText( parseName( txtFullName->text(), NAME_S ) );
	
	ent.setFirstName( txtFirstName->text() );
	ent.setLastName( txtLastName->text() );
	ent.setMiddleName( txtMiddleName->text() );
	ent.setSuffix( txtSuffix->text() );
	
	ent.setFileAs( cmbFileAs->currentText() );
	
	ent.setCategories( cmbCat->currentCategories() );
	
	
	//if (hasTitle)
	ent.setJobTitle( txtJobTitle->text() );
	
	//if (hasCompany)
	ent.setCompany( txtOrganization->text() );
	
	//	if (hasNotes)
	ent.setNotes( txtNote->text() );
	
	//if (hasStreet) {
	ent.setHomeStreet( slHomeAddress[0] );
	ent.setBusinessStreet( slBusinessAddress[0] );
	//	}
	
	//	if (hasCity) {
	ent.setHomeCity( slHomeAddress[3] );
	ent.setBusinessCity( slBusinessAddress[3] );
	//	}
	
	//	if (hasState) {
	ent.setHomeState( slHomeAddress[4] );
	ent.setBusinessState( slBusinessAddress[4] );
	//	}
	
	//	if (hasZip) {
	ent.setHomeZip( slHomeAddress[5] );
	ent.setBusinessZip( slBusinessAddress[5] );
	//	}
	
	//	if (hasCountry) {
	ent.setHomeCountry( slHomeAddress[6] );
	ent.setBusinessCountry( slBusinessAddress[6] );
	//	}
	
	QStringList::ConstIterator it;
	QListIterator<QLineEdit> itLE( listValue );
	for ( it = slDynamicEntries.begin(); itLE.current() && it != slDynamicEntries.end(); ++it, ++itLE) {
		
		if ( *it == "Department" )
			ent.setDepartment( (*itLE)->text() );
		
		if ( *it == "Company" )
			ent.setCompany( (*itLE)->text() );
		
		if ( *it == "Office" )
			ent.setOffice( (*itLE)->text() );
		
		if ( *it == "Profession" )
			ent.setProfession( (*itLE)->text() );
		
		if ( *it == "Assistant" )
			ent.setAssistant( (*itLE)->text() );
		
		if ( *it == "Manager" )
			ent.setManager( (*itLE)->text() );
		
		if ( *it == "Spouse" )
			ent.setSpouse( (*itLE)->text() );
		
		if ( *it == "Nickname" )
			ent.setNickname( (*itLE)->text() );
		
		if ( *it == "Children" )
			ent.setChildren( (*itLE)->text() );
		
	}
	
	
	QStringList::ConstIterator itV;
	for ( it = slChooserNames.begin(), itV = slChooserValues.begin(); it != slChooserNames.end(); ++it, ++itV ) {
		
		if ( ( *it == "Business Phone" ) || ( *it == "Work Phone"  ) )
			ent.setBusinessPhone( *itV );
		
		if ( ( *it == "Business Fax" ) || ( *it == "Work Fax" ) )
			ent.setBusinessFax( *itV );
		
		if ( ( *it == "Business Mobile" ) || ( *it == "Work Mobile" ) )
			ent.setBusinessMobile( *itV );
		
		if ( *it == "Emails" ){
 			QString allemail;
 			QString defaultmail;
			parseEmailFrom( emails.join(","), defaultmail, allemail );
			if ( defaultEmail.isEmpty() ){
				qWarning("Default email was not set by user!");
				qWarning("Using first email in list: %s", defaultmail.latin1());
				ent.setDefaultEmail( defaultmail );
			}
 			ent.setEmails( allemail );
		}
		
		if ( *it == "Default Email")
			ent.setDefaultEmail( defaultEmail /* *itV */ );
		
		if ( *it == "Home Phone" )
			ent.setHomePhone( *itV );
		
		if ( *it == "Home Fax" )
			ent.setHomeFax( *itV );
		
		if ( *it == "Home Mobile" )
			ent.setHomeMobile( *itV );
		
		if ( ( *it == "Business Pager" ) || ( *it == "Work Pager" ) )
			ent.setBusinessPager( *itV );
		
		if ( *it == "Home Web Page" )
			ent.setHomeWebpage( *itV );
		
		if ( ( *it == "Business WebPage" ) || ( *it == "Work Web Page" ) )
			ent.setBusinessWebpage( *itV );
		
		
	}
	
	int gender = cmbGender->currentItem();
	ent.setGender( QString::number( gender ) );
	
	QString str = txtNote->text();
	if ( !str.isNull() )
		ent.setNotes( str );
	
}

void parseEmailFrom( const QString &txt, QString &strDefaultEmail,
		     QString &strAll )
{
	int where,
		start;
	if ( txt.isEmpty() )
		return;
	// find the first
	where = txt.find( ',' );
	if ( where < 0 ) {
		strDefaultEmail = txt;
		strAll = txt;
	} else {
		strDefaultEmail = txt.left( where ).stripWhiteSpace();
		strAll = strDefaultEmail;
		while ( where > -1 ) {
			strAll.append(" ");
			start = where;
			where = txt.find( ',', where + 1 );
			if ( where > - 1 )
				strAll.append( txt.mid(start + 1, where - start - 1).stripWhiteSpace() );
			else // grab until the end...
				strAll.append( txt.right(txt.length() - start - 1).stripWhiteSpace() );
		}
	}
}

void parseEmailTo( const QString &strDefaultEmail,
		   const QString &strOtherEmail, QString &strBack )
{
	// create a comma dilimeted set of emails...
	// use the power of short circuiting...
	bool foundDefault = false;
	QString strTmp;
	int start = 0;
	int where;
	// start at the beginng.
	strBack = strDefaultEmail;
	where = 0;
	while ( where > -1 ) {
		start = where;
		where = strOtherEmail.find( ' ', where + 1 );
		if ( where > 0 ) {
			strTmp = strOtherEmail.mid( start, where - start ).stripWhiteSpace();
		} else
			strTmp = strOtherEmail.right( strOtherEmail.length() - start ).stripWhiteSpace();
		if ( foundDefault || strTmp != strDefaultEmail ) {
			strBack.append( ", " );
			strBack.append( strTmp );
		} else
			foundDefault = true;
	}
}


static inline bool containsAlphaNum( const QString &str )
{
	int i,
		count = str.length();
	for ( i = 0; i < count; i++ )
		if ( !str[i].isSpace() )
			return TRUE;
	return FALSE;
}

static inline bool constainsWhiteSpace( const QString &str )
{
	int i,
		count = str.length();
	for (i = 0; i < count; i++ )
		if ( str[i].isSpace() )
			return TRUE;
	return FALSE;
}

void ContactEditor::setPersonalView( bool personal )
{
	m_personalView = personal;
	
	// Currently disbled due to the fact that
	// show will not work...
	return;
	
	if ( personal ){
		cmbCat->hide();
		labCat->hide();
		
	} else{
		cmbCat->show();
		labCat->show();		
	}
}

void ContactEditor::slotAnniversaryDateChanged( int year, int month, int day)
{
	QDate date;
	date.setYMD( year, month, day );
	QString dateString = TimeString::numberDateString( date );
	anniversaryButton->setText( dateString );
	ent.setAnniversary ( date );
}

void ContactEditor::slotBirthdayDateChanged( int year, int month, int day)
{
	QDate date;
	date.setYMD( year, month, day );
	QString dateString = TimeString::numberDateString( date );
	birthdayButton->setText( dateString );
	ent.setBirthday ( date );
}

void ContactEditor::slotRemoveBirthday()
{
	qWarning("void ContactEditor::slotRemoveBirthday()");
	ent.setBirthday( QDate() );
	updateDatePicker();
}

void ContactEditor::slotRemoveAnniversary()
{
	qWarning("void ContactEditor::slotRemoveAnniversary()");
	ent.setAnniversary( QDate() );
	updateDatePicker();
}
