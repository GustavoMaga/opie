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

/* OPIE */
#include <opie2/opimnotifymanager.h>
#include <opie2/opimrecurrence.h>
#include <opie2/opimtimezone.h>
#include <opie2/odatebookaccessbackend_xml.h>
#include <opie2/odebug.h>

#include <qtopia/global.h>
#include <qtopia/stringutil.h>
#include <qtopia/timeconversion.h>

/* QT */
#include <qasciidict.h>
#include <qfile.h>

/* STD */
#include <errno.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <unistd.h>


using namespace Opie;

namespace {
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

namespace {
    time_t start, end, created, rp_end;
    OPimRecurrence* rec;
    static OPimRecurrence* recur() {
        if (!rec)
            rec = new OPimRecurrence;

        return rec;
    }
    int alarmTime;
    bool alarmSpecified;
    int snd;
    enum Attribute{
        FDescription = 0,
        FLocation,
        FCategories,
        FUid,
        FType,
        FAlarm,
        FSound,
        FRType,
        FRWeekdays,
        FRPosition,
        FRFreq,
        FRHasEndDate,
        FREndDate,
        FRStart,
        FREnd,
        FNote,
        FCreated,      // Should't this be called FRCreated ?
        FTimeZone,
        FRecParent,
        FRecChildren,
        FExceptions
    };

    // FIXME: Use OPimEvent::toMap() here !! (eilers)
    static void save( const OPimEvent& ev, QString& buf ) {
        buf += " description=\"" + Qtopia::escapeString(ev.description() ) + "\"";
        if (!ev.location().isEmpty() )
            buf += " location=\"" + Qtopia::escapeString(ev.location() ) + "\"";

        if (!ev.categories().isEmpty() )
            buf += " categories=\""+ Qtopia::escapeString( Qtopia::Record::idsToString( ev.categories() ) ) + "\"";

        buf += " uid=\"" + QString::number( ev.uid() ) + "\"";

        if (ev.isAllDay() )
            buf += " type=\"AllDay\""; // is that all ?? (eilers)

        if (ev.hasNotifiers() ) {
            OPimAlarm alarm = ev.notifiers().alarms()[0]; // take only the first
            int minutes = alarm.dateTime().secsTo( ev.startDateTime() ) / 60;
            buf += " alarm=\"" + QString::number(minutes) + "\" sound=\"";
            if ( alarm.sound() == OPimAlarm::Loud )
                buf += "loud";
            else
                buf += "silent";
            buf += "\"";
        }
        if ( ev.hasRecurrence() ) {
            buf += ev.recurrence().toString();
        }

        /*
         * fscking timezones :) well, we'll first convert
         * the QDateTime to a QDateTime in UTC time
         * and then we'll create a nice time_t
         */
        OPimTimeZone zone( (ev.timeZone().isEmpty()||ev.isAllDay()) ? OPimTimeZone::utc() : OPimTimeZone::current() );
        buf += " start=\"" + QString::number( zone.fromDateTime( ev.startDateTime()))  + "\"";
        buf += " end=\""   + QString::number( zone.fromDateTime( ev.endDateTime()  ))  + "\"";
        if (!ev.note().isEmpty() ) {
            buf += " note=\"" + Qtopia::escapeString( ev.note() ) + "\"";
        }

        /*
         * Don't save a timezone if AllDay Events
         * as they're UTC only anyway
         */
        if (!ev.isAllDay() ) {

            buf += " timezone=\"";
            if ( ev.timeZone().isEmpty() )
                buf += "None";
            else
                buf += ev.timeZone();
            buf += "\"";
        }

        if (ev.parent() != 0 ) {
            buf += " recparent=\""+QString::number(ev.parent() )+"\"";
        }

        if (ev.children().count() != 0 ) {
            QArray<int> children = ev.children();
            buf += " recchildren=\"";
            for ( uint i = 0; i < children.count(); i++ ) {
                if ( i != 0 ) buf += " ";
                buf += QString::number( children[i] );
            }
            buf+= "\"";
        }

        // skip custom writing
    }

    static bool saveEachEvent( const QMap<int, OPimEvent>& list, QFile& file ) {
        QMap<int, OPimEvent>::ConstIterator it;
        QString buf;
        QCString str;
        int total_written;
        for ( it = list.begin(); it != list.end(); ++it ) {
            buf = "<event";
            save( it.data(), buf );
            buf += " />\n";
            str = buf.utf8();

            total_written = file.writeBlock(str.data(), str.length() );
            if ( total_written != int(str.length() ) )
                return false;
        }
        return true;
    }
}

namespace Opie {
ODateBookAccessBackend_XML::ODateBookAccessBackend_XML( const QString& ,
                                                        const QString& fileName )
    : ODateBookAccessBackend() {
    m_name = fileName.isEmpty() ? Global::applicationFileName( "datebook", "datebook.xml" ) : fileName;
    m_changed = false;
}
ODateBookAccessBackend_XML::~ODateBookAccessBackend_XML() {
}
bool ODateBookAccessBackend_XML::load() {
    return loadFile();
}
bool ODateBookAccessBackend_XML::reload() {
    clear();
    return load();
}
bool ODateBookAccessBackend_XML::save() {
    if (!m_changed) return true;

    int total_written;
    QString strFileNew = m_name + ".new";

    QFile f( strFileNew );
    if (!f.open( IO_WriteOnly | IO_Raw ) ) return false;

    QString buf( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
    buf += "<!DOCTYPE DATEBOOK><DATEBOOK>\n";
    buf += "<events>\n";
    QCString str = buf.utf8();
    total_written = f.writeBlock( str.data(), str.length() );
    if ( total_written != int(str.length() ) ) {
        f.close();
        QFile::remove( strFileNew );
        return false;
    }

    if (!saveEachEvent( m_raw, f ) ) {
        f.close();
        QFile::remove( strFileNew );
        return false;
    }
    if (!saveEachEvent( m_rep, f ) ) {
        f.close();
        QFile::remove( strFileNew );
        return false;
    }

    buf = "</events>\n</DATEBOOK>\n";
    str = buf.utf8();
    total_written = f.writeBlock( str.data(), str.length() );
    if ( total_written != int(str.length() ) ) {
        f.close();
        QFile::remove( strFileNew );
        return false;
    }
    f.close();

    if ( ::rename( strFileNew, m_name ) < 0 ) {
        QFile::remove( strFileNew );
        return false;
    }

    m_changed = false;
    return true;
}
QArray<int> ODateBookAccessBackend_XML::allRecords()const {
    QArray<int> ints( m_raw.count()+ m_rep.count() );
    uint i = 0;
    QMap<int, OPimEvent>::ConstIterator it;

    for ( it = m_raw.begin(); it != m_raw.end(); ++it ) {
        ints[i] = it.key();
        i++;
    }
    for ( it = m_rep.begin(); it != m_rep.end(); ++it ) {
        ints[i] = it.key();
        i++;
    }

    return ints;
}
QArray<int> ODateBookAccessBackend_XML::queryByExample(const OPimEvent&, int,  const QDateTime& ) const {
    return QArray<int>();
}
void ODateBookAccessBackend_XML::clear() {
    m_changed = true;
    m_raw.clear();
    m_rep.clear();
}
OPimEvent ODateBookAccessBackend_XML::find( int uid ) const{
    if ( m_raw.contains( uid ) )
        return m_raw[uid];
    else
        return m_rep[uid];
}
bool ODateBookAccessBackend_XML::add( const OPimEvent& ev ) {
    m_changed = true;
    if (ev.hasRecurrence() )
        m_rep.insert( ev.uid(), ev );
    else
        m_raw.insert( ev.uid(), ev );

    return true;
}
bool ODateBookAccessBackend_XML::remove( int uid ) {
    m_changed = true;
    m_raw.remove( uid );
    m_rep.remove( uid );

    return true;
}
bool ODateBookAccessBackend_XML::replace( const OPimEvent& ev ) {
    remove( ev.uid() );
    return add( ev );
}

QArray<int> ODateBookAccessBackend_XML::rawRepeats()const {
    QArray<int> ints( m_rep.count() );
    uint i = 0;
    QMap<int, OPimEvent>::ConstIterator it;

    for ( it = m_rep.begin(); it != m_rep.end(); ++it ) {
        ints[i] = it.key();
        i++;
    }

    return ints;
}
QArray<int> ODateBookAccessBackend_XML::nonRepeats()const {
    QArray<int> ints( m_raw.count() );
    uint i = 0;
    QMap<int, OPimEvent>::ConstIterator it;

    for ( it = m_raw.begin(); it != m_raw.end(); ++it ) {
        ints[i] = it.key();
        i++;
    }

    return ints;
}
OPimEvent::ValueList ODateBookAccessBackend_XML::directNonRepeats()const {
    OPimEvent::ValueList list;
    QMap<int, OPimEvent>::ConstIterator it;
    for (it = m_raw.begin(); it != m_raw.end(); ++it )
        list.append( it.data() );

    return list;
}
OPimEvent::ValueList ODateBookAccessBackend_XML::directRawRepeats()const {
    OPimEvent::ValueList list;
    QMap<int, OPimEvent>::ConstIterator it;
    for (it = m_rep.begin(); it != m_rep.end(); ++it )
        list.append( it.data() );

    return list;
}

// FIXME: Use OPimEvent::fromMap() (eilers)
bool ODateBookAccessBackend_XML::loadFile() {
    m_changed = false;

    int fd = ::open( QFile::encodeName(m_name).data(), O_RDONLY );
    if ( fd < 0 ) return false;

    struct stat attribute;
    if ( ::fstat(fd, &attribute ) == -1 ) {
        ::close( fd );
        return false;
    }
    void* map_addr = ::mmap(NULL, attribute.st_size, PROT_READ, MAP_SHARED, fd, 0 );
    if ( map_addr == ( (caddr_t)-1) ) {
        ::close( fd );
        return false;
    }

    ::madvise( map_addr, attribute.st_size, MADV_SEQUENTIAL );
    ::close( fd );

    QAsciiDict<int> dict(FExceptions+1);
    dict.setAutoDelete( true );
    dict.insert( "description", new int(FDescription) );
    dict.insert( "location", new int(FLocation) );
    dict.insert( "categories", new int(FCategories) );
    dict.insert( "uid", new int(FUid) );
    dict.insert( "type", new int(FType) );
    dict.insert( "alarm", new int(FAlarm) );
    dict.insert( "sound", new int(FSound) );
    dict.insert( "rtype", new int(FRType) );
    dict.insert( "rweekdays", new int(FRWeekdays) );
    dict.insert( "rposition", new int(FRPosition) );
    dict.insert( "rfreq", new int(FRFreq) );
    dict.insert( "rhasenddate", new int(FRHasEndDate) );
    dict.insert( "enddt", new int(FREndDate) );
    dict.insert( "start", new int(FRStart) );
    dict.insert( "end", new int(FREnd) );
    dict.insert( "note", new int(FNote) );
    dict.insert( "created", new int(FCreated) );  // Shouldn't this be FRCreated ??
    dict.insert( "recparent", new int(FRecParent) );
    dict.insert( "recchildren", new int(FRecChildren) );
    dict.insert( "exceptions", new int(FExceptions) );
    dict.insert( "timezone", new int(FTimeZone) );


    // initialiaze db hack
    m_noTimeZone = true;

    char* dt = (char*)map_addr;
    int len = attribute.st_size;
    int i = 0;
    char* point;
    const char* collectionString = "<event ";
    int strLen = ::strlen(collectionString);
    int *find;
    while ( (  point = ::strstrlen( dt+i, len -i, collectionString, strLen ) ) != 0  ) {
        i = point -dt;
        i+= strLen;

        alarmTime = -1;
        alarmSpecified = FALSE;
        snd = 0; // silent

        OPimEvent ev;
        rec = 0;

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
            find = dict[attr.data()];
            if (!find)
                ev.setCustomField( attr, str );
            else {
                setField( ev, *find, str );
            }
        }
        /* time to finalize */
        finalizeRecord( ev );
        delete rec;
        m_noTimeZone = true;
    }
    ::munmap(map_addr, attribute.st_size );
    m_changed = false; // changed during add

    return true;
}

// FIXME: Use OPimEvent::fromMap() which makes this obsolete.. (eilers)
void ODateBookAccessBackend_XML::finalizeRecord( OPimEvent& ev ) {

    /*
     * quirk to import datebook files. They normally don't have a
     * timeZone attribute and we treat this as to use OPimTimeZone::current()
     */
    if (m_noTimeZone )
        ev.setTimeZone( OPimTimeZone::current().timeZone() );



    /* AllDay is alway in UTC */
    if ( ev.isAllDay() ) {
        OPimTimeZone utc = OPimTimeZone::utc();
        ev.setStartDateTime( utc.toDateTime( start ) );
        ev.setEndDateTime  ( utc.toDateTime( end   ) );
    }else {
        /* to current date time */
        OPimTimeZone   to_zone( ev.timeZone().isEmpty() ? OPimTimeZone::utc() : OPimTimeZone::current() );

        ev.setStartDateTime(to_zone.toDateTime( start));
        ev.setEndDateTime  (to_zone.toDateTime( end));
    }
    if ( rec && rec->doesRecur() ) {
        OPimTimeZone utc = OPimTimeZone::utc();
        OPimRecurrence recu( *rec ); // call copy c'tor;
        recu.setEndDate ( utc.toDateTime( rp_end ).date() );
        recu.setCreatedDateTime( utc.toDateTime( created ) );
        recu.setStart( ev.startDateTime().date() );
        ev.setRecurrence( recu );
    }

    if ( alarmSpecified ) {
        QDateTime dt = ev.startDateTime().addSecs( -1*alarmTime*60 );
        OPimAlarm al( snd ,  dt  );
        ev.notifiers().add( al );
    }
    if ( m_raw.contains( ev.uid() ) || m_rep.contains( ev.uid() ) ) {
        ev.setUid( 1 );
    }

    if ( ev.hasRecurrence() )
        m_rep.insert( ev.uid(), ev );
    else
        m_raw.insert( ev.uid(), ev );

}
void ODateBookAccessBackend_XML::setField( OPimEvent& e, int id, const QString& value) {
    switch( id ) {
    case FDescription:
        e.setDescription( value );
        break;
    case FLocation:
        e.setLocation( value );
        break;
    case FCategories:
        e.setCategories( e.idsFromString( value ) );
        break;
    case FUid:
        e.setUid( value.toInt() );
        break;
    case FType:
        if ( value == "AllDay" ) {
            e.setAllDay( true );
        }
        break;
    case FAlarm:
        alarmTime = value.toInt();
        alarmSpecified = TRUE;
        break;
    case FSound:
        snd = value == "loud" ? OPimAlarm::Loud : OPimAlarm::Silent;
        break;
        // recurrence stuff
    case FRType:
        if ( value == "Daily" )
            recur()->setType( OPimRecurrence::Daily );
        else if ( value == "Weekly" )
            recur()->setType( OPimRecurrence::Weekly);
        else if ( value == "MonthlyDay" )
            recur()->setType( OPimRecurrence::MonthlyDay );
        else if ( value == "MonthlyDate" )
            recur()->setType( OPimRecurrence::MonthlyDate );
        else if ( value == "Yearly" )
            recur()->setType( OPimRecurrence::Yearly );
        else
            recur()->setType( OPimRecurrence::NoRepeat );
        break;
    case FRWeekdays:
        recur()->setDays( value.toInt() );
        break;
    case FRPosition:
        recur()->setPosition( value.toInt() );
        break;
    case FRFreq:
        recur()->setFrequency( value.toInt() );
        break;
    case FRHasEndDate:
        recur()->setHasEndDate( value.toInt() );
        break;
    case FREndDate: {
        rp_end = (time_t) value.toLong();
        break;
    }
    case FRStart: {
        start =  (time_t) value.toLong();
        break;
    }
    case FREnd: {
        end = ( (time_t) value.toLong() );
        break;
    }
    case FNote:
        e.setNote( value );
        break;
    case FCreated:
        created = value.toInt();
        break;
    case FRecParent:
        e.setParent( value.toInt() );
        break;
    case FRecChildren:{
        QStringList list = QStringList::split(' ', value );
        for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
            e.addChild( (*it).toInt() );
        }
    }
        break;
    case FExceptions:{
        QStringList list = QStringList::split(' ', value );
        for (QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
            QDate date( (*it).left(4).toInt(), (*it).mid(4, 2).toInt(), (*it).right(2).toInt() );
            recur()->addException( date );
        }
    }
        break;
    case FTimeZone:
        m_noTimeZone = false;
        if ( value != "None" )
            e.setTimeZone( value );
        break;
    default:
        break;
    }
}
QArray<int> ODateBookAccessBackend_XML::matchRegexp(  const QRegExp &r ) const
{
    QArray<int> m_currentQuery( m_raw.count()+ m_rep.count() );
    uint arraycounter = 0;
    QMap<int, OPimEvent>::ConstIterator it;

    for ( it = m_raw.begin(); it != m_raw.end(); ++it )
        if ( it.data().match( r ) )
            m_currentQuery[arraycounter++] = it.data().uid();
    for ( it = m_rep.begin(); it != m_rep.end(); ++it )
        if ( it.data().match( r ) )
            m_currentQuery[arraycounter++] = it.data().uid();

    // Shrink to fit..
    m_currentQuery.resize(arraycounter);

    return m_currentQuery;
}

}
