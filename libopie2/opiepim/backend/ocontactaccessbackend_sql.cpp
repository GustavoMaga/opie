/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers <eilers.stefan@epost.de>
              =.             Copyright (C) The Opie Team <opie-devel@lists.sourceforge.net>
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
/*
 * SQL Backend for the OPIE-Contact Database.
 */

#include "ocontactaccessbackend_sql.h"

/* OPIE */
#include <opie2/opimcontact.h>
#include <opie2/opimcontactfields.h>
#include <opie2/opimdateconversion.h>
#include <opie2/osqldriver.h>
#include <opie2/osqlresult.h>
#include <opie2/osqlmanager.h>
#include <opie2/osqlquery.h>
#include <opie2/opimsql.h>
#include <opie2/odebug.h>

#include <qpe/global.h>
#include <qpe/recordfields.h>

/* QT */
#include <qarray.h>
#include <qdatetime.h>
#include <qstringlist.h>
#include <qfile.h>


using namespace Opie;
using namespace Opie::DB;


/*
 * Implementation of used query types * CREATE query
 * LOAD query
 * INSERT
 * REMOVE
 * CLEAR
 */
namespace {
    /**
     * CreateQuery for the Todolist Table
     */
    class CreateQuery : public OSQLQuery {
    public:
        CreateQuery();
        ~CreateQuery();
        QString query()const;
    };

    /**
     * Clears (delete) a Table
     */
    class ClearQuery : public OSQLQuery {
    public:
        ClearQuery();
        ~ClearQuery();
        QString query()const;

    };


    /**
     * inserts/adds a OPimContact to the table
     */
    class InsertQuery : public OSQLQuery {
    public:
        InsertQuery(const OPimContact& );
        ~InsertQuery();
        QString query()const;
    private:
        OPimContact m_contact;
    };


    /**
     * removes one from the table
     */
    class RemoveQuery : public OSQLQuery {
    public:
        RemoveQuery(int uid );
        ~RemoveQuery();
        QString query()const;
    private:
        int m_uid;
    };

    /**
     * a find query for custom elements
     */
    class FindCustomQuery : public OSQLQuery {
    public:
        FindCustomQuery(int uid);
        FindCustomQuery(const UIDArray& );
        ~FindCustomQuery();
        QString query()const;
    private:
        QString single()const;
        QString multi()const;
        UIDArray m_uids;
        int m_uid;
    };



    // We using two tables to store the information:
    // 1. addressbook  : It contains General information about the contact (non custom)
    // 2. custom_data  : Not official supported entries
    // All tables are connected by the uid of the contact.
    // Maybe I should add a table for meta-information ?
    CreateQuery::CreateQuery() : OSQLQuery() {}
    CreateQuery::~CreateQuery() {}
    QString CreateQuery::query()const {
        QString qu;

        qu += "create table if not exists addressbook( uid PRIMARY KEY ";

        QStringList fieldList = OPimContactFields::untrfields( false );
        for ( QStringList::Iterator it = ++fieldList.begin(); it != fieldList.end(); ++it ) {
            qu += QString( ",\"%1\" VARCHAR(10)" ).arg( *it );
        }
        qu += " );";

        qu += "create table if not exists custom_data( uid INTEGER, id INTEGER, type VARCHAR(10), priority INTEGER, value VARCHAR(10), PRIMARY KEY /* identifier */ (uid, id) );";

        return qu;
    }

    ClearQuery::ClearQuery()
        : OSQLQuery() {}
    ClearQuery::~ClearQuery() {}
    QString ClearQuery::query()const {
        QString qu = "drop table addressbook;";
        qu += "drop table custom_data;";
//      qu += "drop table dates;";
        return qu;
    }


    InsertQuery::InsertQuery( const OPimContact& contact )
        : OSQLQuery(), m_contact( contact ) {
    }

    InsertQuery::~InsertQuery() {
    }

    /*
     * converts from a OPimContact to a query
     */
    QString InsertQuery::query()const{

        QString qu;
        qu  += "insert into addressbook VALUES( " +
            QString::number( m_contact.uid() );

        // Get all information out of the contact-class
        // Remember: The category is stored in contactMap, too !
        QMap<int, QString> contactMap = m_contact.toMap();

        QStringList fieldList = OPimContactFields::untrfields( false );
        QMap<QString, int> translate = OPimContactFields::untrFieldsToId();
        for ( QStringList::Iterator it = ++fieldList.begin(); it != fieldList.end(); ++it ) {
            // Convert Column-String to Id and get value for this id..
            // Hmmm.. Maybe not very cute solution..
            int id = translate[*it];
            switch ( id ) {
            case Qtopia::Birthday:
            case Qtopia::Anniversary:{
                QDate day;
                if ( id == Qtopia::Birthday ) {
                    day = m_contact.birthday();
                } else {
                    day = m_contact.anniversary();
                }
                // These entries should stored in a special format
                // year-month-day
                if ( day.isValid() ) {
                    qu += QString(",\"%1-%2-%3\"")
                        .arg( QString::number( day.year() ).rightJustify( 4, '0' ) )
                        .arg( QString::number( day.month() ).rightJustify( 2, '0' ) )
                        .arg( QString::number( day.day() ).rightJustify( 2, '0' ) );
                }
                else {
                    qu += ",\"\"";
                }
            }
                break;
            default:
                qu += QString( ",\"%1\"" ).arg( contactMap[id] );
            }
        }
        qu += " );";


        // Now add custom data..
        int id = 0;
        id = 0;
        QMap<QString, QString> customMap = m_contact.toExtraMap();
        for( QMap<QString, QString>::Iterator it = customMap.begin();
             it != customMap.end(); ++it ) {
            qu  += "insert into custom_data VALUES("
                +  QString::number( m_contact.uid() )
                + ","
                +  QString::number( id++ )
                + ",'"
                + it.key()
                + "',"
                + "0" // Priority for future enhancements
                + ",'"
                + it.data()
                + "');";
        }
        // qu  += "commit;";
        odebug << "add " << qu << "" << oendl;
        return qu;
    }


    RemoveQuery::RemoveQuery(int uid )
        : OSQLQuery(), m_uid( uid ) {}

    RemoveQuery::~RemoveQuery() {}

    QString RemoveQuery::query()const {
        QString qu = "DELETE from addressbook where uid = "
            + QString::number(m_uid) + ";";
        qu += "DELETE from custom_data where uid = "
            + QString::number(m_uid) + ";";
        return qu;
    }



    FindCustomQuery::FindCustomQuery(int uid)
        : OSQLQuery(), m_uid( uid ) {
    }
    FindCustomQuery::FindCustomQuery(const UIDArray& ints)
        : OSQLQuery(), m_uids( ints ), m_uid( 0 ) {
    }
    FindCustomQuery::~FindCustomQuery() {
    }
    QString FindCustomQuery::query()const{
//      if ( m_uids.count() == 0 )
            return single();
    }
    QString FindCustomQuery::single()const{
        QString qu = "select uid, type, value from custom_data where uid = ";
        qu += QString::number(m_uid);
        return qu;
    }

};


/* --------------------------------------------------------------------------- */

namespace Opie {

OPimContactAccessBackend_SQL::OPimContactAccessBackend_SQL ( const QString& /* appname */,
                               const QString& filename ):
    OPimContactAccessBackend(), m_changed(false), m_driver( NULL )
{
    odebug << "C'tor OPimContactAccessBackend_SQL starts" << oendl;
    QTime t;
    t.start();

    /* Expecting to access the default filename if nothing else is set */
    if ( filename.isEmpty() ) {
        m_fileName = Global::applicationFileName( "addressbook","addressbook.db" );
    } else
        m_fileName = filename;

    // Get the standart sql-driver from the OSQLManager..
    OSQLManager man;
    m_driver = man.standard();
    m_driver->setUrl( m_fileName );

    m_changeLog = new OPimChangeLog_SQL( m_driver, "changelog", "peers" );

    odebug << "C'tor OPimContactAccessBackend_SQL ends: " << t.elapsed() << " ms" << oendl;
}

OPimContactAccessBackend_SQL::~OPimContactAccessBackend_SQL ()
{
    if( m_changeLog )
        delete m_changeLog;
    if( m_driver )
        delete m_driver;
}

bool OPimContactAccessBackend_SQL::load ()
{
    if (!m_driver->open() )
        return false;

    // Don't expect that the database exists.
    // It is save here to create the table, even if it
    // do exist. ( Is that correct for all databases ?? )
    CreateQuery creat;
    OSQLResult res = m_driver->query( &creat );

    update();

    m_changeLog->init();

    return true;

}

bool OPimContactAccessBackend_SQL::reload()
{
    return load();
}

bool OPimContactAccessBackend_SQL::save()
{
    return m_driver->sync();
}

void OPimContactAccessBackend_SQL::clear()
{
    ClearQuery cle;
    OSQLResult res = m_driver->query( &cle );

    reload();
}

OPimChangeLog *OPimContactAccessBackend_SQL::changeLog() const
{
    return m_changeLog;
}

bool OPimContactAccessBackend_SQL::wasChangedExternally()
{
    return false;
}

bool OPimContactAccessBackend_SQL::dataSourceExists() const {
    return QFile::exists( m_fileName );
}

UIDArray OPimContactAccessBackend_SQL::allRecords() const
{
    // FIXME: Think about cute handling of changed tables..
    // Thus, we don't have to call update here...
    if ( m_changed )
        ((OPimContactAccessBackend_SQL*)this)->update();

    return m_uids;
}

bool OPimContactAccessBackend_SQL::add ( const OPimContact &newcontact )
{
    odebug << "add in contact SQL-Backend" << oendl;
    InsertQuery ins( newcontact );
    OSQLResult res = m_driver->query( &ins );

    if ( res.state() == OSQLResult::Failure )
        return false;

    // Add changelog entry
    int uid = newcontact.uid();
    m_changeLog->addAddEntry( uid );

    int c = m_uids.count();
    m_uids.resize( c+1 );
    m_uids[c] = uid;

    return true;
}

bool OPimContactAccessBackend_SQL::remove ( int uid )
{
    RemoveQuery rem( uid );
    OSQLResult res = m_driver->query(&rem );

    if ( res.state() == OSQLResult::Failure )
        return false;

    // Add changelog entry
    m_changeLog->addDeleteEntry( uid );

    m_changed = true;

    return true;
}

bool OPimContactAccessBackend_SQL::replace ( const OPimContact &contact )
{
    // Disable the changelog (since we don't want the delete and add entries)
    m_changeLog->setEnabled( false );

    // Delete the old record
    bool result = remove( contact.uid() );

    if( result ) {
        // Add the new version back in
        result = add( contact );
    }

    m_changeLog->setEnabled( true );
    if( result ) {
        // Add changelog entry
        m_changeLog->addUpdateEntry( contact.uid() );
    }

    return result;
}


OPimContact OPimContactAccessBackend_SQL::find ( int uid ) const
{
    odebug << "OPimContactAccessBackend_SQL::find(" << uid << ")" << oendl;
    QTime t;
    t.start();

    OPimContact retContact( requestNonCustom( uid ) );
    retContact.setExtraMap( requestCustom( uid ) );

    odebug << "OPimContactAccessBackend_SQL::find() needed: " << t.elapsed() << " ms" << oendl;
    return retContact;
}

OPimContact OPimContactAccessBackend_SQL::find( int uid, const UIDArray& queryUids, uint current, Frontend::CacheDirection direction ) const
{
    odebug << "OPimContactAccessBackend_SQL::find( ..multi.. )" << oendl;
    odebug << "searching for " << uid << "" << oendl;

    QTime t;
    t.start();

    uint numReadAhead = readAhead();
    QArray<int> searchList( numReadAhead );

    uint size =0;

    // Build an array with all elements which should be requested and cached
    // We will just request "numReadAhead" elements, starting from "current" position in
    // the list of many uids !
    switch( direction ) {
        /* forward */
    case Frontend::Forward:
        for ( uint i = current; i < queryUids.count() && size < numReadAhead; i++ ) {
            searchList[size] = queryUids[i];
            size++;
        }
        break;
        /* reverse */
    case Frontend::Reverse:
        for ( uint i = current; i != 0 && size <  numReadAhead; i-- ) {
            searchList[size] = queryUids[i];
            size++;
        }
        break;
    }

    //Shrink to real size..
    searchList.resize( size );

    OPimContact retContact( requestContactsAndCache( uid, searchList ) );

    odebug << "OPimContactAccessBackend_SQL::find( ..multi.. ) needed: " << t.elapsed() << " ms" << oendl;
    return retContact;
}


UIDArray OPimContactAccessBackend_SQL::queryByExample ( const UIDArray& uidlist, const OPimContact &query, int settings,
                                                        const QDateTime& qd ) const
{
    QString searchQuery = "";
    QString datediff_query = "";
    QString uid_query = "";

    // Just add uid's selection if we really try to search in a subset of all uids! Otherwise this would
    // just take time and memory!
    if ( uidlist.count() != m_uids.count() ) {
        uid_query += " (";

        for ( uint i = 0; i < uidlist.count(); i++ ) {
            uid_query += " uid = " + QString::number( uidlist[i] ) + " OR";
        }
        uid_query.remove( uid_query.length()-2, 2 ); // Hmmmm..
        uid_query += " ) AND ";
    }


    QDate startDate;

    if ( qd.isValid() )
        startDate = qd.date();
    else
        startDate = QDate::currentDate();


    QMap<int, QString> queryFields = query.toMap();
    QStringList fieldList = OPimContactFields::untrfields( false );
    QMap<QString, int> translate = OPimContactFields::untrFieldsToId();

    // Convert every filled field to a SQL-Query
//  bool isAnyFieldSelected = false;
    for ( QStringList::Iterator it = ++fieldList.begin(); it != fieldList.end(); ++it ) {

        int id = translate[*it];
        QString queryStr = queryFields[id];
        QDate* endDate = 0l;

        if ( !queryStr.isEmpty() ) {
            // If something is alredy stored in the query, add an "AND"
            // to the end of the string to prepare for the next ..
            if ( !searchQuery.isEmpty() )
                searchQuery += " AND";

//          isAnyFieldSelected = true;
            switch( id ) {
            case Qtopia::Birthday:
                endDate = new QDate( query.birthday() );
                // Fall through !
            case Qtopia::Anniversary:
                if ( endDate == 0l )
                    endDate = new QDate( query.anniversary() );

                if ( settings & OPimContactAccess::DateDiff ) {
                    // To handle datediffs correctly, we need to remove the year information from
                    // the birthday and anniversary.
                    // To do this efficiently, we will create a temporary table which contains the
                    // information we need and do the query on it.
                    // This table is just visible for this process and will be removed
                    // automatically after using.
                    datediff_query = "SELECT uid,substr(\"Birthday\", 6, 10) as \"BirthdayMD\", substr(\"Anniversary\", 6, 10) as \"AnniversaryMD\" FROM addressbook WHERE ( \"Birthday\" != '' OR \"Anniversary\" != '' ) AND ";
                    datediff_query += QString( " (\"%1MD\" <= '%2-%3\' AND \"%4MD\" >= '%5-%6')" )
                        .arg( *it )
                        //.arg( QString::number( endDate->year() ).rightJustify( 4, '0' ) )
                        .arg( QString::number( endDate->month() ).rightJustify( 2, '0' ) )
                        .arg( QString::number( endDate->day() ).rightJustify( 2, '0' ) )
                        .arg( *it )
                        //.arg( QString::number( startDate.year() ).rightJustify( 4, '0' ) )
                        .arg( QString::number( startDate.month() ).rightJustify( 2, '0' ) )
                        .arg( QString::number( startDate.day() ).rightJustify( 2, '0' ) ) ;
                }

                if ( settings & OPimContactAccess::DateYear ) {
                    searchQuery += QString( " (\"%1\" LIKE '%2-%')" )
                        .arg( *it )
                        .arg( QString::number( endDate->year() ).rightJustify( 4, '0' ) );
                }

                if ( settings & OPimContactAccess::DateMonth ) {
                    if ( settings & OPimContactAccess::DateYear )
                        searchQuery += " AND";

                    searchQuery += QString( " (\"%1\" LIKE '%-%2-%')" )
                        .arg( *it )
                        .arg( QString::number( endDate->month() ).rightJustify( 2, '0' ) );
                }

                if ( settings & OPimContactAccess::DateDay ) {
                    if ( ( settings & OPimContactAccess::DateYear )
                         || ( settings & OPimContactAccess::DateMonth ) )
                        searchQuery += " AND";

                    searchQuery += QString( " (\"%1\" LIKE '%-%-%2')" )
                        .arg( *it )
                        .arg( QString::number( endDate->day() ).rightJustify( 2, '0' ) );
                }

                break;
            default:
                // Switching between case sensitive and insensitive...
                // LIKE is not case sensitive, GLOB is case sensitive
                // Do exist a better solution to switch this ?
                if ( settings & OPimContactAccess::IgnoreCase )
                    searchQuery += " (\"" + *it + "\"" + " LIKE " + "'"
                        + queryStr.replace(QRegExp("\\*"),"%") + "'" + ")";
                else
                    searchQuery += " (\"" + *it + "\"" + " GLOB " + "'"
                        + queryStr + "'" + ")";

            }
        }

        delete endDate;

    // The following if line is a replacement for
    // if ( searchQuery.endsWith( "AND" ) )
    if ( searchQuery.findRev( "AND" ) == static_cast<int>( searchQuery.length() - 3 ) ) {
        odebug << "remove AND" << oendl;
        searchQuery.remove( searchQuery.length()-3, 3 ); // Hmmmm..
    }

    }

    // Now compose the complete query
    QString qu = "SELECT uid FROM addressbook WHERE " + uid_query;

    if ( !datediff_query.isEmpty() && !searchQuery.isEmpty() ) {
        // If we use DateDiff, we have to intersect two queries.
        qu = datediff_query + QString( " INTERSECT " ) + qu + searchQuery;
    }
    else if ( datediff_query.isEmpty() && !searchQuery.isEmpty() ) {
        qu += searchQuery;
    }
    else if ( !datediff_query.isEmpty() && searchQuery.isEmpty() ) {
        qu = datediff_query;
    }
    else if ( datediff_query.isEmpty() && searchQuery.isEmpty() ) {
        UIDArray empty;
        return empty;
    }

    odebug << "queryByExample query: " << qu << "" << oendl;

    // Execute query and return the received uid's
    OSQLRawQuery raw( qu );
    OSQLResult res = m_driver->query( &raw );
    if ( res.state() != OSQLResult::Success ) {
        UIDArray empty;
        return empty;
    }

    UIDArray list = extractUids( res );

    return list;
}

UIDArray OPimContactAccessBackend_SQL::matchRegexp( const QRegExp &r ) const
{
#if 0
    QArray<int> nix(0);
    return nix;

#else
    QString qu = "SELECT uid FROM addressbook WHERE (";
    QString searchlist;

    QStringList fieldList = OPimContactFields::untrfields( false );
    // QMap<QString, int> translate = OPimContactFields::untrFieldsToId();
    for ( QStringList::Iterator it = ++fieldList.begin(); it != fieldList.end(); ++it ) {
        if ( !searchlist.isEmpty() )
            searchlist += " OR ";
        searchlist += " rlike(\""+ r.pattern() + "\",\"" + *it + "\") ";
    }

    qu = qu + searchlist + ")";

    odebug << "query: " << qu << "" << oendl;

    OSQLRawQuery raw( qu );
    OSQLResult res = m_driver->query( &raw );

    return extractUids( res );


#endif
}

uint OPimContactAccessBackend_SQL::querySettings() const
{
    return OPimContactAccess::IgnoreCase
        | OPimContactAccess::WildCards
        | OPimContactAccess::DateDiff
        | OPimContactAccess::DateYear
        | OPimContactAccess::DateMonth
        | OPimContactAccess::DateDay
        ;
}

bool OPimContactAccessBackend_SQL::hasQuerySettings (uint querySettings) const
{
    /* OPimContactAccess::IgnoreCase, DateDiff, DateYear, DateMonth, DateDay
     * may be added with any of the other settings. IgnoreCase should never used alone.
     * Wildcards, RegExp, ExactMatch should never used at the same time...
     */

    // Step 1: Check whether the given settings are supported by this backend
    if ( ( querySettings & (
                OPimContactAccess::IgnoreCase
                | OPimContactAccess::WildCards
                | OPimContactAccess::DateDiff
                | OPimContactAccess::DateYear
                | OPimContactAccess::DateMonth
                | OPimContactAccess::DateDay
//              | OPimContactAccess::RegExp
//              | OPimContactAccess::ExactMatch
                   ) ) != querySettings )
        return false;

    // Step 2: Check whether the given combinations are ok..

    // IngoreCase alone is invalid
    if ( querySettings == OPimContactAccess::IgnoreCase )
        return false;

    // WildCards, RegExp and ExactMatch should never used at the same time
    switch ( querySettings & ~( OPimContactAccess::IgnoreCase
                    | OPimContactAccess::DateDiff
                    | OPimContactAccess::DateYear
                    | OPimContactAccess::DateMonth
                    | OPimContactAccess::DateDay
                    )
         ) {
    case OPimContactAccess::RegExp:
        return ( true );
    case OPimContactAccess::WildCards:
        return ( true );
    case OPimContactAccess::ExactMatch:
        return ( true );
    case 0: // one of the upper removed bits were set..
        return ( true );
    default:
        return ( false );
    }

}

UIDArray OPimContactAccessBackend_SQL::sorted( const UIDArray& ar, bool asc, int sortOrder,
                                  int filter, const QArray<int>& categories )const
{
    QTime t;
    t.start();

    QString query = "SELECT uid FROM addressbook";

    query += " WHERE (";
    for ( uint i = 0; i < ar.count(); i++ ) {
        query += " uid = " + QString::number( ar[i] ) + " OR";
    }
    query.remove( query.length()-2, 2 ); // Hmmmm..
    query += ")";


    if ( filter != OPimBase::FilterOff ) {
        if ( filter & OPimContactAccess::DoNotShowWithCategory ) {
            query += " AND ( \"Categories\" == '' )";
        }
        else if ( filter & OPimBase::FilterCategory ) {
            query += " AND (";
            for ( uint i = 0; i < categories.count(); i++ ) {
                query += "\"Categories\" LIKE";
                query += QString( " '%" ) + QString::number( categories[i] ) + "%' OR";
            }
            query.remove( query.length()-2, 2 ); // Hmmmm..
            query += ")";
        }

        if ( filter & OPimContactAccess::DoNotShowWithoutChildren ) {
            query += " AND ( \"Children\" != '' )";
        }

        if ( filter & OPimContactAccess::DoNotShowWithoutAnniversary ) {
            query += " AND ( \"Anniversary\" != '' )";
        }

        if ( filter & OPimContactAccess::DoNotShowWithoutBirthday ) {
            query += " AND ( \"Birthday\" != '' )";
        }

        if ( filter & OPimContactAccess::DoNotShowWithoutHomeAddress ) {
            // Expect that no Street means no Address, too! (eilers)
            query += " AND ( \"Home Street\" != '' )";
        }

        if ( filter & OPimContactAccess::DoNotShowWithoutBusinessAddress ) {
            // Expect that no Street means no Address, too! (eilers)
            query += " AND ( \"Business Street\" != '' )";
        }

    }

    query += " ORDER BY";

    switch ( sortOrder ) {
    case OPimContactAccess::SortSummary:
        query += " \"Notes\"";
        break;
    case OPimContactAccess::SortByCategory:
        query += " \"Categories\"";
        break;
    case OPimContactAccess::SortByDate:
        query += " \"\"";
        break;
    case OPimContactAccess::SortTitle:
        query += " \"Name Title\"";
        break;
    case OPimContactAccess::SortFirstName:
        query += " \"First Name\"";
        break;
    case OPimContactAccess::SortMiddleName:
        query += " \"Middle Name\"";
        break;
    case OPimContactAccess::SortLastName:
        query += " \"Last Name\"";
        break;
    case OPimContactAccess::SortFileAsName:
        query += " \"File As\"";
        break;
    case OPimContactAccess::SortSuffix:
        query += " \"Suffix\"";
        break;
    case OPimContactAccess::SortEmail:
        query += " \"Default Email\"";
        break;
    case OPimContactAccess::SortNickname:
        query += " \"Nickname\"";
        break;
    case OPimContactAccess::SortAnniversary:
        query += " \"Anniversary\"";
        break;
    case OPimContactAccess::SortBirthday:
        query += " \"Birthday\"";
        break;
    case OPimContactAccess::SortGender:
        query += " \"Gender\"";
        break;
    case OPimContactAccess::SortBirthdayWithoutYear:
        query += " substr(\"Birthday\", 6, 10)";
        break;
    case OPimContactAccess::SortAnniversaryWithoutYear:
        query += " substr(\"Anniversary\", 6, 10)";
        break;
    default:
       query += " \"Last Name\"";
    }

    if ( !asc )
        query += " DESC";


    odebug << "sorted query is: " << query << "" << oendl;

    OSQLRawQuery raw( query );
    OSQLResult res = m_driver->query( &raw );
    if ( res.state() != OSQLResult::Success ) {
        UIDArray empty;
        return empty;
    }

    UIDArray list = extractUids( res );

    odebug << "sorted needed " << t.elapsed() << " ms!" << oendl;
    return list;
}


void OPimContactAccessBackend_SQL::update()
{
    odebug << "Update starts" << oendl;
    QTime t;
    t.start();

    // Now load the database set and extract the uid's
    // which will be held locally

    OPimSQLLoadQuery lo( "addressbook", m_changeLog );
    OSQLResult res = m_driver->query(&lo);
    if ( res.state() != OSQLResult::Success )
        return;

    m_uids = extractUids( res );

    m_changed = false;

    odebug << "Update ends " << t.elapsed() << " ms" << oendl;
}

UIDArray OPimContactAccessBackend_SQL::extractUids( OSQLResult& res ) const
{
    odebug << "extractUids" << oendl;
    QTime t;
    t.start();
    OSQLResultItem::ValueList list = res.results();
    OSQLResultItem::ValueList::Iterator it;
    UIDArray ints(list.count() );
    odebug << " count = " << list.count() << "" << oendl;

    int i = 0;
    for (it = list.begin(); it != list.end(); ++it ) {
        ints[i] =  (*it).data("uid").toInt();
        i++;
    }
    odebug << "extractUids ready: count2 = " << i << " needs " << t.elapsed() << " ms" << oendl;

    return ints;

}

QMap<int, QString>  OPimContactAccessBackend_SQL::requestNonCustom( int uid ) const
{
    QTime t;
    t.start();

    int t2needed = 0;
    int t3needed = 0;
    QTime t2;
    t2.start();
    OPimSQLFindQuery query( "addressbook", m_changeLog, uid );
    OSQLResult res_noncustom = m_driver->query( &query );
    t2needed = t2.elapsed();

    OSQLResultItem resItem = res_noncustom.first();

    QMap<int, QString> nonCustomMap;
    QTime t3;
    t3.start();
    nonCustomMap = fillNonCustomMap( resItem );
    t3needed = t3.elapsed();

    // odebug << "Adding UID: " << resItem.data( "uid" ) << "" << oendl;
    odebug << "RequestNonCustom needed: insg.:" << t.elapsed() << " ms, query: " << t2needed
           << " ms, mapping: " << t3needed << " ms" << oendl;

    return nonCustomMap;
}

/* Returns contact requested by uid and fills cache with contacts requested by uids in the cachelist */
OPimContact OPimContactAccessBackend_SQL::requestContactsAndCache( int uid, const UIDArray& uidlist )const
{
    // We want to get all contacts with one query.
    // We don't have to add the given uid to the uidlist, it is expected to be there already (see opimrecordlist.h).
    // All contacts will be stored in the cache, afterwards the contact with the user id "uid" will be returned
    // by using the cache..
    UIDArray cachelist = uidlist;
    OPimContact retContact;

    odebug << "Reqest and cache" << cachelist.size() << "elements !" << oendl;

    QTime t;
    t.start();

    int t2needed = 0;
    int t3needed = 0;
    QTime t2;
    t2.start();
    OPimSQLFindQuery query( "addressbook", m_changeLog, cachelist );
    OSQLResult res_noncustom = m_driver->query( &query );
    t2needed = t2.elapsed();

    QMap<int, QString> nonCustomMap;
    QTime t3;
    t3.start();
    OSQLResultItem resItem = res_noncustom.first();
    do {
        OPimContact contact( fillNonCustomMap( resItem ) );
        contact.setExtraMap( requestCustom( contact.uid() ) );
        odebug << "Caching uid: " << contact.uid() << oendl;
        cache( contact );
        if ( contact.uid() == uid )
            retContact = contact;
        resItem = res_noncustom.next();
    } while ( ! res_noncustom.atEnd() ); //atEnd() is true if we are past(!) the list !!
    t3needed = t3.elapsed();


    // odebug << "Adding UID: " << resItem.data( "uid" ) << "" << oendl;
    odebug << "RequestContactsAndCache needed: insg.:" << t.elapsed() << " ms, query: " << t2needed
           << " ms, mapping: " << t3needed << " ms" << oendl;

    return retContact;
}

QMap<int, QString> OPimContactAccessBackend_SQL::fillNonCustomMap( const OSQLResultItem& resultItem ) const
{
    QMap<int, QString> nonCustomMap;

    // Now loop through all columns
    QStringList fieldList = OPimContactFields::untrfields( false );
    QMap<QString, int> translate = OPimContactFields::untrFieldsToId();
    for ( QStringList::Iterator it = ++fieldList.begin(); it != fieldList.end(); ++it ) {
        // Get data for the selected column and store it with the
        // corresponding id into the map..

        int id =  translate[*it];
        QString value = resultItem.data( (*it) );

        // odebug << "Reading " << (*it) << "... found: " << value << "" << oendl;

        switch( id ) {
        case Qtopia::Birthday:
        case Qtopia::Anniversary:{
            // Birthday and Anniversary are encoded special ( yyyy-mm-dd )
            QStringList list = QStringList::split( '-', value );
            QStringList::Iterator lit = list.begin();
            int year  = (*lit).toInt();
            int month = (*(++lit)).toInt();
            int day   = (*(++lit)).toInt();
            if ( ( day != 0 ) && ( month != 0 ) && ( year != 0 ) ) {
                 QDate date( year, month, day );
                 nonCustomMap.insert( id, OPimDateConversion::dateToString( date ) );
            }
        }
            break;
        case Qtopia::AddressCategory:
            odebug << "Category is: " << value << "" << oendl;
        default:
            nonCustomMap.insert( id, value );
        }
    }

    nonCustomMap.insert( Qtopia::AddressUid, resultItem.data( "uid" ) );

    if( !m_changeLog->slowSync() )
        nonCustomMap.insert( FIELDID_ACTION, resultItem.data( "chgtype" ) );
    
    return nonCustomMap;
}


QMap<QString, QString>  OPimContactAccessBackend_SQL::requestCustom( int uid ) const
{
    QTime t;
    t.start();

    QMap<QString, QString> customMap;

    FindCustomQuery query( uid );
    OSQLResult res_custom = m_driver->query( &query );

    if ( res_custom.state() == OSQLResult::Failure ) {
        owarn << "OSQLResult::Failure in find query !!" << oendl;
        QMap<QString, QString> empty;
        return empty;
    }

    OSQLResultItem::ValueList list = res_custom.results();
    OSQLResultItem::ValueList::Iterator it = list.begin();
    for ( ; it != list.end(); ++it ) {
        customMap.insert( (*it).data( "type" ), (*it).data( "value" ) );
    }

    odebug << "RequestCustom needed: " << t.elapsed() << " ms" << oendl;
    return customMap;
}

}
