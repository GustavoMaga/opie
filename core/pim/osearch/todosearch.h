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

/**
@author Patrick S. Vogt
*/
class TodoSearch : public SearchGroup 
{
public:
    TodoSearch(QListView* parent, QString name);

    ~TodoSearch();

    virtual void expand();
private:
    OTodoAccess *_todos;
};

#endif
