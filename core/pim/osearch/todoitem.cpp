//
//
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: Patrick S. Vogt <tille@handhelds.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "todoitem.h"

#include <opie/otodo.h>
#include <qpe/qcopenvelope_qws.h>

TodoItem::TodoItem(OListViewItem* parent, OTodo *todo)
: ResultItem(parent)
{
	_todo = todo;
	setText( 0, todo->toShortText() );
}

TodoItem::~TodoItem()
{
	delete _todo;
}

QString TodoItem::toRichText()
{
	return _todo->toRichText();
}

void TodoItem::action( int act )
{
	if (act == 0){
		QCopEnvelope e("QPE/Application/todolist", "show(int)");
		e << _todo->uid();
	}else if (act == 1){
		QCopEnvelope e("QPE/Application/todolist", "edit(int)");
		e << _todo->uid();
	}
}

QIntDict<QString> TodoItem::actions()
{
	QIntDict<QString> result;
	result.insert( 0, new QString( QObject::tr("show") ) );
	result.insert( 1, new QString( QObject::tr("edit") ) );
	return result;
}
