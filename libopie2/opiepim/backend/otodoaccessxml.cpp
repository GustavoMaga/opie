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
#include <errno.h>
#include <fcntl.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>


#include <qfile.h>
#include <qvector.h>

#include <qpe/global.h>
#include <qpe/stringutil.h>
#include <qpe/timeconversion.h>

#include <opie2/oconversion.h>
#include <opie2/opimstate.h>
#include <opie2/otimezone.h>
#include <opie2/opimnotifymanager.h>
#include <opie2/orecur.h>
#include <opie2/otodoaccessxml.h>

using namespace Opie;

namespace {
    time_t rp_end;
    ORecur* rec;
    ORecur *recur() {
        if (!rec ) rec = new ORecur;
        return rec;
    }
    int snd;
    enum MoreAttributes {
        FRType = OTodo::CompletedDate + 2,
        FRWeekdays,
        FRPosition,
        FRFreq,
        FRHasEndDate,
        FREndDate,
        FRStart,
        FREnd
    };
    // FROM TT again
char *strstrlen(const char *haystack, int hLen, const char* needle, int nLen)
{
    char needleChar;
    char haystackChar;
    if (!needle || !haystack || !hLen || !nLen)
	return 0;

    const char* hsearch = haystack;

    if ((needleChar = *needle++) != 0) {
	nLen--; //(to make up for needle++)
	do {
	    do {
		if ((haystackChar = *hsearch++) == 0)
		    return (0);
		if (hsearch >= haystack + hLen)
		    return (0);
	    } while (haystackChar != needleChar);
	} while (strncmp(hsearch, needle, QMIN(hLen - (hsearch - haystack), nLen)) != 0);
	hsearch--;
    }
    return ((char *)hsearch);
}
}

namespace Opie {

OTodoAccessXML::OTodoAccessXML( const QString& appName,
                                const QString& fileName )
    : OTodoAccessBackend(), m_app( appName ),  m_opened( false ), m_changed( false )
{
    if (!fileName.isEmpty() )
        m_file = fileName;
    else
        m_file = Global::applicationFileName( "todolist", "todolist.xml" );
}
OTodoAccessXML::~OTodoAccessXML() {

}
bool OTodoAccessXML::load() {
    rec = 0;
    m_opened = true;
    m_changed = false;
    /* initialize dict */
    /*
     * UPDATE dict if you change anything!!!
     */
    QAsciiDict<int> dict(26);
    dict.setAutoDelete( TRUE );
    dict.insert("Categories" ,     new int(OTodo::Category)         );
    dict.insert("Uid" ,            new int(OTodo::Uid)              );
    dict.insert("HasDate" ,        new int(OTodo::HasDate)          );
    dict.insert("Completed" ,      new int(OTodo::Completed)        );
    dict.insert("Description" ,    new int(OTodo::Description)      );
    dict.insert("Summary" ,        new int(OTodo::Summary)          );
    dict.insert("Priority" ,       new int(OTodo::Priority)         );
    dict.insert("DateDay" ,        new int(OTodo::DateDay)          );
    dict.insert("DateMonth" ,      new int(OTodo::DateMonth)        );
    dict.insert("DateYear" ,       new int(OTodo::DateYear)         );
    dict.insert("Progress" ,       new int(OTodo::Progress)         );
    dict.insert("CompletedDate",   new int(OTodo::CompletedDate)    );
    dict.insert("StartDate",       new int(OTodo::StartDate)        );
    dict.insert("CrossReference",  new int(OTodo::CrossReference)   );
    dict.insert("State",           new int(OTodo::State)            );
    dict.insert("Alarms",          new int(OTodo::Alarms)           );
    dict.insert("Reminders",       new int(OTodo::Reminders)        );
    dict.insert("Notifiers",       new int(OTodo::Notifiers)        );
    dict.insert("Maintainer",      new int(OTodo::Maintainer)       );
    dict.insert("rtype",           new int(FRType)                  );
    dict.insert("rweekdays",       new int(FRWeekdays)              );
    dict.insert("rposition",       new int(FRPosition)              );
    dict.insert("rfreq",           new int(FRFreq)                  );
    dict.insert("start",           new int(FRStart)                 );
    dict.insert("rhasenddate",     new int(FRHasEndDate)            );
    dict.insert("enddt",           new int(FREndDate)               );

    // here the custom XML parser from TT it's GPL
    // but we want to push OpiePIM... to TT.....
    // mmap part from zecke :)
    int fd = ::open( QFile::encodeName(m_file).data(), O_RDONLY );
    struct stat attribut;
    if ( fd < 0 ) return false;

    if ( fstat(fd, &attribut ) == -1 ) {
        ::close( fd );
        return false;
    }
    void* map_addr = ::mmap(NULL,  attribut.st_size, PROT_READ, MAP_SHARED, fd, 0 );
    if ( map_addr == ( (caddr_t)-1) ) {
        ::close(fd );
        return false;
    }
    /* advise the kernel who we want to read it */
    ::madvise( map_addr,  attribut.st_size,  MADV_SEQUENTIAL );
    /* we do not the file any more */
    ::close( fd );

    char* dt = (char*)map_addr;
    int len = attribut.st_size;
    int i = 0;
    char *point;
    const char* collectionString = "<Task ";
    int strLen = strlen(collectionString);
    while ( ( point = strstrlen( dt+i, len -i, collectionString, strLen ) ) != 0l ) {
        i = point -dt;
        i+= strLen;
        qWarning("Found a start at %d %d", i,  (point-dt) );

        OTodo ev;
        m_year = m_month = m_day = 0;

        while ( TRUE ) {
            while ( i < len && (dt[i] == ' ' || dt[i] == '\n' || dt[i] == '\r') )
		++i;
	    if ( i >= len-2 || (dt[i] == '/' && dt[i+1] == '>') )
		break;

	    // we have another attribute, read it.
	    int j = i;
	    while ( j < len && dt[j] != '=' )
		++j;
	    QCString attr( dt+i, j-i+1);

	    i = ++j; // skip =

	    // find the start of quotes
	    while ( i < len && dt[i] != '"' )
		++i;
	    j = ++i;

	    bool haveUtf = FALSE;
	    bool haveEnt = FALSE;
	    while ( j < len && dt[j] != '"' ) {
		if ( ((unsigned char)dt[j]) > 0x7f )
		    haveUtf = TRUE;
		if ( dt[j] == '&' )
		    haveEnt = TRUE;
		++j;
	    }
	    if ( i == j ) {
		// empty value
		i = j + 1;
		continue;
	    }

	    QCString value( dt+i, j-i+1 );
	    i = j + 1;

	    QString str = (haveUtf ? QString::fromUtf8( value )
		    : QString::fromLatin1( value ) );
	    if ( haveEnt )
		str = Qtopia::plainString( str );

            /*
             * add key + value
             */
            todo( &dict, ev, attr, str );

        }
        /*
         * now add it
         */
        qWarning("End at %d", i );
        if (m_events.contains( ev.uid() ) || ev.uid() == 0) {
            ev.setUid( 1 );
            m_changed = true;
        }
        if ( ev.hasDueDate() ) {
            ev.setDueDate( QDate(m_year, m_month, m_day) );
        }
        if ( rec && rec->doesRecur() ) {
            OTimeZone utc = OTimeZone::utc();
            ORecur recu( *rec ); // call copy c'tor
            recu.setEndDate( utc.fromUTCDateTime( rp_end ).date() );
            recu.setStart( ev.dueDate() );
            ev.setRecurrence( recu );
        }
        m_events.insert(ev.uid(), ev );
        m_year = m_month = m_day = -1;
        delete rec;
        rec = 0;
    }

    munmap(map_addr, attribut.st_size );

    qWarning("counts %d records loaded!", m_events.count() );
    return true;
}
bool OTodoAccessXML::reload() {
    m_events.clear();
    return load();
}
bool OTodoAccessXML::save() {
//    qWarning("saving");
    if (!m_opened || !m_changed ) {
//        qWarning("not saving");
        return true;
    }
    QString strNewFile = m_file + ".new";
    QFile f( strNewFile );
    if (!f.open( IO_WriteOnly|IO_Raw ) )
        return false;

    int written;
    QString out;
    out = "<!DOCTYPE Tasks>\n<Tasks>\n";

    // for all todos
    QMap<int, OTodo>::Iterator it;
    for (it = m_events.begin(); it != m_events.end(); ++it ) {
        out+= "<Task " + toString( (*it) ) + " />\n";
        QCString cstr = out.utf8();
        written = f.writeBlock( cstr.data(),  cstr.length() );

        /* less written then we wanted */
        if ( written != (int)cstr.length() ) {
            f.close();
            QFile::remove( strNewFile );
            return false;
        }
        out = QString::null;
    }

    out +=  "</Tasks>";
    QCString cstr = out.utf8();
    written = f.writeBlock( cstr.data(), cstr.length() );

    if ( written != (int)cstr.length() ) {
        f.close();
        QFile::remove( strNewFile );
        return false;
    }
    /* flush before renaming */
    f.close();

    if( ::rename( strNewFile.latin1(),  m_file.latin1() ) < 0 ) {
//        qWarning("error renaming");
        QFile::remove( strNewFile );
    }

    m_changed = false;
    return true;
}
QArray<int> OTodoAccessXML::allRecords()const {
    QArray<int> ids( m_events.count() );
    QMap<int, OTodo>::ConstIterator it;
    int i = 0;

    for ( it = m_events.begin(); it != m_events.end(); ++it ) {
        ids[i] = it.key();
        i++;
    }
    return ids;
}
QArray<int> OTodoAccessXML::queryByExample( const OTodo&, int, const QDateTime& ) {
    QArray<int> ids(0);
    return ids;
}
OTodo OTodoAccessXML::find( int uid )const {
    OTodo todo;
    todo.setUid( 0 ); // isEmpty()
    QMap<int, OTodo>::ConstIterator it = m_events.find( uid );
    if ( it != m_events.end() )
        todo = it.data();

    return todo;
}
void OTodoAccessXML::clear() {
    if (m_opened )
        m_changed = true;

    m_events.clear();
}
bool OTodoAccessXML::add( const OTodo& todo ) {
//    qWarning("add");
    m_changed = true;
    m_events.insert( todo.uid(), todo );

    return true;
}
bool OTodoAccessXML::remove( int uid ) {
    m_changed = true;
    m_events.remove( uid );

    return true;
}
bool OTodoAccessXML::replace( const OTodo& todo) {
    m_changed = true;
    m_events.replace( todo.uid(), todo );

    return true;
}
QArray<int> OTodoAccessXML::effectiveToDos( const QDate& start,
                                            const QDate& end,
                                            bool includeNoDates ) {
    QArray<int> ids( m_events.count() );
    QMap<int, OTodo>::Iterator it;

    int i = 0;
    for ( it = m_events.begin(); it != m_events.end(); ++it ) {
        if ( !it.data().hasDueDate() ) {
            if ( includeNoDates ) {
                ids[i] = it.key();
                i++;
            }
        }else if ( it.data().dueDate() >= start &&
                   it.data().dueDate() <= end ) {
            ids[i] = it.key();
            i++;
        }
    }
    ids.resize( i );
    return ids;
}
QArray<int> OTodoAccessXML::overDue() {
    QArray<int> ids( m_events.count() );
    int i = 0;

    QMap<int, OTodo>::Iterator it;
    for ( it = m_events.begin(); it != m_events.end(); ++it ) {
        if ( it.data().isOverdue() ) {
            ids[i] = it.key();
            i++;
        }
    }
    ids.resize( i );
    return ids;
}


/* private */
void OTodoAccessXML::todo( QAsciiDict<int>* dict, OTodo& ev,
                            const QCString& attr, const QString& val) {
//    qWarning("parse to do from XMLElement" );

    int *find=0;

    find = (*dict)[ attr.data() ];
    if (!find ) {
//            qWarning("Unknown option" + it.key() );
        ev.setCustomField( attr, val );
        return;
    }

    switch( *find ) {
    case OTodo::Uid:
        ev.setUid( val.toInt() );
        break;
    case OTodo::Category:
        ev.setCategories( ev.idsFromString( val ) );
        break;
    case OTodo::HasDate:
        ev.setHasDueDate( val.toInt() );
        break;
    case OTodo::Completed:
        ev.setCompleted( val.toInt() );
        break;
    case OTodo::Description:
        ev.setDescription( val );
        break;
    case OTodo::Summary:
        ev.setSummary( val );
        break;
    case OTodo::Priority:
        ev.setPriority( val.toInt() );
        break;
    case OTodo::DateDay:
        m_day = val.toInt();
        break;
    case OTodo::DateMonth:
        m_month = val.toInt();
        break;
    case OTodo::DateYear:
        m_year = val.toInt();
        break;
    case OTodo::Progress:
        ev.setProgress( val.toInt() );
        break;
    case OTodo::CompletedDate:
        ev.setCompletedDate( OConversion::dateFromString( val ) );
        break;
    case OTodo::StartDate:
        ev.setStartDate( OConversion::dateFromString( val ) );
        break;
    case OTodo::State:
        ev.setState( val.toInt() );
        break;
    case OTodo::Alarms:{
        OPimNotifyManager &manager = ev.notifiers();
        QStringList als = QStringList::split(";", val );
        for (QStringList::Iterator it = als.begin(); it != als.end(); ++it ) {
            QStringList alarm = QStringList::split(":", (*it), TRUE ); // allow empty
            qWarning("alarm: %s", alarm.join("___").latin1() );
            qWarning("alarm[0]: %s %s", alarm[0].latin1(), OConversion::dateTimeFromString( alarm[0] ).toString().latin1() );
            OPimAlarm al( alarm[2].toInt(), OConversion::dateTimeFromString( alarm[0] ), alarm[1].toInt() );
            manager.add( al );
        }
    }
        break;
    case OTodo::Reminders:{
        OPimNotifyManager &manager = ev.notifiers();
        QStringList rems = QStringList::split(";", val );
        for (QStringList::Iterator it = rems.begin(); it != rems.end(); ++it ) {
            OPimReminder rem( (*it).toInt() );
            manager.add( rem );
        }
    }
        break;
    case OTodo::CrossReference:
    {
        /*
         * A cross refernce looks like
         * appname,id;appname,id
         * we need to split it up
         */
        QStringList  refs = QStringList::split(';', val );
        QStringList::Iterator strIt;
        for (strIt = refs.begin(); strIt != refs.end(); ++strIt ) {
            int pos = (*strIt).find(',');
            if ( pos > -1 )
                ; // ev.addRelation( (*strIt).left(pos),  (*strIt).mid(pos+1).toInt() );

        }
        break;
    }
    /* Recurrence stuff below + post processing later */
    case FRType:
        if ( val == "Daily" )
            recur()->setType( ORecur::Daily );
        else if ( val == "Weekly" )
            recur()->setType( ORecur::Weekly);
        else if ( val == "MonthlyDay" )
            recur()->setType( ORecur::MonthlyDay );
        else if ( val == "MonthlyDate" )
            recur()->setType( ORecur::MonthlyDate );
        else if ( val == "Yearly" )
            recur()->setType( ORecur::Yearly );
        else
            recur()->setType( ORecur::NoRepeat );
        break;
    case FRWeekdays:
        recur()->setDays( val.toInt() );
        break;
    case FRPosition:
        recur()->setPosition( val.toInt() );
        break;
    case FRFreq:
        recur()->setFrequency( val.toInt() );
        break;
    case FRHasEndDate:
        recur()->setHasEndDate( val.toInt() );
        break;
    case FREndDate: {
        rp_end = (time_t) val.toLong();
        break;
    }
    default:
        ev.setCustomField( attr, val );
        break;
    }
}

// from PalmtopRecord... GPL ### FIXME
namespace {
QString customToXml(const QMap<QString, QString>& customMap )
{
    //qWarning(QString("writing custom %1").arg(customMap.count()));
    QString buf(" ");
    for ( QMap<QString, QString>::ConstIterator cit = customMap.begin();
	    cit != customMap.end(); ++cit) {
// 	qWarning(".ITEM.");
	buf += cit.key();
	buf += "=\"";
	buf += Qtopia::escapeString(cit.data());
	buf += "\" ";
    }
    return buf;
}


}

QString OTodoAccessXML::toString( const OTodo& ev )const {
    QString str;

    str += "Completed=\"" + QString::number( ev.isCompleted() ) + "\" ";
    str += "HasDate=\"" + QString::number( ev.hasDueDate() ) + "\" ";
    str += "Priority=\"" + QString::number( ev.priority() ) + "\" ";
    str += "Progress=\"" + QString::number(ev.progress() ) + "\" ";

    str += "Categories=\"" + toString( ev.categories() ) + "\" ";
    str += "Description=\"" + Qtopia::escapeString( ev.description() ) + "\" ";
    str += "Summary=\"" + Qtopia::escapeString( ev.summary() ) + "\" ";

    if ( ev.hasDueDate() ) {
        str += "DateYear=\"" + QString::number( ev.dueDate().year() ) + "\" ";
        str += "DateMonth=\"" + QString::number( ev.dueDate().month() ) + "\" ";
        str += "DateDay=\"" + QString::number( ev.dueDate().day() ) + "\" ";
    }
//    qWarning( "Uid %d",  ev.uid() );
    str += "Uid=\"" + QString::number( ev.uid() ) + "\" ";

// append the extra options
    /* FIXME Qtopia::Record this is currently not
     * possible you can set custom fields
     * but don' iterate over the list
     * I may do #define private protected
     * for this case - cough  --zecke
     */
    /*
    QMap<QString, QString> extras = ev.extras();
    QMap<QString, QString>::Iterator extIt;
    for (extIt = extras.begin(); extIt != extras.end(); ++extIt )
        str += extIt.key() + "=\"" +  extIt.data() + "\" ";
    */
    // cross refernce
    if ( ev.hasRecurrence() ) {
        str += ev.recurrence().toString();
    }
    if ( ev.hasStartDate() )
        str += "StartDate=\""+ OConversion::dateToString( ev.startDate() ) +"\" ";
    if ( ev.hasCompletedDate() )
        str += "CompletedDate=\""+ OConversion::dateToString( ev.completedDate() ) +"\" ";
    if ( ev.hasState() )
        str += "State=\""+QString::number( ev.state().state() )+"\" ";

    /*
     * save reminders and notifiers!
     * DATE_TIME:DURATION:SOUND:NOT_USED_YET;OTHER_DATE_TIME:OTHER_DURATION:SOUND:....
     */
    if ( ev.hasNotifiers() ) {
        OPimNotifyManager manager = ev.notifiers();
        OPimNotifyManager::Alarms alarms = manager.alarms();
        if (!alarms.isEmpty() ) {
            QStringList als;
            OPimNotifyManager::Alarms::Iterator it = alarms.begin();
            for ( ; it != alarms.end(); ++it ) {
                /* only if time is valid */
                if ( (*it).dateTime().isValid() ) {
                    als << OConversion::dateTimeToString( (*it).dateTime() )
                        + ":" + QString::number( (*it).duration() )
                        + ":" + QString::number( (*it).sound() )
                        + ":";
                }
            }
            // now write the list
            qWarning("als: %s", als.join("____________").latin1() );
            str += "Alarms=\""+als.join(";") +"\" ";
        }

        /*
         * now the same for reminders but more easy. We just save the uid of the OEvent.
         */
        OPimNotifyManager::Reminders reminders = manager.reminders();
        if (!reminders.isEmpty() ) {
            OPimNotifyManager::Reminders::Iterator it = reminders.begin();
            QStringList records;
            for ( ; it != reminders.end(); ++it ) {
                records << QString::number( (*it).recordUid() );
            }
            str += "Reminders=\""+ records.join(";") +"\" ";
        }
    }
    str += customToXml( ev.toExtraMap() );


    return str;
}
QString OTodoAccessXML::toString( const QArray<int>& ints ) const {
    return Qtopia::Record::idsToString( ints );
}

/* internal class for sorting
 *
 * Inspired by todoxmlio.cpp from TT
 */

struct OTodoXMLContainer {
    OTodo todo;
};

namespace {
    inline QString string( const OTodo& todo) {
        return  todo.summary().isEmpty() ?
            todo.description().left(20 ) :
            todo.summary();
    }
    inline int completed( const OTodo& todo1, const OTodo& todo2) {
        int ret = 0;
        if ( todo1.isCompleted() ) ret++;
        if ( todo2.isCompleted() ) ret--;
        return ret;
    }
    inline int priority( const OTodo& t1, const OTodo& t2) {
        return ( t1.priority() - t2.priority() );
    }
    inline int description( const OTodo& t1, const OTodo& t2) {
        return QString::compare( string(t1), string(t2) );
    }
    inline int deadline( const OTodo& t1, const OTodo& t2) {
        int ret = 0;
        if ( t1.hasDueDate() &&
             t2.hasDueDate() )
            ret = t2.dueDate().daysTo( t1.dueDate() );
        else if ( t1.hasDueDate() )
            ret = -1;
        else if ( t2.hasDueDate() )
            ret = 1;
        else
            ret = 0;

        return ret;
    }

};

/*
   * Returns:
   *       0 if item1 == item2
   *
   *   non-zero if item1 != item2
   *
   *   This function returns int rather than bool so that reimplementations
   *   can return one of three values and use it to sort by:
   *
   *   0 if item1 == item2
   *
   *   > 0 (positive integer) if item1 > item2
   *
   *   < 0 (negative integer) if item1 < item2
   *
   */
class OTodoXMLVector : public QVector<OTodoXMLContainer> {
public:
    OTodoXMLVector(int size, bool asc,  int sort)
        : QVector<OTodoXMLContainer>( size )
        {
            setAutoDelete( true );
            m_asc = asc;
            m_sort = sort;
        }
        /* return the summary/description */
    QString string( const OTodo& todo) {
        return  todo.summary().isEmpty() ?
            todo.description().left(20 ) :
            todo.summary();
    }
    /**
     * we take the sortorder( switch on it )
     *
     */
    int compareItems( Item d1, Item d2 ) {
        bool seComp, sePrio, seDesc, seDeadline;
        seComp = sePrio = seDeadline = seDesc = false;
        int ret =0;
        OTodoXMLContainer* con1 = (OTodoXMLContainer*)d1;
        OTodoXMLContainer* con2 = (OTodoXMLContainer*)d2;

        /* same item */
        if ( con1->todo.uid() == con2->todo.uid() )
            return 0;

        switch ( m_sort ) {
            /* completed */
        case 0: {
            ret = completed( con1->todo, con2->todo );
            seComp = TRUE;
            break;
        }
            /* priority */
        case 1: {
            ret = priority( con1->todo, con2->todo );
            sePrio = TRUE;
            break;
        }
            /* description */
        case 2: {
            ret  = description( con1->todo, con2->todo );
            seDesc = TRUE;
            break;
        }
            /* deadline */
        case 3: {
            ret = deadline( con1->todo, con2->todo );
            seDeadline = TRUE;
            break;
        }
        default:
            ret = 0;
            break;
        };
        /*
         * FIXME do better sorting if the first sort criteria
         * ret equals 0 start with complete and so on...
         */

        /* twist it we're not ascending*/
        if (!m_asc)
            ret = ret * -1;

        if ( ret )
            return ret;

        // default did not gave difference let's try it other way around
        /*
         * General try if already checked if not test
         * and return
         * 1.Completed
         * 2.Priority
         * 3.Description
         * 4.DueDate
         */
        if (!seComp ) {
            if ( (ret = completed( con1->todo, con2->todo ) ) ) {
                if (!m_asc ) ret *= -1;
                return ret;
            }
        }
        if (!sePrio ) {
            if ( (ret = priority( con1->todo, con2->todo ) ) ) {
                if (!m_asc ) ret *= -1;
                return ret;
            }
        }
        if (!seDesc ) {
            if ( (ret = description(con1->todo, con2->todo ) ) ) {
                if (!m_asc) ret *= -1;
                return ret;
            }
        }
        if (!seDeadline) {
            if ( (ret = deadline( con1->todo, con2->todo ) ) ) {
                if (!m_asc) ret *= -1;
                return ret;
            }
        }

        return 0;
    }
 private:
    bool m_asc;
    int m_sort;

};

QArray<int> OTodoAccessXML::sorted( bool asc,  int sortOrder,
                                    int sortFilter,  int cat ) {
    OTodoXMLVector vector(m_events.count(), asc,sortOrder );
    QMap<int, OTodo>::Iterator it;
    int item = 0;

    bool bCat = sortFilter & 1 ? true : false;
    bool bOnly = sortFilter & 2 ? true : false;
    bool comp = sortFilter & 4 ? true : false;
    for ( it = m_events.begin(); it != m_events.end(); ++it ) {

        /* show category */
        /* -1 == unfiled */
        if ( bCat && cat == -1 ) {
            if(!(*it).categories().isEmpty() )
                continue;
        }else if ( bCat && cat != 0)
            if (!(*it).categories().contains( cat ) ) {
                continue;
            }
        /* isOverdue but we should not show overdue - why?*/
/*        if ( (*it).isOverdue() &&  !bOnly  ) {
            qWarning("item is overdue but !bOnly");
            continue;
        }
*/
        if ( !(*it).isOverdue() && bOnly ) {
            continue;
        }

        if ((*it).isCompleted() && comp ) {
            continue;
        }


        OTodoXMLContainer* con = new OTodoXMLContainer();
        con->todo = (*it);
        vector.insert(item, con );
        item++;
    }
    vector.resize( item );
    /* sort it now */
    vector.sort();
    /* now get the uids */
    QArray<int> array( vector.count() );
    for (uint i= 0; i < vector.count(); i++ ) {
        array[i] = ( vector.at(i) )->todo.uid();
    }
    return array;
};
void OTodoAccessXML::removeAllCompleted() {
    QMap<int, OTodo> events = m_events;
    for ( QMap<int, OTodo>::Iterator it = m_events.begin(); it != m_events.end(); ++it ) {
        if ( (*it).isCompleted() )
            events.remove( it.key() );
    }
    m_events = events;
}
QBitArray OTodoAccessXML::supports()const {
    static QBitArray ar = sup();
    return ar;
}
QBitArray OTodoAccessXML::sup() {
    QBitArray ar( OTodo::CompletedDate +1 );
    ar.fill( true );
    ar[OTodo::CrossReference] = false;
    ar[OTodo::State ] = false;
    ar[OTodo::Reminders] = false;
    ar[OTodo::Notifiers] = false;
    ar[OTodo::Maintainer] = false;

    return ar;
}
QArray<int> OTodoAccessXML::matchRegexp(  const QRegExp &r ) const
{
	QArray<int> m_currentQuery( m_events.count() );
	uint arraycounter = 0;

        QMap<int, OTodo>::ConstIterator it;
        for (it = m_events.begin(); it != m_events.end(); ++it ) {
		if ( it.data().match( r ) )
			m_currentQuery[arraycounter++] = it.data().uid();

	}
	// Shrink to fit..
	m_currentQuery.resize(arraycounter);

	return m_currentQuery;
}

}
