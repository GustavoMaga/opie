#include <qdatetime.h>

#include <qpe/alarmserver.h>

// #include "otodoaccesssql.h"
#include "otodoaccess.h"
#include "obackendfactory.h"

OTodoAccess::OTodoAccess( OTodoAccessBackend* end )
    : QObject(), OPimAccessTemplate<OTodo>( end ),  m_todoBackEnd( end )
{
//     if (end == 0l )
//         m_todoBackEnd = new OTodoAccessBackendSQL( QString::null);

	// Zecke: Du musst hier noch f�r das XML-Backend einen Appnamen �bergeben !
        if (end == 0l )
		m_todoBackEnd = OBackendFactory<OTodoAccessBackend>::Default ("todo", QString::null);

    setBackEnd( m_todoBackEnd );
}
OTodoAccess::~OTodoAccess() {
//    qWarning("~OTodoAccess");
}
void OTodoAccess::mergeWith( const QValueList<OTodo>& list ) {
    QValueList<OTodo>::ConstIterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
        replace( (*it) );
    }
}
OTodoAccess::List OTodoAccess::effectiveToDos( const QDate& start,
                                               const QDate& end,
                                               bool includeNoDates ) {
    QArray<int> ints = m_todoBackEnd->effectiveToDos( start, end, includeNoDates );

    List lis( ints, this );
    return lis;
}
OTodoAccess::List OTodoAccess::effectiveToDos( const QDate& start,
                                               bool includeNoDates ) {
    return effectiveToDos( start, QDate::currentDate(),
                           includeNoDates );
}
OTodoAccess::List OTodoAccess::overDue() {
    List lis( m_todoBackEnd->overDue(), this );
    return lis;
}
void OTodoAccess::addAlarm( const OTodo& event) {
    if (!event.hasAlarmDateTime() )
        return;

    QDateTime now      = QDateTime::currentDateTime();
    QDateTime schedule = event.alarmDateTime();

    if ( schedule > now ){
        AlarmServer::addAlarm( schedule,
                               "QPE/Application/todolist",
                               "alarm(QDateTime,int)", event.uid() );

    }
}
void OTodoAccess::delAlarm( int uid) {

    QDateTime schedule; // Create null DateTime

    // I hope this will remove all scheduled alarms
    // with the given uid !?
    // If not: I have to rethink how to remove already
    // scheduled events... (se)
    //  it should be fine -zecke
//    qWarning("Removing alarm for event with uid %d", uid );
    AlarmServer::deleteAlarm( schedule ,
                              "QPE/Application/todolist",
                              "alarm(QDateTime,int)", uid );
}
/* sort order */
OTodoAccess::List OTodoAccess::sorted( bool ascending, int sort,int filter, int cat ) {
    QArray<int> ints = m_todoBackEnd->sorted( ascending, sort,
                                      filter, cat );
    OTodoAccess::List list( ints, this );
    return list;
}
