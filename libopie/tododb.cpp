
#include <qdir.h>
#include <opie/tododb.h>
#include <opie/xmltree.h>
#include <opie/todoresource.h>
#include <qpe/palmtoprecord.h>
#include <qpe/global.h>

using namespace Opie;

namespace {

class FileToDoResource : public ToDoResource {
public:
  FileToDoResource() {};
    // FIXME better parsing
  bool save(const QString &name, const QValueList<ToDoEvent> &m_todos ){
    // prepare the XML
    XMLElement *tasks = new XMLElement( );
    tasks->setTagName("Tasks" );
    for( QValueList<ToDoEvent>::ConstIterator it = m_todos.begin(); it != m_todos.end(); ++it ){
	XMLElement::AttributeMap map;
	XMLElement *task = new XMLElement();
	map.insert( "Completed", QString::number((int)(*it).isCompleted() ) );
	map.insert( "HasDate", QString::number((int)(*it).hasDate() ) );
	map.insert( "Priority", QString::number( (*it).priority() ) );
        map.insert( "Summary", (*it).summary() );
	QArray<int> arrat = (*it).categories();
	QString attr;
	for(uint i=0; i < arrat.count(); i++ ){
	  attr.append(QString::number(arrat[i])+";" );
	}
	if(!attr.isEmpty() ) // remove the last ;
	  attr.remove(attr.length()-1, 1 );
	map.insert( "Categories", attr );
	//else
	//map.insert( "Categories", QString::null );
	map.insert( "Description", (*it).description() );
	if( (*it).hasDate() ){
	  map.insert("DateYear",  QString::number( (*it).date().year()  ) );
	  map.insert("DateMonth", QString::number( (*it).date().month() ) );
	  map.insert("DateDay", QString::number( (*it).date().day()  ) );
	}
	map.insert("Uid", QString::number( (*it).uid() ) );
	task->setTagName("Task" );
	task->setAttributes( map );
	tasks->appendChild(task);
    }
    QFile file( name);
    if( file.open(IO_WriteOnly ) ){
      QTextStream stream(&file );
      stream.setEncoding( QTextStream::UnicodeUTF8 );
      stream << "<!DOCTYPE Tasks>" << endl;
      tasks->save(stream );
      delete tasks;
      stream << "</Tasks>" << endl;
      file.close();
      return true;
    }
    return false;
  }
  QValueList<ToDoEvent> load( const QString &name ){
    qWarning("loading tododb" );
    QValueList<ToDoEvent> m_todos;
    XMLElement *root = XMLElement::load( name );
    if(root != 0l ){ // start parsing
      qWarning("ToDoDB::load tagName(): %s", root->tagName().latin1()  );
	//if( root->tagName() == QString::fromLatin1("Tasks" ) ){// Start
      XMLElement *element = root->firstChild();
      element = element->firstChild();
      while( element ){
	if( element->tagName() != QString::fromLatin1("Task") ){
	  element = element->nextChild();
	  continue;
	}
	qWarning("ToDoDB::load element tagName() : %s", element->tagName().latin1() );
	QString dummy;
	ToDoEvent event;
	bool ok;
	int dumInt;
	// completed
	dummy = element->attribute("Completed" );
	dumInt = dummy.toInt(&ok );
	if(ok ) event.setCompleted( dumInt == 0 ? false : true );
	// hasDate
	dummy = element->attribute("HasDate" );
	dumInt = dummy.toInt(&ok );
	if(ok ) event.setHasDate( dumInt == 0 ? false: true );
	// set the date
	bool hasDa = dumInt;
	if ( hasDa ) { //parse the date
	  int year, day, month = 0;
	  year = day = month;
	  // year
	  dummy = element->attribute("DateYear" );
	  dumInt = dummy.toInt(&ok );
	  if( ok ) year = dumInt;
	  // month
	  dummy = element->attribute("DateMonth" );
	  dumInt = dummy.toInt(&ok );
	  if(ok ) month = dumInt;
	  dummy = element->attribute("DateDay" );
	  dumInt = dummy.toInt(&ok );
	  if(ok ) day = dumInt;
	  // set the date
	  QDate date( year, month, day );
	  event.setDate( date);
	}
	dummy = element->attribute("Priority" );
	dumInt = dummy.toInt(&ok );
	if(!ok ) dumInt = ToDoEvent::NORMAL;
	event.setPriority( dumInt );
	//description
	dummy = element->attribute("Description" );
	event.setDescription( dummy );
        dummy = element->attribute("Summary" );
        event.setSummary( dummy );
	// category
	dummy = element->attribute("Categories" );
	QStringList ids = QStringList::split(";", dummy );
	event.setCategories( ids );

	//uid
	dummy = element->attribute("Uid" );
	dumInt = dummy.toInt(&ok );
	if(ok ) event.setUid( dumInt );
	m_todos.append( event );
	element = element->nextChild(); // next element
      }
      //}
    }else {
      qWarning("could not load" );
    }
    delete root;
    qWarning("returning" );
    return m_todos;
  }
};

}

ToDoDB::ToDoDB(const QString &fileName, ToDoResource *res ){
    m_fileName = fileName;
    if( fileName.isEmpty() && res == 0 ){
	m_fileName = Global::applicationFileName("todolist","todolist.xml");
	res = new FileToDoResource();
	//qWarning("%s", m_fileName.latin1() );
    }else if(res == 0 ){ // let's create a ToDoResource for xml
      res = new FileToDoResource();
    }
    m_res = res;
    load();
}
ToDoResource* ToDoDB::resource(){
  return m_res;
};
void ToDoDB::setResource( ToDoResource *res )
{
  delete m_res;
  m_res = res;
}
ToDoDB::~ToDoDB()
{
  delete m_res;
}
QValueList<ToDoEvent> ToDoDB::effectiveToDos(const QDate &from, const QDate &to,
					     bool all )
{
    QValueList<ToDoEvent> events;
    for( QValueList<ToDoEvent>::Iterator it = m_todos.begin(); it!= m_todos.end(); ++it ){
	if( (*it).hasDate() ){
	    if( (*it).date() >= from && (*it).date() <= to )
		events.append( (*it) );
	}else if( all ){
	    events.append( (*it) );
	}
    }
    return events;
}
QValueList<ToDoEvent> ToDoDB::effectiveToDos(const QDate &from,
					     bool all)
{
    return effectiveToDos( from, QDate::currentDate(), all );
}
QValueList<ToDoEvent> ToDoDB::overDue()
{
    QValueList<ToDoEvent> events;
    for( QValueList<ToDoEvent>::Iterator it = m_todos.begin(); it!= m_todos.end(); ++it ){
	if( (*it).isOverdue() )
	    events.append((*it) );
    }
    return events;
}
QValueList<ToDoEvent> ToDoDB::rawToDos()
{
    return m_todos;
}
void ToDoDB::addEvent( const ToDoEvent &event )
{
    m_todos.append( event );
}
void ToDoDB::editEvent( const ToDoEvent &event )
{
    m_todos.remove( event );
    m_todos.append( event );
}
void ToDoDB::removeEvent( const ToDoEvent &event )
{
    m_todos.remove( event );
}
void ToDoDB::replaceEvent(const ToDoEvent &event )
{
  QValueList<ToDoEvent>::Iterator it;
  int uid = event.uid();
  // == is not overloaded as we would like :( so let's search for the uid
  for(it = m_todos.begin(); it != m_todos.end(); ++it ){
    if( (*it).uid() == uid ){
      m_todos.remove( (*it) );
      break; // should save us the iterate is now borked
    }
  }
  m_todos.append(event);
}
void ToDoDB::reload()
{
    load();
}
void ToDoDB::mergeWith(const QValueList<ToDoEvent>& events )
{
  QValueList<ToDoEvent>::ConstIterator it;
  for( it = events.begin(); it != events.end(); ++it ){
    replaceEvent( (*it) );
  }
}
void ToDoDB::setFileName(const QString &file )
{
    m_fileName =file;
}
QString ToDoDB::fileName()const
{
    return m_fileName;
}
void ToDoDB::load()
{
  m_todos = m_res->load( m_fileName );
}
bool ToDoDB::save()
{
  return m_res->save( m_fileName, m_todos );
}








