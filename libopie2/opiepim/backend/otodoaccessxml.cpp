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
#include <opie2/opimdateconversion.h>
#include <opie2/opimstate.h>
#include <opie2/opimtimezone.h>
#include <opie2/opimnotifymanager.h>
#include <opie2/otodoaccessxml.h>
#include <opie2/otodoaccess.h>
#include <opie2/odebug.h>

#include <opie2/private/opimtodosortvector.h>

#include <qpe/global.h>
#include <qpe/stringutil.h>
#include <qpe/timeconversion.h>

/* QT */
#include <qfile.h>
#include <qvector.h>
#include <qintdict.h>

/* STD */
#include <errno.h>
#include <fcntl.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

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

namespace Opie {

OPimTodoAccessXML::OPimTodoAccessXML( const QString& appName,
                                const QString& fileName )
    : OPimTodoAccessBackend(), m_app( appName ),  m_opened( false ), m_changed( false )
{
    if (!fileName.isEmpty() )
        m_file = fileName;
    else
        m_file = Global::applicationFileName( "todolist", "todolist.xml" );
}

OPimTodoAccessXML::~OPimTodoAccessXML() {

}

void OPimTodoAccessXML::initDict( QAsciiDict<int> &dict ) const
{
    /*
     * UPDATE this if you change anything!!!
     */
    dict.setAutoDelete( TRUE );
    dict.insert("Categories" ,     new int(OPimTodo::Category)         );
    dict.insert("Uid" ,            new int(OPimTodo::Uid)              );
    dict.insert("HasDate" ,        new int(OPimTodo::HasDate)          );
    dict.insert("Completed" ,      new int(OPimTodo::Completed)        );
    dict.insert("Description" ,    new int(OPimTodo::Description)      );
    dict.insert("Summary" ,        new int(OPimTodo::Summary)          );
    dict.insert("Priority" ,       new int(OPimTodo::Priority)         );
    dict.insert("DateDay" ,        new int(OPimTodo::DateDay)          );
    dict.insert("DateMonth" ,      new int(OPimTodo::DateMonth)        );
    dict.insert("DateYear" ,       new int(OPimTodo::DateYear)         );
    dict.insert("Progress" ,       new int(OPimTodo::Progress)         );
    dict.insert("CompletedDate",   new int(OPimTodo::CompletedDate)    );
    dict.insert("StartDate",       new int(OPimTodo::StartDate)        );
    dict.insert("CrossReference",  new int(OPimTodo::CrossReference)   );
    dict.insert("State",           new int(OPimTodo::State)            );
    dict.insert("Alarms",          new int(OPimTodo::Alarms)           );
    dict.insert("Reminders",       new int(OPimTodo::Reminders)        );
    dict.insert("Maintainer",      new int(OPimTodo::Maintainer)       );
    dict.insert("rtype",           new int(OPimTodo::FRType)           );
    dict.insert("rweekdays",       new int(OPimTodo::FRWeekdays)       );
    dict.insert("rposition",       new int(OPimTodo::FRPosition)       );
    dict.insert("rfreq",           new int(OPimTodo::FRFreq)           );
    dict.insert("start",           new int(OPimTodo::FRStart)          );
    dict.insert("rhasenddate",     new int(OPimTodo::FRHasEndDate)     );
    dict.insert("enddt",           new int(OPimTodo::FREndDate)        );
}

bool OPimTodoAccessXML::load() {
    m_opened = true;
    m_changed = false;
    /* initialize dict */
    QAsciiDict<int> dict(26);
    initDict(dict);

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
    QMap<int, QString> map;
    int *find;
    while ( ( point = strstrlen( dt+i, len -i, collectionString, strLen ) ) != 0l ) {
        i = point -dt;
        i+= strLen;

        OPimTodo ev;

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
            else
                map[*find] = str;
        }

        /*
         * now add it
         */
        ev.fromMap( map );
        if (m_events.contains( ev.uid() ) || ev.uid() == 0) {
            ev.setUid( 1 );
            m_changed = true;
        }

        m_events.insert(ev.uid(), ev );
    }

    munmap(map_addr, attribut.st_size );

    return true;
}

bool OPimTodoAccessXML::reload() {
    m_events.clear();
    return load();
}

bool OPimTodoAccessXML::save() {
    if (!m_opened || !m_changed ) {
        return true;
    }
    QString strNewFile = m_file + ".new";
    QFile f( strNewFile );
    if (!f.open( IO_WriteOnly|IO_Raw ) )
        return false;

    int written;
    QString out;
    out = "<!DOCTYPE Tasks>\n<Tasks>\n";

    QAsciiDict<int> dict(26);
    initDict( dict );
    QIntDict<QString> revdict( dict.size() );
    dict.setAutoDelete( true );
    // Now we need to reverse the dictionary (!)
    for( QAsciiDictIterator<int> it( dict ); it.current(); ++it ) {
        revdict.insert( (*it), new QString( it.currentKey() ) );
    }

    // for all todos
    QMap<int, OPimTodo>::Iterator it;
    for (it = m_events.begin(); it != m_events.end(); ++it ) {
        out+= "<Task " + toString( (*it), revdict ) + " />\n";
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
        QFile::remove( strNewFile );
    }

    m_changed = false;
    return true;
}

QArray<int> OPimTodoAccessXML::allRecords()const {
    QArray<int> ids( m_events.count() );
    QMap<int, OPimTodo>::ConstIterator it;
    int i = 0;

    for ( it = m_events.begin(); it != m_events.end(); ++it )
        ids[i++] = it.key();


    return ids;
}

OPimTodo OPimTodoAccessXML::find( int uid )const {
    OPimTodo todo;
    todo.setUid( 0 ); // isEmpty()
    QMap<int, OPimTodo>::ConstIterator it = m_events.find( uid );
    if ( it != m_events.end() )
        todo = it.data();

    return todo;
}

void OPimTodoAccessXML::clear() {
    if (m_opened )
        m_changed = true;

    m_events.clear();
}

bool OPimTodoAccessXML::add( const OPimTodo& todo ) {
    m_changed = true;
    m_events.insert( todo.uid(), todo );

    return true;
}

bool OPimTodoAccessXML::remove( int uid ) {
    m_changed = true;
    m_events.remove( uid );

    return true;
}

bool OPimTodoAccessXML::replace( const OPimTodo& todo) {
    m_changed = true;
    m_events.replace( todo.uid(), todo );

    return true;
}

QArray<int> OPimTodoAccessXML::effectiveToDos( const QDate& start,
                                            const QDate& end,
                                            bool includeNoDates )const {
    QArray<int> ids( m_events.count() );
    QMap<int, OPimTodo>::ConstIterator it;

    int i = 0;
    for ( it = m_events.begin(); it != m_events.end(); ++it ) {
        if ( !it.data().hasDueDate() && includeNoDates) {
                ids[i++] = it.key();
        }
        else if ( it.data().dueDate() >= start &&
                   it.data().dueDate() <= end ) {
            ids[i++] = it.key();
        }
    }
    ids.resize( i );
    return ids;
}

QArray<int> OPimTodoAccessXML::overDue()const {
    QArray<int> ids( m_events.count() );
    int i = 0;

    QMap<int, OPimTodo>::ConstIterator it;
    for ( it = m_events.begin(); it != m_events.end(); ++it ) {
        if ( it.data().isOverdue() ) {
            ids[i] = it.key();
            i++;
        }
    }
    ids.resize( i );
    return ids;
}

// from PalmtopRecord... GPL ### FIXME
namespace {
QString customToXml(const QMap<QString, QString>& customMap )
{
    QString buf(" ");
    for ( QMap<QString, QString>::ConstIterator cit = customMap.begin();
        cit != customMap.end(); ++cit) {
    buf += cit.key();
    buf += "=\"";
    buf += Qtopia::escapeString(cit.data());
    buf += "\" ";
    }
    return buf;
}


}

QString OPimTodoAccessXML::toString( const OPimTodo& ev, const QIntDict<QString> &revdict )const {
    QString str;

    QMap<int, QString> map = ev.toMap();

    for ( QMap<int, QString>::ConstIterator it = map.begin(); it != map.end(); ++it ) {
        const QString &value = it.data();
        int key = it.key();
        if ( !value.isEmpty() ) {
            str += " " + *revdict[ key ];
            str += "=\"" + Qtopia::escapeString( value ) + "\"";
        }
    }
    if( str[0] == ' ' )
        str = str.mid(1);

    str += customToXml( ev.toExtraMap() );

    return str;
}

QString OPimTodoAccessXML::toString( const QArray<int>& ints ) const {
    return Qtopia::Record::idsToString( ints );
}


QArray<int> OPimTodoAccessXML::sorted( const UIDArray& events, bool asc,
                                       int sortOrder,int sortFilter,
                                       const QArray<int>& categories )const {
    Internal::OPimTodoSortVector vector(events.count(), asc,sortOrder );
    int item = 0;

    bool bCat = sortFilter  & OPimTodoAccess::FilterCategory ? true : false;
    bool bOnly = sortFilter & OPimTodoAccess::OnlyOverDue ? true : false;
    bool comp = sortFilter  & OPimTodoAccess::DoNotShowCompleted ? true : false;
    bool catPassed = false;
    int cat;

    for ( uint i = 0; i < events.count(); ++i ) {
        /* Guard against creating a new item... */
        if ( !m_events.contains( events[i] ) )
            continue;

        OPimTodo todo = m_events[events[i]];

        /* show category */
        /* -1 == unfiled */
        catPassed = false;
        for ( uint cat_nu = 0; cat_nu < categories.count(); ++cat_nu ) {
            cat = categories[cat_nu];
            if ( bCat && cat == -1 ) {
                if(!todo.categories().isEmpty() )
                    continue;
            } 
            else if ( bCat && cat != 0)
                if (!todo.categories().contains( cat ) )
                    continue;
            catPassed = true;
            break;
        }

        /*
         * If none of the Categories matched
         * continue
         */
        if ( !catPassed )
            continue;
        if ( !todo.isOverdue() && bOnly )
            continue;
        if (todo.isCompleted() && comp )
            continue;

        vector.insert(item++, todo );
    }

    vector.resize( item );
    /* sort it now */
    vector.sort();
    /* now get the uids */
    UIDArray array( vector.count() );
    for (uint i= 0; i < vector.count(); i++ )
        array[i] = vector.uidAt( i );

    return array;
}

void OPimTodoAccessXML::removeAllCompleted() {
    QMap<int, OPimTodo> events = m_events;
    for ( QMap<int, OPimTodo>::Iterator it = m_events.begin(); it != m_events.end(); ++it ) {
        if ( (*it).isCompleted() )
            events.remove( it.key() );
    }
    m_events = events;
}

QArray<int> OPimTodoAccessXML::matchRegexp(  const QRegExp &r ) const
{
    QArray<int> currentQuery( m_events.count() );
    uint arraycounter = 0;

    QMap<int, OPimTodo>::ConstIterator it;
    for (it = m_events.begin(); it != m_events.end(); ++it ) {
        if ( it.data().match( r ) )
            currentQuery[arraycounter++] = it.data().uid();

    }
    // Shrink to fit..
    currentQuery.resize(arraycounter);

    return currentQuery;
}

}
