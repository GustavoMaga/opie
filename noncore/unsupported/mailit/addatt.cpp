/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#include <qlayout.h>
#include <qdir.h>
#include <qstringlist.h>
#include "resource.h"
#include "addatt.h"

FileItem::FileItem(QListView *parent, QFileInfo fileInfo, QString fileType)
		: QListViewItem(parent)
{
	file = fileInfo;
	type = fileType;

	setText(0, fileInfo.baseName());
	
	if (fileType == "Picture") {
		setPixmap(0, Resource::loadPixmap("pixmap"));
	} else if (fileType == "Document") {
		setPixmap(0, Resource::loadPixmap("txt"));	
	} else if (fileType == "Sound") {
		setPixmap(0, Resource::loadPixmap("play"));
	} else if (fileType == "Movie") {
		setPixmap(0, Resource::loadPixmap("MPEGPlayer"));
	} else if (fileType == "File") {
		setPixmap(0, Resource::loadPixmap("exec"));
	}
}

QFileInfo FileItem::getFileInfo()
{
	return file;
}

QString FileItem::getFileType()
{
	return type;
}

AddAtt::AddAtt(QWidget *parent, const char *name, WFlags f)
	: QDialog(parent, name, f)
{
	setCaption("Adding attatchments");

	QGridLayout *top = new QGridLayout(this, 3, 2);

	fileCategoryButton = new QPushButton(this);
	attatchButton = new QPushButton("Attatch ->", this);
	removeButton = new QPushButton("Remove", this);

	fileCategories = new QPopupMenu(fileCategoryButton);
	fileCategoryButton->setPopup(fileCategories);
	fileCategories->insertItem("Document");
	fileCategories->insertItem("Picture");
	fileCategories->insertItem("Sound");
	fileCategories->insertItem("Movie");
	fileCategories->insertItem("File");

	fileCategoryButton->setText("Document");
	top->addWidget(fileCategoryButton, 0, 0);
	top->addWidget(attatchButton, 2, 0);
	top->addWidget(removeButton, 2, 1);

	connect(fileCategories, SIGNAL(activated(int)), this,
    		SLOT(fileCategorySelected(int)) );
	connect(attatchButton, SIGNAL(clicked()), this,
		SLOT(addAttatchment()) );
	connect(removeButton, SIGNAL(clicked()), this,
		SLOT(removeAttatchment()) );
	
	listView = new QListView(this, "AttView");
	listView->addColumn("Documents");
	connect(listView, SIGNAL(doubleClicked(QListViewItem *)), this,
		SLOT(addAttatchment()) );
	
	attView = new QListView(this, "Selected");
	attView->addColumn("Attatched");
	connect(attView, SIGNAL(doubleClicked(QListViewItem *)), this,
		SLOT(removeAttatchment()) );

	top->addWidget(listView, 1,0);
	top->addWidget(attView, 1,1);
	
	clear();	
}

void AddAtt::clear()
{
	attView->clear();
	getFiles();
	modified = FALSE;
}

void AddAtt::fileCategorySelected(int id)
{
	fileCategoryButton->setText(fileCategories->text(id));
	getFiles();
}

void AddAtt::addAttatchment()
{
	QFileInfo info;
	QString type;
	
	if (listView->selectedItem() != NULL) {
		item = (FileItem *) listView->selectedItem();
		info = item->getFileInfo();
		type = item->getFileType();
		item = new FileItem(attView, info, type);
	}
	modified = TRUE;
}

void AddAtt::removeAttatchment()
{
	if (attView->selectedItem() != NULL) {
		attView->takeItem(attView->selectedItem());
	}
	modified = TRUE;
}

void AddAtt::reject()
{
	if (modified) {
		attView->clear();
		modified = FALSE;
	}
}

void AddAtt::accept()
{
	modified = FALSE;
	hide();
}

void AddAtt::getFiles()
{
	QString path, selected;
	QDir *dir;
	
	listView->clear();
	
	selected = fileCategoryButton->text();
	if (selected == "Picture") {
		path = "../pics/";
	} else if (selected == "Document") {
		path = ""			;					//sub-dirs not decided
	} else if (selected == "Sound") {
		path = "../sounds/";					//sub-dirs not decided
	} else if (selected == "Movie") {
		path = "";								//sub-dirs not decided
	} else if (selected == "File") {
		path = "";								//sub-dirs not decided
	}

	dir = new QDir(path);
	dir->setFilter(QDir::Files);
	const QFileInfoList *dirInfoList = dir->entryInfoList();

	QFileInfoListIterator it(*dirInfoList);      // create list iterator

	while ( (fi=it.current()) ) {           // for each file...
		item = new FileItem(listView, *fi, selected);
		++it;                               // goto next list element
	}
}

QStringList AddAtt::returnAttatchedFiles()
{
	QFileInfo info;
	QStringList list;
	
	item = (FileItem *) attView->firstChild();
	while (item != NULL) {
		info = item->getFileInfo();
		list += info.filePath();
		item = (FileItem *) item->nextSibling();
	}
	return list;
}

QStringList AddAtt::returnFileTypes()
{
	QStringList list;
	
	item = (FileItem *) attView->firstChild();
	while (item != NULL) {
		list += item->getFileType();
		item = (FileItem *) item->nextSibling();
	}
	return list;
}
