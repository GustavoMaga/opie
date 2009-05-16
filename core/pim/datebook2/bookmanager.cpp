/*
                             This file is part of the Opie Project
                             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
              =.             
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "bookmanager.h"

#include <qpe/alarmserver.h>
#include <opie2/opimnotifymanager.h>
#include <opie2/opimrecurrence.h>

using namespace Opie;
using namespace Opie::Datebook;

BookManager::BookManager() {
    m_db = 0;
    m_holiday = new DateBookHoliday;
}

BookManager::~BookManager() {
    delete m_db;
}

void BookManager::add( const OPimEvent& ev) {
    if (!m_db ) return;

    addAlarms( ev );
    m_db->add( ev );
}

void BookManager::update( const OPimEvent& up) {
    if ( !m_db ) return;

    removeAlarms( event(up.uid()) ); // need to remove old alarms
    addAlarms( up );
    m_db->replace( up );
}

void BookManager::remove( int uid ) {
    if ( !m_db ) return;

    removeAlarms( event(uid) );
    m_db->remove( uid );
}

void BookManager::remove( const QArray<int>& ar) {
    uint count = ar.count();
    for (uint i = 0; i < count; i++ )
        remove( ar[i] );
}

QList<OPimRecord> BookManager::records( const QDate& , const QDate& ) {
    return QList<OPimRecord>();
}

bool BookManager::isLoaded() const{
    return ( m_db != 0 );
}

bool BookManager::load() {
    if(!m_db) {
        m_db = new ODateBookAccess;
        return m_db->load();
    }
    else
        return true;
}

void BookManager::reload() {
    if (!m_db ) return;

    m_db->reload();
}

OPimEvent BookManager::event( int uid ) {
    if (!m_db ) return OPimEvent();

    return m_db->find( uid );
}

ODateBookAccess::List BookManager::allRecords()const {
    if (!m_db) return ODateBookAccess::List();

    return m_db->allRecords();
}

OPimOccurrence::List BookManager::list( const QDate& from,
                                              const QDate& to ) {
    if (!m_db) return OPimOccurrence::List();

    OPimOccurrence::List ret = m_holiday->getEffectiveEvents( from, to );
    ret += m_db->occurrences( from, to );

    return ret;
}

bool BookManager::save() {
    if (!m_db) return false;
    return m_db->save();
}

DateBookHoliday *BookManager::holiday() {
    return m_holiday;
}

void BookManager::addAlarms( const OPimEvent &ev ) {
    // Technically we only currently support one alarm, but let's iterate through anyway
    const OPimNotifyManager::Alarms &als = ev.notifiers().alarms();
    OPimNotifyManager::Alarms::ConstIterator it;
    for ( it = als.begin(); it != als.end(); ++it ) {
        QDateTime alarmDateTime = (*it).dateTime();
        if( ev.hasRecurrence() ) {
            // HACK: The application only supports setting an alarm n seconds before 
            // the event, not at a specific datetime; however all we have from the 
            // notifiers list is a datetime which is event start + n seconds, and in 
            // the case of a recurring event we need the datetime n seconds prior to 
            // the next occurrence, not the first one.
            int warn = alarmDateTime.secsTo( ev.startDateTime() );
            if( ! nextOccurrence( ev, QDateTime::currentDateTime().addSecs( warn + 1 ), alarmDateTime ) )
                continue;
            alarmDateTime = alarmDateTime.addSecs(-warn);
        }
        
        if( alarmDateTime < QDateTime::currentDateTime() )
            continue;

        AlarmServer::addAlarm( alarmDateTime, "QPE/Application/datebook2", "alarm(QDateTime,int)", ev.uid() );
    }
}

void BookManager::snoozeAlarm( const QDateTime &dt, int uid ) {
    AlarmServer::addAlarm( dt, "QPE/Application/datebook2", "alarm(QDateTime,int)", uid );
}

void BookManager::removeAlarms( const OPimEvent &ev ) {
    AlarmServer::deleteAlarm( QDateTime(), "QPE/Application/datebook2", "alarm(QDateTime,int)", ev.uid() );
}

void BookManager::setupAlarms( const OPimEvent &ev ) {
    removeAlarms( ev );
    addAlarms( ev );
}

void BookManager::setupAllAlarms() {
    // Unregister all alarms that belong to us
    AlarmServer::deleteAlarm( QDateTime(), "QPE/Application/datebook2", "alarm(QDateTime,int)", -1 );
    // Now, register all alarms in the future
    ODateBookAccess::List allrecs = allRecords();
    for ( ODateBookAccess::List::Iterator it = allrecs.begin(); it != allrecs.end(); ++it ) {
        addAlarms( (*it) );
    }    
}

// FIXME: this ought to be moved to somewhere in libopiepim2
bool BookManager::nextOccurrence( const OPimEvent &ev, const QDateTime &start, QDateTime &dt ) {
    QDateTime recurDateTime = ev.startDateTime();
    if( ev.hasRecurrence() ) {
        QDate recurDate;
        QDate startDate = start.date();
        do {
            if( ! ev.recurrence().nextOcurrence( startDate, recurDate ) )
                return false;
            if( recurDate == startDate ) {
                if( start.time() >= ev.startDateTime().time() ) {
                    startDate = startDate.addDays(1);
                    continue;
                }
            }
            recurDateTime = QDateTime( recurDate, ev.startDateTime().time() );
        } while(recurDateTime < start);
    }
    if( recurDateTime > start ) {
        dt = recurDateTime;
        return true;
    }
    return false;
}

QDate BookManager::findRealStart( const OPimOccurrence &occ ) {
    QDate dt( occ.date() );

    if( occ.position() != OPimOccurrence::Start ) {
        int uid = occ.toEvent().uid();
        OPimOccurrence::List occs = list(dt, dt);
        for (OPimOccurrence::List::ConstIterator it = occs.begin(); it != occs.end(); ++it ) {
            OPimOccurrence ito = (*it);
            if ( ito.toEvent().uid() == uid && ito.position() == OPimOccurrence::Start )
                return ito.date();
        }
    }

    return dt;
}

OPimEvent BookManager::find( const QString &str, bool caseSensitive, QDateTime &dt )
{
    // queryByExample not yet implemented in the backends, so we do it by hand instead

    QRegExp r( str );
    r.setCaseSensitive( caseSensitive );
    OPimRecordList<OPimEvent> lst = m_db->matchRegexp( r );

    QDateTime min;
    OPimEvent minev;
    OPimRecordListIterator<OPimEvent> it;
    for( it = lst.begin(); it != lst.end(); ++it ) 
    {
        QDateTime evdate;
        if( nextOccurrence( (*it), dt, evdate ) ) {
            if( !min.isValid() || min < evdate ) {
                min = evdate;
                minev = (*it);
            }
        }
    }

    dt = min;
    return minev;
}
