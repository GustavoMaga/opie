/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers (Eilers.Stefan@epost.de)
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
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
#include <qtl.h>

#include <opie2/opimrecurrence.h>

#include <opie2/odatebookaccessbackend.h>

using namespace Opie;

namespace {
/* a small helper to get all NonRepeating events for a range of time */
    void events( OEffectiveEvent::ValueList& tmpList, const OPimEvent::ValueList& events,
                 const QDate& from, const QDate& to ) {
        QDateTime dtStart, dtEnd;

        for ( OPimEvent::ValueList::ConstIterator it = events.begin(); it != events.end(); ++it ) {
            dtStart = (*it).startDateTime();
            dtEnd   = (*it).endDateTime();

            /*
             * If in range
             */
            if (dtStart.date() >= from && dtEnd.date() <= to ) {
                OEffectiveEvent eff;
                eff.setEvent( (*it) );
                eff.setDate( dtStart.date() );
                eff.setStartTime( dtStart.time() );

                /* if not on the same day */
                if ( dtStart.date() != dtEnd.date() )
                    eff.setEndTime( QTime(23, 59, 0 ) );
                else
                    eff.setEndTime( dtEnd.time() );

                tmpList.append( eff );
            }

            /* we must also check for end date information... */
            if ( dtEnd.date() != dtStart.date() && dtEnd.date() >= from ) {
                QDateTime dt = dtStart.addDays( 1 );
                dt.setTime( QTime(0, 0, 0 ) );
                QDateTime dtStop;
                if ( dtEnd > to )
                    dtStop = to;
                else
                    dtStop = dtEnd;

                while ( dt <= dtStop ) {
                    OEffectiveEvent eff;
                    eff.setEvent( (*it) );
                    eff.setDate( dt.date() );

                    if ( dt >= from ) {
                        eff.setStartTime( QTime(0, 0, 0 ) );
                        if ( dt.date() == dtEnd.date() )
                            eff.setEndTime( dtEnd.time() );
                        else
                            eff.setEndTime( QTime(23, 59, 0 ) );
                        tmpList.append( eff );
                    }
                    dt = dt.addDays( 1 );
                }
            }
        }
    }

    void repeat( OEffectiveEvent::ValueList& tmpList, const OPimEvent::ValueList& list,
                 const QDate& from, const QDate& to ) {
        QDate repeat;
        for ( OPimEvent::ValueList::ConstIterator it = list.begin(); it != list.end(); ++it ) {
            int dur = (*it).startDateTime().date().daysTo( (*it).endDateTime().date() );
            QDate itDate = from.addDays(-dur );
            OPimRecurrence rec = (*it).recurrence();
            if ( !rec.hasEndDate() || rec.endDate() > to ) {
                rec.setEndDate( to );
                rec.setHasEndDate( true );
            }
            while (rec.nextOcurrence(itDate, repeat ) ) {
                if (repeat > to ) break;
                OEffectiveEvent eff;
                eff.setDate( repeat );
                if ( (*it).isAllDay() ) {
                    eff.setStartTime( QTime(0, 0, 0 ) );
                    eff.setEndTime( QTime(23, 59, 59 ) );
                }else {
                    /* we only occur by days, not hours/minutes/seconds.  Hence
                     * the actual end and start times will be the same for
                     * every repeated event.  For multi day events this is
                     * fixed up later if on wronge day span
                     */
                    eff.setStartTime( (*it).startDateTime().time() );
                    eff.setEndTime( (*it).endDateTime().time() );
                }
                if ( dur != 0 ) {
                    // multi-day repeating events
                    QDate sub_it = QMAX( repeat, from );
                    QDate startDate = repeat;
                    QDate endDate = startDate.addDays( dur );

                    while ( sub_it <= endDate && sub_it <= to ) {
                        OEffectiveEvent tmpEff = eff;
                        tmpEff.setEvent( (*it) );
                        if ( sub_it != startDate )
                            tmpEff.setStartTime( QTime(0, 0, 0 ) );
                        if ( sub_it != endDate )
                            tmpEff.setEndTime( QTime( 23, 59, 59 ) );

                        tmpEff.setDate( sub_it );
                        tmpEff.setEffectiveDates( startDate, endDate );
                        tmpList.append( tmpEff );

                        sub_it = sub_it.addDays( 1 );
                    }
                    itDate = endDate;
                }else {
                    eff.setEvent( (*it) );
                    tmpList.append( eff );
                    itDate = repeat.addDays( 1 );
                }
            }
        }
    }
}

namespace Opie {

ODateBookAccessBackend::ODateBookAccessBackend()
    : OPimAccessBackend<OPimEvent>()
{

}
ODateBookAccessBackend::~ODateBookAccessBackend() {

}
OEffectiveEvent::ValueList ODateBookAccessBackend::effectiveEvents( const QDate& from,
                                                                    const QDate& to ) {
    OEffectiveEvent::ValueList tmpList;
    OPimEvent::ValueList list = directNonRepeats();

    events( tmpList, list, from, to );
    repeat( tmpList, directRawRepeats(),from,to );

    list = directRawRepeats();  // Useless, isn't it ? (eilers)

    qHeapSort( tmpList );
    return tmpList;
}
OEffectiveEvent::ValueList ODateBookAccessBackend::effectiveEvents( const QDateTime& dt ) {
    OEffectiveEvent::ValueList day = effectiveEvents( dt.date(), dt.date() );
    OEffectiveEvent::ValueList::Iterator it;

    OEffectiveEvent::ValueList tmpList;
    QDateTime dtTmp;
    for ( it = day.begin(); it != day.end(); ++it ) {
        dtTmp = QDateTime( (*it).date(), (*it).startTime() );
        if ( QABS(dt.secsTo(dtTmp) ) < 60 )
            tmpList.append( (*it) );
    }

    return tmpList;
}

OEffectiveEvent::ValueList ODateBookAccessBackend::effectiveNonRepeatingEvents( const QDate& from,
                                                                    const QDate& to ) {
    OEffectiveEvent::ValueList tmpList;
    OPimEvent::ValueList list = directNonRepeats();

    events( tmpList, list, from, to );

    qHeapSort( tmpList );
    return tmpList;
}

OEffectiveEvent::ValueList ODateBookAccessBackend::effectiveNonRepeatingEvents( const QDateTime& dt ) {
    OEffectiveEvent::ValueList day = effectiveNonRepeatingEvents( dt.date(), dt.date() );
    OEffectiveEvent::ValueList::Iterator it;

    OEffectiveEvent::ValueList tmpList;
    QDateTime dtTmp;
    for ( it = day.begin(); it != day.end(); ++it ) {
        dtTmp = QDateTime( (*it).date(), (*it).startTime() );
        if ( QABS(dt.secsTo(dtTmp) ) < 60 )
            tmpList.append( (*it) );
    }

    return tmpList;
}

}
