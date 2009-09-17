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
#ifndef SEARCHGROUP_H
#define SEARCHGROUP_H

#include "olistviewitem.h"

class QRegExp;
class QPopupMenu;

/**
@author Patrick S. Vogt
*/
class SearchGroup : public OListViewItem
{
public:
    SearchGroup(QListView* parent, QString name);

    ~SearchGroup();
    int m_resultCount;

    virtual void expand();
    virtual void doSearch();
    virtual void setSearch(QRegExp);
    virtual int rtti() { return Searchgroup;}

protected:
    virtual void load() = 0;
    virtual int search() = 0;
    virtual void insertItem( void* ) = 0;
    QRegExp m_search;
    QRegExp m_lastSearch;
    QString m_name;
private:
    int realSearch();
    bool m_loaded;
};

#endif
