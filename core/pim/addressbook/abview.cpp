/**********************************************************************
** Copyright (c) 2002 Stefan Eilers (eilers.stefan@epost.de)
**
** This file is part of Qt Palmtop Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**
**********************************************************************/

#include "abview.h"

#include <opie2/ocontactaccessbackend_vcard.h>

#include <qpe/global.h>

#include <qlayout.h>

#include <assert.h>


// Is defined in LibQPE
extern QString categoryFileName();

QString addressbookPersonalVCardName()
{
	QString filename = Global::applicationFileName("addressbook",
						       "businesscard.vcf");
	return filename;
}


AbView::AbView ( QWidget* parent, const QValueList<int>& ordered ):
	QWidget(parent),
	mCat(0),
	m_inSearch( false ),
	m_inPersonal( false ),
	m_curr_category( -1 ),
	m_curr_View( TableView ),
	m_prev_View( TableView ),
	m_curr_Contact ( 0 ),
	m_contactdb ( 0l ),
	m_storedDB ( 0l ),
	m_viewStack( 0l ),
	m_abTable( 0l ),
	m_orderedFields( ordered )
{
	qWarning("AbView::c'tor");
	// Load default database and handle syncing myself.. !
	m_contactdb = new Opie::OPimContactAccess ( "addressbook", 0l, 0l, false );
	m_contactdb -> setReadAhead( 16 ); // Use ReadAhead-Cache if available
	mCat.load( categoryFileName() );

	// Create Layout and put WidgetStack into it.
  	QVBoxLayout *vb = new QVBoxLayout( this );
 	m_viewStack = new QWidgetStack( this );
	vb->addWidget( m_viewStack );

	// Creat TableView
 	QVBox* tableBox = new QVBox( m_viewStack );
 	m_abTable = new AbTable( m_orderedFields, tableBox, "table" );
	m_abTable->setCurrentCell( 0, 0 );
	m_abTable->setFocus();

	// Add TableView to WidgetStack and raise it
	m_viewStack -> addWidget( tableBox , TableView );

	// Create CardView and add it to WidgetStack
 	QVBox* cardBox = new QVBox( m_viewStack );
	m_ablabel = new AbLabel( cardBox, "CardView");
	m_viewStack -> addWidget( cardBox , CardView );

	// Connect views to me
	connect ( m_abTable, SIGNAL( signalSwitch(void) ),
		  this, SLOT( slotSwitch(void) ) );
	connect ( m_ablabel, SIGNAL( signalOkPressed(void) ),
		  this, SLOT( slotSwitch(void) ) );

	load();
}

AbView::~AbView()
{
	m_contactdb -> save();
	delete m_contactdb;

	if ( m_storedDB ){
		m_storedDB -> save();
		delete m_storedDB;
	}
}


void AbView::setView( Views view )
{
	qWarning("AbView::setView( Views view )");
 	m_curr_View = view;
 	load();
}

void AbView::addEntry( const Opie::OPimContact &newContact )
{
	qWarning("abview:AddContact");
	m_contactdb->add ( newContact );
	load();

}
void AbView::removeEntry( const int UID )
{
	qWarning("abview:RemoveContact");
	m_contactdb->remove( UID );
	load();
}

void AbView::replaceEntry( const Opie::OPimContact &contact )
{
	qWarning("abview:ReplaceContact");
	m_contactdb->replace( contact );
	load();

}

Opie::OPimContact AbView::currentEntry()
{
	Opie::OPimContact currentContact;

	switch ( (int) m_curr_View ) {
	case TableView:
		currentContact = m_abTable -> currentEntry();
		break;
	case CardView:
		currentContact = m_ablabel -> currentEntry();
		break;
	}
	m_curr_Contact = currentContact.uid();
	return currentContact;
}

bool AbView::save()
{
	//	qWarning("abView:Save data");

	return m_contactdb->save();
}

void AbView::load()
{
	qWarning("abView:Load data");

	// Letter Search is stopped at this place
	emit signalClearLetterPicker();

	if ( m_inPersonal )
		// VCard Backend does not sort..
		m_list = m_contactdb->allRecords();
	else{
		m_list = m_contactdb->sorted( true, 0, 0, 0 );
		if ( m_curr_category != -1 )
			clearForCategory();
	}

	qWarning ("Number of contacts: %d", m_list.count());

	updateView( true );

}

void AbView::reload()
{
        qWarning( "void AbView::reload()" );

	m_contactdb->reload();
	load();
}

void AbView::clear()
{
	// :SX
}

void AbView::setShowByCategory( const QString& cat )
{
	qWarning("AbView::setShowCategory( const QString& cat )");

	int intCat = 0;

	// All (cat == NULL) will be stored as -1
	if ( cat.isNull() )
		intCat = -1;
	else
		intCat = mCat.id("Contacts", cat );

	// Just do anything if we really change the category
	if ( intCat != m_curr_category ){
		//		qWarning ("Categories: Selected %s.. Number: %d", cat.latin1(), m_curr_category);

		m_curr_category = intCat;
		emit signalClearLetterPicker();

		load();
	}

}

void AbView::setShowToView( Views view )
{
       	qWarning("void AbView::setShowToView( View %d )", view);

	if ( m_curr_View != view ){
		qWarning ("Change the View (Category is: %d)", m_curr_category);
		m_prev_View = m_curr_View;
		m_curr_View = view;

		updateView();
	}

}

void AbView::setShowByLetter( char c, AbConfig::LPSearchMode mode )
{
	qWarning("void AbView::setShowByLetter( %c, %d )", c, mode );

	assert( mode < AbConfig::LASTELEMENT );

	Opie::OPimContact query;
	if ( c == 0 ){
		load();
		return;
	}else{
		// If the current Backend is unable to solve the query, we will
		// ignore the request ..
		if ( ! m_contactdb->hasQuerySettings( Opie::OPimContactAccess::WildCards | Opie::OPimContactAccess::IgnoreCase ) ){
			return;
		}

		switch( mode ){
		case AbConfig::LastName:
			query.setLastName( QString("%1*").arg(c) );
			break;
		case AbConfig::FileAs:
			query.setFileAs( QString("%1*").arg(c) );
			break;
		default:
			qWarning( "Unknown Searchmode for AbView::setShowByLetter ! -> %d", mode );
			qWarning( "I will ignore it.." );
			return;
		}
		m_list = m_contactdb->queryByExample( query, Opie::OPimContactAccess::WildCards | Opie::OPimContactAccess::IgnoreCase );
		if ( m_curr_category != -1 )
			clearForCategory();
		m_curr_Contact = 0;
	}
	updateView( true );
}

void AbView::setListOrder( const QValueList<int>& ordered )
{
	m_orderedFields = ordered;
	if ( m_abTable ){
		m_abTable->setOrderedList( ordered );
		m_abTable->refresh();
	}
	updateView();
}


QString AbView::showCategory() const
{
	return mCat.label( "Contacts", m_curr_category );
}

void AbView::showPersonal( bool personal )
{
	qWarning ("void AbView::showPersonal( %d )", personal);

	if ( personal ){

		if ( m_inPersonal )
			return;

		// Now switch to vCard Backend and load data.
		// The current default backend will be stored
		// to avoid unneeded load/stores.
		m_storedDB = m_contactdb;

		Opie::OPimContactAccessBackend* vcard_backend = new Opie::OPimContactAccessBackend_VCard( QString::null,
									  addressbookPersonalVCardName() );
		m_contactdb = new Opie::OPimContactAccess ( "addressbook", QString::null , vcard_backend, true );

		m_inPersonal = true;
		m_curr_View = CardView;

	}else{

		if ( !m_inPersonal )
			return;

		// Remove vCard Backend and restore default
		m_contactdb->save();
		delete m_contactdb;

		m_contactdb = m_storedDB;
		m_storedDB = 0l;

		m_curr_View = TableView;
		m_inPersonal = false;

	}
	load();
}

void AbView::setCurrentUid( int uid ){

	m_curr_Contact = uid;
	updateView( true ); //true: Don't modificate the UID !
}


QStringList AbView::categories()
{
	mCat.load( categoryFileName() );
	QStringList categoryList = mCat.labels( "Contacts" );
	return categoryList;
}

// BEGIN: Slots
void AbView::slotDoFind( const QString &str, bool caseSensitive, bool useRegExp,
			 bool , QString cat )
{
	//	qWarning( "void AbView::slotDoFind" );

	// We reloading the data: Deselect Letterpicker
	emit signalClearLetterPicker();

	// Use the current Category if nothing else selected
	int category = 0;

	if ( cat.isEmpty() )
		category = m_curr_category;
	else{
		category = mCat.id("Contacts", cat );
	}

	//	qWarning ("Find in Category %d", category);

	QRegExp r( str );
	r.setCaseSensitive( caseSensitive );
	r.setWildcard( !useRegExp );

	// Get all matching entries out of the database
	m_list = m_contactdb->matchRegexp( r );

	//	qWarning( "found: %d", m_list.count() );
	if ( m_list.count() == 0 ){
		emit signalNotFound();
		return;
	}

	// Now remove all contacts with wrong category (if any selected)
	// This algorithm is a litte bit ineffective, but
	// we will not have a lot of matching entries..
	if ( m_curr_category != -1 )
		clearForCategory();

	// Now show all found entries
	updateView( true );
}

void AbView::offSearch()
{
	m_inSearch = false;

	load();
}

void AbView::slotSwitch(){
	//	qWarning("AbView::slotSwitch()");

	m_prev_View = m_curr_View;
	switch ( (int) m_curr_View ){
	case TableView:
		qWarning("Switching to CardView");
		m_curr_View = CardView;
		break;
	case CardView:
		qWarning("Switching to TableView");
		m_curr_View = TableView;
		break;
	}
	updateView();

}

// END: Slots

void AbView::clearForCategory()
{
	Opie::OPimContactAccess::List::Iterator it;
	// Now remove all contacts with wrong category if any category selected

	Opie::OPimContactAccess::List allList = m_list;
	if ( m_curr_category != -1 ){
		for ( it = allList.begin(); it != allList.end(); ++it ){
			if ( !contactCompare( *it, m_curr_category ) ){
				//				qWarning("Removing %d", (*it).uid());
				m_list.remove( (*it).uid() );
			}
		}
	}

}

bool AbView::contactCompare( const Opie::OPimContact &cnt, int category )
{
	//	qWarning ("bool AbView::contactCompare( const Opie::OPimContact &cnt, %d )", category);

	bool returnMe;
	QArray<int> cats;
	cats = cnt.categories();

	//	qWarning ("Number of categories: %d", cats.count() );

	returnMe = false;
	if ( cats.count() == 0 && category == 0 )
		// Contacts with no category will just shown on "All" and "Unfiled"
		returnMe = true;
	else {
		int i;
		for ( i = 0; i < int(cats.count()); i++ ) {
			//			qWarning("Comparing %d with %d",cats[i],category );
			if ( cats[i] == category ) {
				returnMe = true;
				break;
			}
		}
	}
	//	qWarning ("Return: %d", returnMe);
	return returnMe;
}

// In Some rare cases we have to update all lists..
void AbView::updateListinViews()
{
		m_abTable -> setContacts( m_list );
		m_ablabel -> setContacts( m_list );
}

void  AbView::updateView( bool newdata )
{
	//	qWarning("AbView::updateView()");

	if ( m_viewStack -> visibleWidget() ){
		m_viewStack -> visibleWidget() -> clearFocus();
	}

	// If we switching the view, we have to store some information
	if ( !newdata ){
		if ( m_list.count() ){
			switch ( (int) m_prev_View ) {
			case TableView:
				m_curr_Contact = m_abTable -> currentEntry_UID();
				break;
			case CardView:
				m_curr_Contact = m_ablabel -> currentEntry_UID();
				break;
			}
		}else
			m_curr_Contact = 0;
	}

	// Feed all views with new lists
	if ( newdata )
		updateListinViews();

	// Tell the world that the view is changed
	if ( m_curr_View != m_prev_View )
		emit signalViewSwitched ( (int) m_curr_View );

	m_prev_View = m_curr_View;

	// Switch to new View
	switch ( (int) m_curr_View ) {
	case TableView:
		m_abTable -> setChoiceSelection( m_orderedFields );
		if ( m_curr_Contact != 0 )
			m_abTable -> selectContact ( m_curr_Contact );
		m_abTable -> setFocus();
		break;
	case CardView:
		if ( m_curr_Contact != 0 )
			m_ablabel -> selectContact( m_curr_Contact );
		m_ablabel -> setFocus();
		break;
	}

	// Raise the current View
	m_viewStack -> raiseWidget( m_curr_View );
}


