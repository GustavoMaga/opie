//
//
// C++ Implementation: $MODULE$
//
// Description: 
//
//
// Author: Patrick S. Vogt <tille@handhelds.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "datebooksearch.h"

#include "eventitem.h"

#include <qpe/resource.h>
#include <qpe/config.h>
#include <opie/oevent.h>
#include <opie/orecur.h>
#include <opie/odatebookaccess.h>
#include <qiconset.h>
#include <qaction.h>
#include <qdatetime.h>
#include <qpopupmenu.h>

DatebookSearch::DatebookSearch(QListView* parent, QString name)
: SearchGroup(parent, name), _dates(0), _popupMenu(0)
{
	QIconSet is = Resource::loadIconSet( "datebook/DateBookSmall" );
	setPixmap( 0, is.pixmap( QIconSet::Large, true ) );
	actionShowPastEvents = new QAction( QObject::tr("show past events"),QString::null,  0, 0, 0, true );
	actionSearchInDates = new QAction( QObject::tr("search in dates"),QString::null,  0, 0, 0, true );
	Config cfg( "osearch", Config::User );
   	cfg.setGroup( "datebook_settings" );
   	actionShowPastEvents->setOn( cfg.readBoolEntry( "show_past_events", false ) );
   	actionSearchInDates->setOn( cfg.readBoolEntry( "search_in_dates", true ) );
}

DatebookSearch::~DatebookSearch()
{
	qDebug("SAVE DATEBOOK SEARCH CONFIG");
	Config cfg( "osearch", Config::User );
   	cfg.setGroup( "datebook_settings" );
   	cfg.writeEntry( "show_past_events", actionShowPastEvents->isOn() );
   	cfg.writeEntry( "search_in_dates", actionSearchInDates->isOn() );
	delete _dates;
	delete _popupMenu;
	delete actionShowPastEvents;
	delete actionSearchInDates;
}


void DatebookSearch::load()
{
	 _dates = new ODateBookAccess();
	 _dates->load();
}

int DatebookSearch::search()
{
	ORecordList<OEvent> results = _dates->matchRegexp(_search);
	for (uint i = 0; i < results.count(); i++)
		insertItem( new OEvent( results[i] ) );
	return _resultCount;
}

void DatebookSearch::insertItem( void *rec )
{
	OEvent *ev = (OEvent*)rec;
	if ( !actionShowPastEvents->isOn() &&
	      ev->endDateTime() < QDateTime::currentDateTime() &&
	      !ev->recurrence().doesRecur()
	    ) return;
	if ( !actionSearchInDates->isOn() && (
	      ev->lastHitField() == Qtopia::StartDateTime ||
 	      ev->lastHitField() == Qtopia::EndDateTime )
	    ) return;
	new EventItem( this, ev );
	_resultCount++;
}

QPopupMenu* DatebookSearch::popupMenu()
{
	if (!_popupMenu){
		_popupMenu = new QPopupMenu( 0 );
 		actionShowPastEvents->addTo( _popupMenu );
 		actionSearchInDates->addTo( _popupMenu );
	}
	return _popupMenu;
}

