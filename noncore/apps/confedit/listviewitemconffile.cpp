/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>

#include "listviewitemconffile.h"
#include <qmessagebox.h>
#include <qtextstream.h>
#include <qstring.h>
#include "listviewitemconfigentry.h"

#define tr QObject::tr

ListViewItemConfFile::ListViewItemConfFile(QFileInfo *file, QListView *parent)
   : ListViewItemConf(parent), _valid(false)
{
  confFileInfo = file;
  parseFile();
  _changed = false;
  displayText();
}

ListViewItemConfFile::~ListViewItemConfFile()
{
}


void ListViewItemConfFile::displayText()
{
  setText(0,(_changed?"*":"*")+confFileInfo->fileName());	
}

QString ListViewItemConfFile::fileName()
{
	return confFileInfo->fileName();
}

void ListViewItemConfFile::parseFile()
{
	qDebug(  confFileInfo->absFilePath() );
  QFile confFile(confFileInfo->absFilePath());
	qDebug(  confFileInfo->absFilePath() );
 // QString fileName = confFileInfo->fileName();
	if(! confFile.open(IO_ReadOnly))
 		QMessageBox::critical(0,tr("Could not open"),tr("The file ")+confFileInfo->fileName()+tr(" could not be opened."),1,0);	
	QTextStream t( &confFile );
 	QString s;
  QString group;
  ListViewItemConfigEntry *groupItem;
  ListViewItemConfigEntry *item;
  while ( !t.atEnd() )
  {
  	s = t.readLine().stripWhiteSpace();
	//	qDebug( "line: >%s<\n", s.latin1() );
  	if (s.contains("<?xml"))
   	{
    	_valid = false;
			break;
    }else
    if ( s[0] == '[' && s[s.length()-1] == ']' )
   	{
      qDebug("got group"+s);
      group = s.mid(1,s.length()-2);
      groupItem = new ListViewItemConfigEntry(this, group );
      insertItem( groupItem );
    } else
    if ( int pos = s.find('=') )
    {
 //     qDebug("got key"+s);
    	item = new ListViewItemConfigEntry(this, group, s );
      groupItem->insertItem( item );
    }
  }	
	confFile.close();
	setExpandable( _valid );
}

void ListViewItemConfFile::save()
{
	qDebug("ListViewItemConfFile::save()");
	qDebug("no saveing yet...");
 	unchanged();
}
