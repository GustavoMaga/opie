//
//
// C++ Interface: $MODULE$
//
// Description:
//
//
// Author: Patrick S. Vogt <tille@handhelds.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TODOSEARCH_H
#define TODOSEARCH_H

#include "searchgroup.h"

class OTodoAccess;
class QAction;

/**
@author Patrick S. Vogt
*/
class TodoSearch : public SearchGroup
{
public:
    TodoSearch(QListView* parent, QString name);
    ~TodoSearch();

    virtual QPopupMenu* popupMenu();

protected:
	virtual void load();
	virtual int search();
	virtual void insertItem( void* );

private:
    OTodoAccess *_todos;
    QAction *actionShowCompleted;
    QPopupMenu *_popupMenu;
};

#endif
