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

#include <opie2/odebug.h>
#include <opie2/opimevent.h>
#include <opie2/opimrecurrence.h>
#include <opie2/oresource.h>

#include <qpe/config.h>

#include <qaction.h>
#include <qpopupmenu.h>



DatebookSearch::DatebookSearch(QListView* parent, QString name)
: SearchGroup(parent, name), _dates(0), _popupMenu(0)
{
    setPixmap( 0, Opie::Core::OResource::loadPixmap( "datebook/DateBook", Opie::Core::OResource::SmallIcon ) );

    actionShowPastEvents = new QAction( QObject::tr("Show past events"),QString::null,  0, 0, 0, true );
    actionSearchInDates = new QAction( QObject::tr("Search in dates"),QString::null,  0, 0, 0, true );
    Config cfg( "osearch", Config::User );
    cfg.setGroup( "datebook_settings" );
    actionShowPastEvents->setOn( cfg.readBoolEntry( "show_past_events", false ) );
    actionSearchInDates->setOn( cfg.readBoolEntry( "search_in_dates", true ) );
}

DatebookSearch::~DatebookSearch()
{
    odebug << "SAVE DATEBOOK SEARCH CONFIG" << oendl;
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
    OPimRecordList<OPimEvent> results = _dates->matchRegexp(m_search);
    for (uint i = 0; i < results.count(); i++)
        insertItem( new OPimEvent( results[i] ) );
    return m_resultCount;
}

void DatebookSearch::insertItem( void *rec )
{
    OPimEvent *ev = (OPimEvent*)rec;
    if ( !actionShowPastEvents->isOn() &&
          ev->endDateTime() < QDateTime::currentDateTime() &&
          !ev->recurrence().doesRecur()
        ) return;
    if ( !actionSearchInDates->isOn() && (
          ev->lastHitField() == Qtopia::StartDateTime ||
          ev->lastHitField() == Qtopia::EndDateTime )
        ) return;
    new EventItem( this, ev );
    m_resultCount++;
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

