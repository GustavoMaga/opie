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
#ifndef TODOITEM_H
#define TODOITEM_H

#include "resultitem.h"
class OTodo;

/**
@author Patrick S. Vogt
*/
class TodoItem : public ResultItem
{
public:
    TodoItem(OListViewItem* parent, OTodo *todo);
    ~TodoItem();

    virtual void expand();
    virtual QString toRichText();
    virtual void showItem();
    virtual void editItem();

private:
    OTodo *_todo;

};

#endif
