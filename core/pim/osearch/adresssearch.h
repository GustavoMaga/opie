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
#ifndef ADRESSSEARCH_H
#define ADRESSSEARCH_H

#include "searchgroup.h"

class OContactAccess;

/**
@author Patrick S. Vogt
*/
class AdressSearch : public SearchGroup
{
public:
    AdressSearch(QListView* parent, QString name);
    ~AdressSearch();

protected:
	virtual void load();
	virtual int search();
	virtual void insertItem( void* );

private:
    OContactAccess *_contacts;
};

#endif
