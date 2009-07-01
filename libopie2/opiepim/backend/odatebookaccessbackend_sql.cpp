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
/*
 * SQL Backend for the OPIE-Calender Database.
 *
 */

/* OPIE */
#include <opie2/osqldriver.h>
#include <opie2/osqlmanager.h>
#include <opie2/osqlquery.h>

#include <opie2/opimrecurrence.h>
#include <opie2/odatebookaccessbackend_sql.h>
#include <opie2/odebug.h>

#include <qpe/global.h>

/* QT */
#include <qarray.h>
#include <qstringlist.h>

/* STD */
#include <stdio.h>
#include <stdlib.h>


using namespace Opie::DB;

namespace {
    /**
     * a find query for custom elements
     */
    class FindCustomQuery : public OSQLQuery {
    public:
        FindCustomQuery(int uid);
        FindCustomQuery(const QArray<int>& );
        ~FindCustomQuery();
        QString query()const;
    private:
        QString single()const;
        QString multi()const;
        QArray<int> m_uids;
        int m_uid;
    };

    FindCustomQuery::FindCustomQuery(int uid)
        : OSQLQuery(), m_uid( uid ) {
    }
    FindCustomQuery::FindCustomQuery(const QArray<int>& ints)
        : OSQLQuery(), m_uids( ints ){
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
}


namespace Opie {


ODateBookAccessBackend_SQL::ODateBookAccessBackend_SQL( const QString& ,
                                                        const QString& fileName )
    : ODateBookAccessBackend(), m_driver( NULL )
{
    m_fileName = fileName.isEmpty() ? Global::applicationFileName( "datebook", "datebook.db" ) : fileName;

    // Get the standart sql-driver from the OSQLManager..
    OSQLManager man;
    m_driver = man.standard();
    m_driver->setUrl( m_fileName );

    initFields();

    load();
}

ODateBookAccessBackend_SQL::~ODateBookAccessBackend_SQL() {
    if( m_driver )
        delete m_driver;
}

void ODateBookAccessBackend_SQL::initFields()
{

    // This map contains the translation of the fieldtype id's to
    // the names of the table columns
    m_fieldMap.insert( OPimEvent::FUid, "uid" );
    m_fieldMap.insert( OPimEvent::FCategories, "Categories" );
    m_fieldMap.insert( OPimEvent::FDescription, "Description" );
    m_fieldMap.insert( OPimEvent::FLocation, "Location" );
    m_fieldMap.insert( OPimEvent::FType, "Type" );
    m_fieldMap.insert( OPimEvent::FAlarm, "Alarm" );
    m_fieldMap.insert( OPimEvent::FSound, "Sound" );
    m_fieldMap.insert( OPimEvent::FRType, "RType" );
    m_fieldMap.insert( OPimEvent::FRWeekdays, "RWeekdays" );
    m_fieldMap.insert( OPimEvent::FRPosition, "RPosition" );
    m_fieldMap.insert( OPimEvent::FRFreq, "RFreq" );
    m_fieldMap.insert( OPimEvent::FRHasEndDate, "RHasEndDate" );
    m_fieldMap.insert( OPimEvent::FREndDate, "REndDate" );
    m_fieldMap.insert( OPimEvent::FRCreated, "RCreated" );
    m_fieldMap.insert( OPimEvent::FRExceptions, "RExceptions" );
    m_fieldMap.insert( OPimEvent::FStart, "Start" );
    m_fieldMap.insert( OPimEvent::FEnd, "End" );
    m_fieldMap.insert( OPimEvent::FNote, "Note" );
    m_fieldMap.insert( OPimEvent::FTimeZone, "TimeZone" );
    m_fieldMap.insert( OPimEvent::FRecParent, "RecParent" );
    m_fieldMap.insert( OPimEvent::FRecChildren, "Recchildren" );

    // Create a map that maps the column name to the id
    QMapConstIterator<int, QString> it;
    for ( it = m_fieldMap.begin(); it != m_fieldMap.end(); ++it ){
        m_reverseFieldMap.insert( it.data(), it.key() );
    }

}

bool ODateBookAccessBackend_SQL::load()
{
    if (!m_driver->open() )
        return false;

    // Don't expect that the database exists.
    // It is save here to create the table, even if it
    // do exist. ( Is that correct for all databases ?? )
    QString qu = "create table if not exists datebook( uid INTEGER PRIMARY KEY ";

    QMap<int, QString>::Iterator it;
    for ( it = ++m_fieldMap.begin(); it != m_fieldMap.end(); ++it ){
        qu += QString( ",%1 VARCHAR(10)" ).arg( it.data() );
    }
    qu += " );";

    qu += "create table if not exists custom_data( uid INTEGER, id INTEGER, type VARCHAR(10), priority INTEGER, value VARCHAR(10), PRIMARY KEY /* identifier */ (uid, id) );";

    OSQLRawQuery raw( qu );
    OSQLResult res = m_driver->query( &raw );
    if ( res.state() != OSQLResult::Success )
        return false;

    update();

    return true;
}

void ODateBookAccessBackend_SQL::update()
{

    QString qu = "select uid from datebook";
    OSQLRawQuery raw( qu );
    OSQLResult res = m_driver->query( &raw );
    if ( res.state() != OSQLResult::Success ){
        // m_uids.clear();
        return;
    }

    m_uids = extractUids( res );

}

bool ODateBookAccessBackend_SQL::reload()
{
    return load();
}

bool ODateBookAccessBackend_SQL::save()
{
    return m_driver->close();  // Shouldn't m_driver->sync be better than close ? (eilers)
}

QArray<int> ODateBookAccessBackend_SQL::allRecords()const
{
    return m_uids;
}

QArray<int> ODateBookAccessBackend_SQL::queryByExample(const int& unUsed, int settings,  const QDateTime& ) {
	qDebug( "Accessing ODateBookAccessBackend_SQL::queryByExample() which is not implemented!" );
    return QArray<int>();
}

void ODateBookAccessBackend_SQL::clear()
{
    QString qu = "drop table datebook;";
    qu += "drop table custom_data;";

    OSQLRawQuery raw( qu );
    OSQLResult res = m_driver->query( &raw );

    reload();
}


OPimEvent ODateBookAccessBackend_SQL::find( int uid ) const{
    odebug << "ODateBookAccessBackend_SQL::find( " << uid << " )" << oendl;

    QString qu = "select *";
    qu += "from datebook where uid = " + QString::number(uid);

    odebug << "Query: " << qu << "" << oendl;

    OSQLRawQuery raw( qu );
    OSQLResult res = m_driver->query( &raw );

    OSQLResultItem resItem = res.first();

    // Create Map for date event and insert UID
    QMap<int,QString> dateEventMap;
    dateEventMap.insert( OPimEvent::FUid, QString::number( uid ) );

    // Now insert the data out of the columns into the map.
    QMapConstIterator<int, QString> it;
    for ( it = ++m_fieldMap.begin(); it != m_fieldMap.end(); ++it ){
        dateEventMap.insert( m_reverseFieldMap[*it], resItem.data( *it ) );
    }

    // Last step: Put map into date event, add custom map and return it
    OPimEvent retDate( dateEventMap );
    retDate.setExtraMap( requestCustom( uid ) );

    odebug << "ODateBookAccessBackend_SQL::find( " << uid << " ) end" << oendl;
    return retDate;
}

// FIXME: Speed up update of uid's..
bool ODateBookAccessBackend_SQL::add( const OPimEvent& ev )
{
    QMap<int,QString> eventMap = ev.toMap();

    QString qu = "insert into datebook VALUES( " + QString::number( ev.uid() );
    QMap<int, QString>::Iterator it;
    for ( it = ++m_fieldMap.begin(); it != m_fieldMap.end(); ++it ){
        if ( !eventMap[it.key()].isEmpty() )
            qu += QString( ",\"%1\"" ).arg( eventMap[it.key()] );
        else
            qu += QString( ",\"\"" );
    }
    qu += " );";

    // Add custom entries
    int id = 0;
    QMap<QString, QString> customMap = ev.toExtraMap();
    for( QMap<QString, QString>::Iterator it = customMap.begin();
         it != customMap.end(); ++it ){
        qu  += "insert into custom_data VALUES("
            +  QString::number( ev.uid() )
            + ","
            +  QString::number( id++ )
            + ",'"
            + it.key() //.latin1()
            + "',"
            + "0" // Priority for future enhancements
            + ",'"
            + it.data() //.latin1()
            + "');";
    }

    OSQLRawQuery raw( qu );
    OSQLResult res = m_driver->query( &raw );
    if ( res.state() != OSQLResult::Success ){
        return false;
    }

    // Update list of uid's
    update();

    return true;
}

// FIXME: Speed up update of uid's..
bool ODateBookAccessBackend_SQL::remove( int uid )
{
    QString qu = "DELETE from datebook where uid = "
        + QString::number( uid ) + ";";
    qu += "DELETE from custom_data where uid = "
        + QString::number( uid ) + ";";

    OSQLRawQuery raw( qu );
    OSQLResult res = m_driver->query( &raw );
    if ( res.state() != OSQLResult::Success ){
        return false;
    }

    // Update list of uid's
    update();

    return true;
}

bool ODateBookAccessBackend_SQL::replace( const OPimEvent& ev )
{
    remove( ev.uid() );
    return add( ev );
}


QArray<int> ODateBookAccessBackend_SQL::rawRepeats()const
{
    QString qu = "select uid from datebook where RType!=\"\" AND RType!=\"NoRepeat\"";
    OSQLRawQuery raw( qu );
    OSQLResult res = m_driver->query( &raw );
    if ( res.state() != OSQLResult::Success ){
        QArray<int> nix;
        return nix;
    }

    return extractUids( res );
}

QArray<int> ODateBookAccessBackend_SQL::nonRepeats()const
{
    QString qu = "select uid from datebook where RType=\"\" or RType=\"NoRepeat\"";
    OSQLRawQuery raw( qu );
    OSQLResult res = m_driver->query( &raw );
    if ( res.state() != OSQLResult::Success ){
        QArray<int> nix;
        return nix;
    }

    return extractUids( res );
}

OPimEvent::ValueList ODateBookAccessBackend_SQL::directNonRepeats()const
{
    QArray<int> nonRepUids = nonRepeats();
    OPimEvent::ValueList list;

    for (uint i = 0; i < nonRepUids.count(); ++i ){
        list.append( find( nonRepUids[i] ) );
    }

    return list;

}
OPimEvent::ValueList ODateBookAccessBackend_SQL::directRawRepeats()const
{
    QArray<int> rawRepUids = rawRepeats();
    OPimEvent::ValueList list;

    for (uint i = 0; i < rawRepUids.count(); ++i ){
        list.append( find( rawRepUids[i] ) );
    }

    return list;
}


QArray<int> ODateBookAccessBackend_SQL::matchRegexp(  const QRegExp &r ) const
{

    QString qu = "SELECT uid FROM datebook WHERE (";

    // Do it make sense to search other fields, too ?
    qu += " rlike(\""+ r.pattern() + "\", Location ) OR";
    qu += " rlike(\""+ r.pattern() + "\", Note )";

    qu += " )";

    odebug << "query: " << qu << "" << oendl;

    OSQLRawQuery raw( qu );
    OSQLResult res = m_driver->query( &raw );

    return extractUids( res );



}

/* ===== Private Functions ========================================== */

QArray<int> ODateBookAccessBackend_SQL::extractUids( OSQLResult& res ) const
{
    QTime t;
    t.start();
    OSQLResultItem::ValueList list = res.results();
    OSQLResultItem::ValueList::Iterator it;
    QArray<int> ints(list.count() );

    int i = 0;
    for (it = list.begin(); it != list.end(); ++it ) {
        ints[i] =  (*it).data("uid").toInt();
        i++;
    }

    return ints;

}

QMap<QString, QString> ODateBookAccessBackend_SQL::requestCustom( int uid ) const
{
    QTime t;
    t.start();

    QMap<QString, QString> customMap;

    FindCustomQuery query( uid );
    OSQLResult res_custom = m_driver->query( &query );

    if ( res_custom.state() == OSQLResult::Failure ) {
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
