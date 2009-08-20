/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

 // (c) 2002 Patrick S. Vogtp <tille@handhelds.org>

#ifndef LISTVIEWCONFDIR_H
#define LISTVIEWCONFDIR_H

#include <qwidget.h>
#include <qlistview.h>
#include <qdir.h>

class QDir;

class OListView : public QListView  {
   Q_OBJECT
public:
	OListView(QWidget *parent=0, const char *name=0);
	~OListView();
protected slots:
	void expand(QListViewItem*);
};

#endif
