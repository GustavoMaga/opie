/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers (Eilers.Stefan@epost.de)
                             Copyright (C) Holger Freyther (zecke@handhelds.org)
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

/* OPIE */
#include <opie2/osqldriver.h>
#include <opie2/osqlresult.h>
#include <opie2/osqlmanager.h>
#include <opie2/osqlquery.h>
#include <opie2/opimsql.h>

#include <opie2/otodoaccess.h>
#include <opie2/otodoaccesssql.h>
#include <opie2/opimstate.h>
#include <opie2/opimnotifymanager.h>
#include <opie2/opimrecurrence.h>
#include <opie2/odebug.h>

#include <qpe/global.h>

/* QT */
#include <qdatetime.h>
#include <qmap.h>
#include <qstring.h>
#include <qfile.h>


using namespace Opie::DB;

using namespace Opie;
/*
 * first some query
 * CREATE query
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
     * inserts/adds a OPimTodo to the table
     */
    class InsertQuery : public OSQLQuery {
    public:
        InsertQuery(const OPimTodo& );
        ~InsertQuery();
        QString query()const;
    private:
        OPimTodo m_todo;
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
     * Clears (delete) a Table
     */
    class ClearQuery : public OSQLQuery {
    public:
        ClearQuery();
        ~ClearQuery();
        QString query()const;

    };

    /**
     * overdue query
     */
    class OverDueQuery : public OSQLQuery {
    public:
        OverDueQuery();
        ~OverDueQuery();
        QString query()const;
    };
    class EffQuery : public OSQLQuery {
    public:
        EffQuery( const QDate&, const QDate&, bool inc );
        ~EffQuery();
        QString query()const;
    private:
        QString with()const;
        QString out()const;
        QDate m_start;
        QDate m_end;
        bool m_inc :1;
    };

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



    CreateQuery::CreateQuery() : OSQLQuery() {}
    CreateQuery::~CreateQuery() {}
    QString CreateQuery::query()const {
        QString qu;
        qu += "create table if not exists todolist( uid PRIMARY KEY, categories, completed, ";
        qu += "description, summary, priority, DueDate, progress ,  state, ";
    // This is the recurrance-stuff .. Exceptions are currently not supported (see OPimRecurrence.cpp) ! (eilers)
    qu += "RType, RWeekdays, RPosition, RFreq, RHasEndDate, EndDate, Created, Exceptions, ";
    qu += "reminders, alarms, maintainer, startdate, completeddate);";
    qu += "create table if not exists custom_data( uid INTEGER, id INTEGER, type VARCHAR(10), priority INTEGER, value VARCHAR(10), PRIMARY KEY /* identifier */ (uid, id) );";
        return qu;
    }


    InsertQuery::InsertQuery( const OPimTodo& todo )
        : OSQLQuery(), m_todo( todo ) {
    }
    InsertQuery::~InsertQuery() {
    }
    /*
     * converts from a OPimTodo to a query
     * we leave out X-Ref + Maintainer
     * FIXME: Implement/Finish toMap()/fromMap() into OpimTodo to move the encoding
     *        decoding stuff there.. (eilers)
     */
    QString InsertQuery::query()const{

        int year, month, day;
        year = month = day = 0;
        if (m_todo.hasDueDate() ) {
            QDate date = m_todo.dueDate();
            year = date.year();
            month = date.month();
            day = date.day();
    }
    int sYear = 0, sMonth = 0, sDay = 0;
    if( m_todo.hasStartDate() ){
        QDate sDate = m_todo.startDate();
        sYear = sDate.year();
        sMonth= sDate.month();
        sDay  = sDate.day();
    }

    int eYear = 0, eMonth = 0, eDay = 0;
    if( m_todo.hasCompletedDate() ){
        QDate eDate = m_todo.completedDate();
        eYear = eDate.year();
        eMonth= eDate.month();
        eDay  = eDate.day();
    }
        QString qu;
    QMap<int, QString> recMap = m_todo.recurrence().toMap();
        qu  = "insert into todolist VALUES("
        +  QString::number( m_todo.uid() ) + ","
        + "'" + m_todo.idsToString( m_todo.categories() ) + "'" + ","
        +       QString::number( m_todo.isCompleted() )         + ","
        + "'" + m_todo.description()                      + "'" + ","
        + "'" + m_todo.summary()                          + "'" + ","
        +       QString::number(m_todo.priority() )             + ","
        + "'" + QString::number(year).rightJustify( 4, '0' ) + "-"
        +       QString::number(month).rightJustify( 2, '0' )
        +       "-" + QString::number( day ).rightJustify( 2, '0' )+ "'" + ","
        +       QString::number( m_todo.progress() )            + ","
        +       QString::number( m_todo.state().state() )       + ","
        + "'" + recMap[ OPimRecurrence::RType ]                   + "'" + ","
        + "'" + recMap[ OPimRecurrence::RWeekdays ]               + "'" + ","
        + "'" + recMap[ OPimRecurrence::RPosition ]               + "'" + ","
        + "'" + recMap[ OPimRecurrence::RFreq ]                   + "'" + ","
        + "'" + recMap[ OPimRecurrence::RHasEndDate ]             + "'" + ","
        + "'" + recMap[ OPimRecurrence::EndDate ]                 + "'" + ","
        + "'" + recMap[ OPimRecurrence::Created ]                 + "'" + ","
        + "'" + recMap[ OPimRecurrence::Exceptions ]              + "'" + ",";

    if ( m_todo.hasNotifiers() ) {
        OPimNotifyManager manager = m_todo.notifiers();
        qu += "'" + manager.remindersToString()           + "'" + ","
            + "'" + manager.alarmsToString()          + "'" + ",";
    }
    else{
        qu += QString( "''" )                                   + ","
            + "''"                                          + ",";
    }

    qu +=   QString( "''" )                             + QString( "," ) // Maintainers (cur. not supported !)
        + "'" + QString::number(sYear).rightJustify( 4, '0' ) + "-"
        + QString::number(sMonth).rightJustify( 2, '0' )
        + "-" + QString::number(sDay).rightJustify( 2, '0' )+ "'" + ","
        + "'" + QString::number(eYear).rightJustify( 4, '0' ) + "-"
        +       QString::number(eMonth).rightJustify( 2, '0' )
        +       "-"+QString::number(eDay).rightJustify( 2, '0' ) + "'"
        + "); ";

    // Save custom Entries:
    int id = 0;
    id = 0;
    QMap<QString, QString> customMap = m_todo.toExtraMap();
    for( QMap<QString, QString>::Iterator it = customMap.begin();
         it != customMap.end(); ++it ){
        qu  += "insert into custom_data VALUES("
            +  QString::number( m_todo.uid() )
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


        odebug << "add " << qu << "" << oendl;
        return qu;
    }

    RemoveQuery::RemoveQuery(int uid )
        : OSQLQuery(), m_uid( uid ) {}

    RemoveQuery::~RemoveQuery() {}

    QString RemoveQuery::query()const {
        QString qu = "DELETE FROM todolist WHERE uid = " + QString::number(m_uid) + " ;";
        qu += "DELETE FROM custom_data WHERE uid = " + QString::number(m_uid);
        return qu;
    }


    ClearQuery::ClearQuery()
        : OSQLQuery() {}
    ClearQuery::~ClearQuery() {}
    QString ClearQuery::query()const
    {
        QString qu = "drop table todolist";
        return qu;
    }


    OverDueQuery::OverDueQuery(): OSQLQuery() {}
    OverDueQuery::~OverDueQuery() {}
    QString OverDueQuery::query()const {
        QDate date = QDate::currentDate();
        QString str;
        str = QString("select uid from todolist where DueDate ='%1-%2-%3'")
        .arg( QString::number( date.year() ).rightJustify( 4, '0' ) )
        .arg( QString::number( date.month() ).rightJustify( 2, '0' ) )
        .arg( QString::number( date.day() ) .rightJustify( 2, '0' ) );

        return str;
    }


    EffQuery::EffQuery( const QDate& start, const QDate& end, bool inc )
        : OSQLQuery(), m_start( start ), m_end( end ),m_inc(inc) {}
    EffQuery::~EffQuery() {}
    QString EffQuery::query()const {
        return m_inc ? with() : out();
    }
    QString EffQuery::with()const {
        QString str;
        str = QString("select uid from todolist where ( DueDate >= '%1-%2-%3' AND DueDate <= '%4-%5-%6' ) OR DueDate = '0-0-0' ")
        .arg( QString::number( m_start.year() ).rightJustify( 4, '0' ) )
        .arg( QString::number( m_start.month() ).rightJustify( 2, '0' ) )
        .arg( QString::number( m_start.day() ).rightJustify( 2, '0' ) )
        .arg( QString::number( m_end.year() ).rightJustify( 4, '0' ) )
        .arg( QString::number( m_end.month() ).rightJustify( 2, '0' ) )
        .arg( QString::number( m_end.day() ).rightJustify( 2, '0' ) );
        return str;
    }
    QString EffQuery::out()const {
        QString str;
        str = QString("select uid from todolist where DueDate >= '%1-%2-%3' AND  DueDate <= '%4-%5-%6'")
        .arg( QString::number( m_start.year() ).rightJustify( 4, '0' ) )
        .arg( QString::number( m_start.month() ).rightJustify( 2, '0' ) )
        .arg( QString::number( m_start.day() ).rightJustify( 2, '0' ) )
        .arg( QString::number( m_end.year() ).rightJustify( 4, '0' ) )
        .arg( QString::number( m_end.month() ).rightJustify( 2, '0' ) )
        .arg( QString::number( m_end.day() ).rightJustify( 2, '0' ) );

        return str;
    }

    FindCustomQuery::FindCustomQuery(int uid)
        : OSQLQuery(), m_uid( uid ) {
    }
    FindCustomQuery::FindCustomQuery(const QArray<int>& ints)
        : OSQLQuery(), m_uids( ints ), m_uid( 0 ) {
    }
    FindCustomQuery::~FindCustomQuery() {
    }
    QString FindCustomQuery::query()const{
        return single(); // Multiple requests not supported !
    }
    QString FindCustomQuery::single()const{
        QString qu = "select uid, type, value from custom_data where uid = ";
        qu += QString::number(m_uid);
        return qu;
    }

};


namespace Opie {
OPimTodoAccessBackendSQL::OPimTodoAccessBackendSQL( const QString& file )
    : OPimTodoAccessBackend(),/* m_dict(15),*/ m_driver(NULL), m_dirty(true)
{
    m_fileName = file.isEmpty() ? Global::applicationFileName( "todolist", "todolist.db" ) : file;

    OSQLManager man;
    m_driver = man.standard();
    m_driver->setUrl(m_fileName);
    // fillDict();
    m_changeLog = new OPimChangeLog_SQL( m_driver, "changelog", "peers" );
}

OPimTodoAccessBackendSQL::~OPimTodoAccessBackendSQL(){
    if( m_changeLog )
        delete m_changeLog;
    if( m_driver )
        delete m_driver;
}

bool OPimTodoAccessBackendSQL::load(){
    if (!m_driver->open() )
        return false;

    CreateQuery creat;
    OSQLResult res = m_driver->query(&creat );

    m_changeLog->init();

    m_dirty = true;
    return true;
}
bool OPimTodoAccessBackendSQL::reload(){
    return load();
}

bool OPimTodoAccessBackendSQL::save(){
    return m_driver->sync();
}

OPimChangeLog *OPimTodoAccessBackendSQL::changeLog() const
{
    return m_changeLog;
}

bool OPimTodoAccessBackendSQL::dataSourceExists() const {
    return QFile::exists( m_fileName );
}

QArray<int> OPimTodoAccessBackendSQL::allRecords()const {
    if (m_dirty )
        update();

    return m_uids;
}
// QArray<int> OPimTodoAccessBackendSQL::queryByExample( const UIDArray& uidlist, const OPimTodo& , int, const QDateTime& ){
//     QArray<int> ints(0);
//     return ints;
// }
OPimTodo OPimTodoAccessBackendSQL::find(int uid ) const{
    OPimSQLFindQuery query( "todolist", m_changeLog, uid );
    return parseResultAndCache( uid, m_driver->query(&query) );
}

// Remember: uid is already in the list of uids, called ints !
OPimTodo OPimTodoAccessBackendSQL::find( int uid, const QArray<int>& ints,
                                   uint cur, Frontend::CacheDirection dir ) const{
    uint CACHE = readAhead();
    odebug << "searching for " << uid << "" << oendl;
    QArray<int> search( CACHE );
    uint size =0;

    // we try to cache CACHE items
    switch( dir ) {
    /* forward */
    case Frontend::Forward:
        for (uint i = cur; i < ints.count() && size < CACHE; i++ ) {
            search[size] = ints[i];
            size++;
        }
        break;
    /* reverse */
    case Frontend::Reverse:
        for (uint i = cur; i != 0 && size <  CACHE; i-- ) {
            search[size] = ints[i];
            size++;
        }
        break;
    }

    search.resize( size );
    OPimSQLFindQuery query( "todolist", m_changeLog, search );
    OSQLResult res = m_driver->query( &query  );
    if ( res.state() != OSQLResult::Success )
        return OPimTodo();

    return parseResultAndCache( uid, res );
}

void OPimTodoAccessBackendSQL::clear() {
    ClearQuery cle;
    OSQLResult res = m_driver->query( &cle );
    CreateQuery qu;
    res = m_driver->query(&qu);
}
bool OPimTodoAccessBackendSQL::add( const OPimTodo& t) {
    InsertQuery ins( t );
    OSQLResult res = m_driver->query( &ins );

    if ( res.state() == OSQLResult::Failure )
        return false;

    // Add changelog entry
    int uid = t.uid();
    m_changeLog->addAddEntry( uid );

    int c = m_uids.count();
    m_uids.resize( c+1 );
    m_uids[c] = uid;

    return true;
}
bool OPimTodoAccessBackendSQL::remove( int uid ) {
    RemoveQuery rem( uid );
    OSQLResult res = m_driver->query(&rem );

    if ( res.state() == OSQLResult::Failure )
        return false;

    // Add changelog entry
    m_changeLog->addDeleteEntry( uid );

    m_dirty = true;
    return true;
}
/*
 * FIXME better set query
 * but we need the cache for that
 * now we remove
 */
bool OPimTodoAccessBackendSQL::replace( const OPimTodo& t) {
    // Disable the changelog (since we don't want the delete and add entries)
    m_changeLog->setEnabled( false );

    // Delete the old record
    bool result = remove( t.uid() );

    if( result ) {
        // Add the new version back in
        result = add( t );
    }

    m_changeLog->setEnabled( true );
    if( result ) {
        // Add changelog entry
        m_changeLog->addUpdateEntry( t.uid() );
        
        m_dirty = false; // we changed some stuff but the UID stayed the same
    }

    return result;
}
QArray<int> OPimTodoAccessBackendSQL::overDue()const {
    OverDueQuery qu;
    return uids( m_driver->query(&qu ) );
}
QArray<int> OPimTodoAccessBackendSQL::effectiveToDos( const QDate& s,
                                                   const QDate& t,
                                                   bool u)const {
    EffQuery ef(s, t, u );
    return uids (m_driver->query(&ef) );
}

/*
 *
 */
UIDArray OPimTodoAccessBackendSQL::sorted( bool asc, int sortOrder,
                                           int sortFilter, const QArray<int>& categories ) const{
    odebug << "sorted " << asc << ", " << sortOrder << "" << oendl;
    QString query;
    query = "";

    /*
     * Sort Filter stuff
     * not that straight forward
     * FIXME: Replace magic numbers
     *
     */
    /* Category */
    QString cat_query = "";
    for ( uint cat_nu = 0; cat_nu < categories.count(); ++cat_nu ) {
        int cat = categories[cat_nu];
        if( cat == -1 ) {
            cat_query += " categories = '' OR";
            break;
        }
        else if( cat != 0 ) {
            cat_query += " categories LIKE '%" + QString::number( cat ) + "%' OR";
            break;
        }
    }
    if( !cat_query.isEmpty() ) {
        cat_query.remove( cat_query.length()-2, 2 );
        query += "(" + cat_query + ") AND";
    }

    /* Show only overdue */
    if ( sortFilter & OPimTodoAccess::OnlyOverDue ) {
        QDate date = QDate::currentDate();
        QString due;
        QString base;
        base = QString("DueDate != '0000-00-00' AND DueDate < '%1-%2-%3' AND completed = 0")
        .arg( QString::number( date.year() ).rightJustify( 4, '0' ) )
        .arg( QString::number( date.month() ).rightJustify( 2, '0' ) )
        .arg( QString::number( date.day() ).rightJustify( 2, '0' ) );
        query += " " + base + " AND";
    }
    /* not show completed */
    if ( sortFilter & OPimTodoAccess::DoNotShowCompleted ) {
        query += " completed = 0 AND";
    }

    /* strip the end */
    if( !query.isEmpty() )
        query = "select uid from todolist WHERE " + query.remove( query.length()-3, 3 );
    else
        query = "select uid from todolist ";

    /*
     * sort order stuff
     * quite straight forward
     */
    query += "ORDER BY ";
    QString orderfields;
    switch( sortOrder ) {
        /* completed */
    case OPimTodoAccess::Completed:
        orderfields = "completed, priority, duedate";
        break;
    case OPimTodoAccess::Priority:
        orderfields = "priority, completed, duedate";
        break;
    case OPimTodoAccess::Deadline:
        orderfields = "duedate";
        break;
    case OPimTodoAccess::SortSummary:
    default:
        orderfields = "summary";
        break;
    }

    if ( asc )
        query += orderfields;
    else
        query += orderfields.replace(QRegExp(","), " DESC,") + " DESC";


    odebug << query << oendl;
    OSQLRawQuery raw(query );
    return uids( m_driver->query(&raw) );
}


bool OPimTodoAccessBackendSQL::date( QDate& da, const QString& str ) const{
    if ( str == "0000-00-00" )
        return false;
    else{
        int day, year, month;
        QStringList list = QStringList::split("-", str );
        year = list[0].toInt();
        month = list[1].toInt();
        day = list[2].toInt();
        da.setYMD( year, month, day );
        return true;
    }
}


OPimTodo OPimTodoAccessBackendSQL::parseResultAndCache( int uid, const OSQLResult& res ) const{
    if ( res.state() == OSQLResult::Failure ) {
        OPimTodo to;
        return to;
    }

    OPimTodo retTodo;

    OSQLResultItem::ValueList list = res.results();
    OSQLResultItem::ValueList::Iterator it = list.begin();
    OPimTodo to, tmp;

    for ( ; it != list.end(); ++it ) {
        OPimTodo newTodo = parse( (*it) );
        cache( newTodo );
	if ( newTodo.uid() == uid )
		retTodo = newTodo;
    }
    return retTodo;
}
OPimTodo OPimTodoAccessBackendSQL::parse( OSQLResultItem& item )const {

    // Request information from addressbook table and create the OPimTodo-object.

    bool hasDueDate = false; QDate dueDate = QDate::currentDate();
    hasDueDate = date( dueDate, item.data("DueDate") );
    QStringList cats = QStringList::split(";", item.data("categories") );

    OPimTodo to( (bool)item.data("completed").toInt(), item.data("priority").toInt(),
              cats, item.data("summary"), item.data("description"),
              item.data("progress").toUShort(), hasDueDate, dueDate,
              item.data("uid").toInt() );

    bool isOk;
    int prioInt = QString( item.data("priority") ).toInt( &isOk );
    if ( isOk )
        to.setPriority( prioInt );

    bool hasStartDate = false; QDate startDate = QDate::currentDate();
    hasStartDate = date( startDate, item.data("startdate") );
    bool hasCompletedDate = false; QDate completedDate = QDate::currentDate();
    hasCompletedDate = date( completedDate, item.data("completeddate") );

    if ( hasStartDate )
        to.setStartDate( startDate );
    if ( hasCompletedDate )
        to.setCompletedDate( completedDate );

    OPimNotifyManager& manager = to.notifiers();
    manager.alarmsFromString( item.data("alarms") );
    manager.remindersFromString( item.data("reminders") );

    OPimState pimState;
    pimState.setState( QString( item.data("state") ).toInt() );
    to.setState( pimState );

    QMap<int, QString> recMap;
    recMap.insert( OPimRecurrence::RType      , item.data("RType") );
    recMap.insert( OPimRecurrence::RWeekdays  , item.data("RWeekdays") );
    recMap.insert( OPimRecurrence::RPosition  , item.data("RPosition") );
    recMap.insert( OPimRecurrence::RFreq      , item.data("RFreq") );
    recMap.insert( OPimRecurrence::RHasEndDate, item.data("RHasEndDate") );
    recMap.insert( OPimRecurrence::EndDate    , item.data("EndDate") );
    recMap.insert( OPimRecurrence::Created    , item.data("Created") );
    recMap.insert( OPimRecurrence::Exceptions , item.data("Exceptions") );

    OPimRecurrence recur;
    recur.fromMap( recMap );
    to.setRecurrence( recur );

    if( !m_changeLog->slowSync() ) {
        QString chgtype = item.data("chgtype");
        if( chgtype == "D" )
            to.setAction( OPimRecord::ACTION_REMOVE );
        else if( chgtype == "U" )
            to.setAction( OPimRecord::ACTION_REPLACE );
    }
    
    // Finally load the custom-entries for this UID and put it into the created object
    to.setExtraMap( requestCustom( to.uid() ) );

    return to;
}

// FIXME: Where is the difference to "find" ? (eilers)
OPimTodo OPimTodoAccessBackendSQL::todo( int uid )const {
    OPimSQLFindQuery find( "todolist", m_changeLog, uid );
    return parseResultAndCache( uid, m_driver->query(&find) );
}


/*
 * need to be const so let's fool the
 * compiler :(
 */
void OPimTodoAccessBackendSQL::update()const {
    ((OPimTodoAccessBackendSQL*)this)->m_dirty = false;
    OPimSQLLoadQuery lo( "todolist", m_changeLog );
    OSQLResult res = m_driver->query(&lo);
    if ( res.state() != OSQLResult::Success )
        return;

    ((OPimTodoAccessBackendSQL*)this)->m_uids = uids( res );
}
QArray<int> OPimTodoAccessBackendSQL::uids( const OSQLResult& res) const{

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

QArray<int> OPimTodoAccessBackendSQL::matchRegexp(  const QRegExp &r ) const
{
    QString qu = "SELECT uid FROM todolist WHERE (";

    // Does it make sense to search other fields, too ?
    qu += " rlike(\""+ r.pattern() + "\",\"description\") OR";
    qu += " rlike(\""+ r.pattern() + "\",\"summary\")";

    qu += ")";

    OSQLRawQuery raw( qu );
    OSQLResult res = m_driver->query( &raw );

    return uids( res );
}

void OPimTodoAccessBackendSQL::removeAllCompleted(){
    // First we need the uids from all entries which are
    // completed. Then, we just have to remove them...

    QString qu = "SELECT uid FROM todolist WHERE completed = 1";

    OSQLRawQuery raw( qu );
    OSQLResult res = m_driver->query( &raw );

    QArray<int> completed_uids = uids( res );

    if ( completed_uids.size() == 0 )
        return;

    qu = "DELETE FROM todolist WHERE (";
    QString query;

    for ( uint i = 0; i < completed_uids.size(); i++ ){
        if ( !query.isEmpty() )
            query += " OR ";
        query += QString( "uid = %1" ).arg( completed_uids[i] );
    }
    qu += query + " );";

    // Put remove of custom entries in this query to speed up..
    qu += "DELETE FROM custom_data WHERE (";
    query = "";

    for ( uint i = 0; i < completed_uids.size(); i++ ){
        if ( !query.isEmpty() )
            query += " OR ";
        query += QString( "uid = %1" ).arg( completed_uids[i] );
    }
    qu += query + " );";

    OSQLRawQuery raw2( qu );
    res = m_driver->query( &raw2 );

    if ( res.state() == OSQLResult::Failure )
        owarn << "OPimTodoAccessBackendSQL::removeAllCompleted():Failure in query: " << qu << "" << oendl;

}


QMap<QString, QString>  OPimTodoAccessBackendSQL::requestCustom( int uid ) const
{
    QMap<QString, QString> customMap;

    FindCustomQuery query( uid );
    OSQLResult res_custom = m_driver->query( &query );

    if ( res_custom.state() == OSQLResult::Failure ) {
        owarn << "OSQLResult::Failure in find query !!" << oendl;
        return QMap<QString, QString>();
    }

    OSQLResultItem::ValueList list = res_custom.results();
    OSQLResultItem::ValueList::Iterator it = list.begin();
    for ( ; it != list.end(); ++it )
        customMap.insert( (*it).data( "type" ), (*it).data( "value" ) );


    return customMap;
}




}
