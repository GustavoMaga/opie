/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
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
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <opie2/odebug.h>
#include <opie2/opimrecordlist.h>
#include <opie2/oresource.h>

#include <qpe/timestring.h>

#include "abtable.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <ctype.h> //toupper() for key hack

#if 0

/*!
  \class AbTableItem abtable.h

  \brief QTableItem based class for showing a field of an entry
*/

AbTableItem::AbTableItem( QTable *t, EditType et, const QString &s,
			  const QString &secondSortKey)
	: QTableItem( t, et, s )
{
	//    sortKey = s.lower() + QChar( '\0' ) + secondSortKey.lower();
	sortKey = Qtopia::buildSortKey( s, secondSortKey );
}

int AbTableItem::alignment() const
{
	return AlignLeft|AlignVCenter;
}

QString AbTableItem::key() const
{
	return sortKey;
}

// A way to reset the item, without out doing a delete or a new...
void AbTableItem::setItem( const QString &txt, const QString &secondKey )
{
	setText( txt );
	sortKey = Qtopia::buildSortKey( txt, secondKey );

	//    sortKey = txt.lower() + QChar( '\0' ) + secondKey.lower();
}

/*!
  \class AbPickItem abtable.h

  \brief QTableItem based class for showing slection of an entry
*/

AbPickItem::AbPickItem( QTable *t ) :
	QTableItem(t, WhenCurrent, "?")
{
}

QWidget *AbPickItem::createEditor() const
{
	QComboBox* combo = new QComboBox( table()->viewport() );
	( (AbPickItem*)this )->cb = combo;
	AbTable* t = static_cast<AbTable*>(table());
	QStringList c = t->choiceNames();
	int cur = 0;
	for (QStringList::ConstIterator it = c.begin(); it!=c.end(); ++it) {
		if ( *it == text() )
			cur = combo->count();
		combo->insertItem(*it);
	}
	combo->setCurrentItem(cur);
	return combo;
}

void AbPickItem::setContentFromEditor( QWidget *w )
{
	if ( w->inherits("QComboBox") )
		setText( ( (QComboBox*)w )->currentText() );
	else
		QTableItem::setContentFromEditor( w );
}

#endif

/*!
  \class AbTable abtable.h

  \brief QTable based class for showing a list of entries
*/

AbTable::AbTable( const QValueList<int> order, QWidget *parent, const char *name )
	: QTable( parent, name ),
	  lastSortCol( -1 ),
	  asc( TRUE ),
	  intFields( order ),
	  enablePainting( true ),
	  columnVisible( true ),
	  countNested( 0 )
{
	//	odebug << "C'tor start" << oendl;
	setSelectionMode( NoSelection );
	init();
	setSorting( false ); // The table should not sort by itself!

	connect( this, SIGNAL(clicked(int,int,int,const QPoint&)),
		 this, SLOT(itemClicked(int,int)) );

	// contactList.clear();
	//	odebug << "C'tor end" << oendl;
}

AbTable::~AbTable()
{
}

void AbTable::init()
{
	// :SX showChar = '\0';
	setNumRows( 0 );
	setNumCols( 2 );

	horizontalHeader()->setLabel( 0, tr( "Full Name" ));
	horizontalHeader()->setLabel( 1, tr( "Contact" ));
	setLeftMargin( 0 );
	verticalHeader()->hide();
	columnVisible = true;
}

void AbTable::setContacts( const Opie::OPimContactAccess::List& viewList )
{
	odebug << "AbTable::setContacts()" << oendl;

	clear();
	m_viewList = viewList;

	setPaintingEnabled( FALSE );

	setNumRows( m_viewList.count() );


	updateVisible();

	setPaintingEnabled( TRUE );

}

void AbTable::setOrderedList( const QValueList<int> ordered )
{
	intFields = ordered;
}


bool AbTable::selectContact( int UID )
{
	odebug << "AbTable::selectContact( " << UID << " )" << oendl;
	int rows = numRows();
	bool found = false;

	setPaintingEnabled( FALSE );
	odebug << "Search start" << oendl;
	for ( int r = 0; r < rows; ++r ) {
		if ( m_viewList.uidAt( r ) == UID ){
			ensureCellVisible( r, 0 );
			setCurrentCell( r, 0 );
			found = true;
			break;
		}
	}
	odebug << "Search end" << oendl;

	if ( !found ){
		ensureCellVisible( 0,0 );
		setCurrentCell( 0, 0 );
	}

	setPaintingEnabled( TRUE );

	return true;
}


void AbTable::columnClicked( int col )
{
	odebug << "columClicked(" << col << ")" << oendl;

	if ( col == 0 ){
		odebug << "Change sort order: " << asc << oendl;
		asc = !asc;
		emit signalSortOrderChanged( asc );
	}
}

void AbTable::resort()
{
	owarn << "void AbTable::resort() NOT POSSIBLE !!" << oendl;

}

Opie::OPimContact AbTable::currentEntry()
{
	return m_viewList[currentRow()];
}

int AbTable::currentEntry_UID()
{
	return ( currentEntry().uid() );
}

void AbTable::clear()
{
	odebug << "void AbTable::clear()" << oendl;
	// contactList.clear();

	setPaintingEnabled( FALSE );
	for ( int r = 0; r < numRows(); ++r ) {
		for ( int c = 0; c < numCols(); ++c ) {
			if ( cellWidget( r, c ) )
				clearCellWidget( r, c );
			clearCell( r, c );
		}
	}
	setNumRows( 0 );
	setPaintingEnabled( TRUE );
}

// Refresh updates column 2 if the contactsettings changed
void AbTable::refresh()
{
	owarn << "void AbTable::refresh() NOT IMPLEMENTED !!" << oendl;

#if 0
	int rows = numRows();
	AbTableItem *abi;
	ContactItem contactItem;

	setPaintingEnabled( FALSE );
	for ( int r = 0; r < rows; ++r ) {
		abi = static_cast<AbTableItem*>( item(r, 0) );
		contactItem = findContactContact( contactList[abi], r );
		static_cast<AbTableItem*>( item(r, 1) )->setItem( contactItem.value, abi->text() );
		if ( !contactItem.icon.isNull() ){
			static_cast<AbTableItem*>( item(r, 1) )->
				setPixmap( contactItem.icon );
		}else{
			static_cast<AbTableItem*>( item(r, 1) )->
				setPixmap( QPixmap() );
		}
	}
	resort();
	setPaintingEnabled( TRUE );

#endif
}

void AbTable::keyPressEvent( QKeyEvent *e )
{
	char key = toupper( e->ascii() );

	if ( key >= 'A' && key <= 'Z' )
		moveTo( key );

	//		odebug << "Received key .." << oendl;
	switch( e->key() ) {
	case Qt::Key_Space:
	case Qt::Key_Return:
	case Qt::Key_Enter:
		emit signalSwitch();
		break;
// 	case Qt::Key_Up:
// 		odebug << "a" << oendl;
// 		emit signalKeyUp();
// 		break;
// 	case Qt::Key_Down:
// 		odebug << "b" << oendl;
// 		emit signalKeyDown();
// 		break;
	default:
		QTable::keyPressEvent( e );
	}

}

void AbTable::moveTo( char /*c*/ )
{
	odebug << "void AbTable::moveTo( char c ) NOT IMPLEMENTED !!" << oendl;

}

#if 0
// Useless.. Nobody uses it .. (se)
QString AbTable::findContactName( const Opie::OPimContact &entry )
{
	// We use the fileAs, then company, defaultEmail
	QString str;
	str = entry.fileAs();
	if ( str.isEmpty() ) {
		str = entry.company();
		if ( str.isEmpty() ) {
			str = entry.defaultEmail();
		}
	}
	return str;
}
#endif


void AbTable::resizeRows() {
	/*
	  if (numRows()) {
	  for (int i = 0; i < numRows(); i++) {
	  setRowHeight( i, size );
	  }
	  }
	  updateVisible();
	*/
}


void AbTable::realignTable()
{
	//	odebug << "void AbTable::realignTable()" << oendl;

	setPaintingEnabled( FALSE );

	resizeRows();
	fitColumns();

	setPaintingEnabled( TRUE );

}




#if QT_VERSION <= 230
#ifndef SINGLE_APP
void QTable::paintEmptyArea( QPainter *p, int cx, int cy, int cw, int ch )
{
	// Region of the rect we should draw
	QRegion reg( QRect( cx, cy, cw, ch ) );
	// Subtract the table from it
	reg = reg.subtract( QRect( QPoint( 0, 0 ), tableSize() ) );
	// And draw the rectangles (transformed as needed)
	QArray<QRect> r = reg.rects();
	for (unsigned int i=0; i<r.count(); i++)
		p->fillRect( r[i], colorGroup().brush( QColorGroup::Base ) );
}
#endif
#endif


// int AbTable::rowHeight( int ) const
// {
//     return 18;
// }

// int AbTable::rowPos( int row ) const
// {
//     return 18*row;
// }

// int AbTable::rowAt( int pos ) const
// {
//     return QMIN( pos/18, numRows()-1 );
// }



void AbTable::fitColumns()
{
	odebug << "void AbTable::fitColumns()" << oendl;
	int contentsWidth = visibleWidth() / 2;
	// Fix to better value
	// contentsWidth = 130;

	setPaintingEnabled( FALSE );

	if ( columnVisible == false ){
		showColumn(0);
		columnVisible = true;
	}

	//	odebug << "Width: " << contentsWidth << oendl;

	setColumnWidth( 0, contentsWidth );
	adjustColumn(1);
	if ( columnWidth(1) < contentsWidth )
		setColumnWidth( 1, contentsWidth );

	setPaintingEnabled( TRUE );
}

void AbTable::show()
{
	//	odebug << "void AbTable::show()" << oendl;
	realignTable();
	QTable::show();
}

#if 0
void AbTable::setChoiceNames( const QStringList& list)
{
	choicenames = list;
	if ( choicenames.isEmpty() ) {
		// hide pick column
		setNumCols( 2 );
	} else {
		// show pick column
		setNumCols( 3 );
		setColumnWidth( 2, fontMetrics().width(tr( "Pick" ))+8 );
		horizontalHeader()->setLabel( 2, tr( "Pick" ));
	}
	fitColumns();
}
#endif

void AbTable::itemClicked(int,int col)
{
	//	odebug << "AbTable::itemClicked(int, col: " << col << ")" << oendl;
	if ( col == 2 ) {
		return;
	} else {
		//	odebug << "Emitting signalSwitch()" << oendl;
		emit signalSwitch();
	}
}

#if 0
QStringList AbTable::choiceNames() const
{
	return choicenames;
}

#endif
void AbTable::setChoiceSelection( const QValueList<int>& list )
{
	intFields = list;
}

QStringList AbTable::choiceSelection(int /*index*/) const
{
	QStringList r;
	/* ######

	QString selname = choicenames.at(index);
	for (each row) {
	Opie::OPimContact *c = contactForRow(row);
	if ( text(row,2) == selname ) {
	r.append(c->email);
	}
	}

	*/
	return r;
}


void AbTable::updateVisible()
{
	//	odebug << "void AbTable::updateVisible()" << oendl;

	int visible,
		totalRows,
		row,
		selectedRow = 0;

	visible = 0;

	setPaintingEnabled( FALSE );

	realignTable();

	totalRows = numRows();
	for ( row = 0; row < totalRows; row++ ) {
		if ( rowHeight(row) == 0 ) {
			showRow( row );
			adjustRow( row );
			if ( isSelected( row,0 ) || isSelected( row,1 ) )
				selectedRow = row;
		}
		visible++;
	}

	if ( selectedRow )
		setCurrentCell( selectedRow, 0 );

	if ( !visible )
		setCurrentCell( -1, 0 );

	setPaintingEnabled( TRUE );
}


void AbTable::setPaintingEnabled( bool e )
{
	//	odebug << "IN void AbTable::setPaintingEnabled( " << e << " )->Nested: "
	//						<< countNested << oendl;

	if ( e ) {
		if ( countNested > 0 )
			--countNested;
		if ( ! countNested ){
			setUpdatesEnabled( true );
			enablePainting = true;
			rowHeightChanged( 0 );
			viewport()->update();
		}
	} else {
		++countNested;
		enablePainting = false;
		setUpdatesEnabled( false );
	}
	//	odebug << "OUT void AbTable::setPaintingEnabled( " << e << " )->Nested: "
	//						<< countNested << oendl;
}

void AbTable::viewportPaintEvent( QPaintEvent* e ) {
	//	odebug << "void AbTable::viewportPaintEvent( QPaintEvent* e ) -> "
	//						<< enablePainting << oendl;
	if ( enablePainting )
		QTable::viewportPaintEvent( e );
}

void AbTable::paintCell(QPainter* p,  int row, int col, const QRect& cr, bool ) {
    const QColorGroup &cg = colorGroup();

    p->save();

	//	odebug << "Paint row: " << row << oendl;

    Opie::OPimContact act_contact = m_viewList[row];

    // Paint alternating background bars
    if (  (row % 2 ) == 0 ) {
        p->fillRect( 0, 0, cr.width(), cr.height(), cg.brush( QColorGroup::Base ) );
        p->setPen( QPen( cg.text() ) );
    }
    else {
        p->fillRect( 0, 0, cr.width(), cr.height(), cg.brush( QColorGroup::Background ) );
        p->setPen( QPen( cg.buttonText() ) );
    }

    QFont f = p->font();
    QFontMetrics fm(f);

    int marg = 2;
    int x = 0;
    int y = ( cr.height() - 14 ) / 2;

    QString nameText    = act_contact.fileAs();

    switch( col ){
	    case 0:
		    p->drawText( x + marg,2 + fm.ascent(), nameText );
	    break;
	    case 1:{

		    ContactItem contactItem = findContactContact( act_contact, 0 );
		    QPixmap contactPic  = contactItem.icon; /* pixmap( row, col ); */
		    QString contactText = contactItem.value;

		    if ( !contactPic.isNull() )
			    {
				    p->drawPixmap( x + marg, y, contactPic );
				    p->drawText( x + marg + contactPic.width()
						 + 4,2 + fm.ascent(), contactText );
			    }
		    else
			    {
				    p->drawText( x + marg,2 + fm.ascent(), contactText );
			    }
	    }
	    break;

    }
    p->restore();
}

void AbTable::rowHeightChanged( int row )
{
	if ( enablePainting )
		QTable::rowHeightChanged( row );
}
ContactItem AbTable::findContactContact( const Opie::OPimContact &entry, int /* row */ )
{

	ContactItem item;

	item.value = "";

	for ( QValueList<int>::ConstIterator it = intFields.begin();
	      it != intFields.end(); ++it ) {
		switch ( *it ) {
		default:
			break;
		case Qtopia::Title:
			item.value = entry.title();
			break;
		case Qtopia::Suffix:
			item.value = entry.suffix();
			break;
		case Qtopia::FileAs:
			item.value = entry.fileAs();
			break;
		case Qtopia::DefaultEmail:
			item.value = entry.defaultEmail();
			if ( !item.value.isEmpty() )
				item.icon = Opie::Core::OResource::loadPixmap( "addressbook/email" );
			break;
		case Qtopia::Emails:
			item.value = entry.emails();
			if ( !item.value.isEmpty() )
				item.icon = Opie::Core::OResource::loadPixmap( "addressbook/email" );
			break;
		case Qtopia::HomeStreet:
			item.value = entry.homeStreet();
			break;
		case Qtopia::HomeCity:
			item.value = entry.homeCity();
			break;
		case Qtopia::HomeState:
			item.value = entry.homeState();
			break;
		case Qtopia::HomeZip:
			item.value = entry.homeZip();
			break;
		case Qtopia::HomeCountry:
			item.value = entry.homeCountry();
			break;
		case Qtopia::HomePhone:
			item.value = entry.homePhone();
			if ( !item.value.isEmpty() )
				item.icon = Opie::Core::OResource::loadPixmap( "addressbook/phonehome" );
			break;
		case Qtopia::HomeFax:
			item.value = entry.homeFax();
			if ( !item.value.isEmpty() )
				item.icon = Opie::Core::OResource::loadPixmap( "addressbook/faxhome" );
			break;
		case Qtopia::HomeMobile:
			item.value = entry.homeMobile();
			if ( !item.value.isEmpty() )
				item.icon = Opie::Core::OResource::loadPixmap( "addressbook/mobilehome" );
			break;
		case Qtopia::HomeWebPage:
			item.value = entry.homeWebpage();
			if ( !item.value.isEmpty() )
				item.icon = Opie::Core::OResource::loadPixmap( "addressbook/webpagehome" );
			break;
		case Qtopia::Company:
			item.value = entry.company();
			break;
		case Qtopia::BusinessCity:
			item.value = entry.businessCity();
			break;
		case Qtopia::BusinessStreet:
			item.value = entry.businessStreet();
			break;
		case Qtopia::BusinessZip:
			item.value = entry.businessZip();
			break;
		case Qtopia::BusinessCountry:
			item.value = entry.businessCountry();
			break;
		case Qtopia::BusinessWebPage:
			item.value = entry.businessWebpage();
			if ( !item.value.isEmpty() )
				item.icon = Opie::Core::OResource::loadPixmap( "addressbook/webpagework" );
			break;
		case Qtopia::JobTitle:
			item.value = entry.jobTitle();
			break;
		case Qtopia::Department:
			item.value = entry.department();
			break;
		case Qtopia::Office:
			item.value = entry.office();
			break;
		case Qtopia::BusinessPhone:
			item.value = entry.businessPhone();
			if ( !item.value.isEmpty() )
				item.icon = Opie::Core::OResource::loadPixmap( "addressbook/phonework" );
			break;
		case Qtopia::BusinessFax:
			item.value = entry.businessFax();
			if ( !item.value.isEmpty() )
				item.icon = Opie::Core::OResource::loadPixmap( "addressbook/faxwork" );
			break;
		case Qtopia::BusinessMobile:
			item.value = entry.businessMobile();
			if ( !item.value.isEmpty() )
				item.icon = Opie::Core::OResource::loadPixmap( "addressbook/mobilework" );
			break;
		case Qtopia::BusinessPager:
			item.value = entry.businessPager();
			break;
		case Qtopia::Profession:
			item.value = entry.profession();
			break;
		case Qtopia::Assistant:
			item.value = entry.assistant();
			break;
		case Qtopia::Manager:
			item.value = entry.manager();
			break;
		case Qtopia::Spouse:
			item.value = entry.spouse();
			break;
		case Qtopia::Gender:
			item.value = entry.gender();
			break;
		case Qtopia::Birthday:
			if ( ! entry.birthday().isNull() ){
				item.value = TimeString::numberDateString( entry.birthday() );
			}
			break;
		case Qtopia::Anniversary:
			if ( ! entry.anniversary().isNull() ){
				item.value = TimeString::numberDateString( entry.anniversary() );
			}
			break;
		case Qtopia::Nickname:
			item.value = entry.nickname();
			break;
		case Qtopia::Children:
			item.value = entry.children();
			break;
		case Qtopia::Notes:
			item.value = entry.notes();
			break;
		}
		if ( !item.value.isEmpty() )
			break;
	}
	return item;
}
