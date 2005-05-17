/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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

#include "tableviewer.h"
#include "ui/tvbrowseview.h"
#include "ui/tvfilterview.h"
#include "ui/tvlistview.h"
#include "ui/tveditview.h"
#include "ui/tvkeyedit.h"
#include "db/datacache.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/oresource.h>
#include <qpe/fileselector.h>
using namespace Opie::Core;

/* QT */
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qpopupmenu.h>
#include <qapplication.h>
#include <qwidgetstack.h>
#include <qlayout.h>
#include <qbuffer.h>

/*!
    \class TableViewerWindow
    \brief The main window widget of the application

    This is the main widget of the table viewer application.  
    It is the co-ordination point.
*/

/*!
    Constructs a new TableViewerWindow
*/
TableViewerWindow::TableViewerWindow(QWidget *parent, const char *name, WFlags f)
    : QMainWindow(parent, name, f)
{
    setCaption(tr("Table Viewer"));

/* Build data */
    ds = new DBStore();
    doc.setType("text/x-xml-tableviewer");
    doc.setName("table");

    dirty = FALSE;
    ts.current_column = 0;
    ts.kRep = ds->getKeys();

/* build menus */
    menu = new QMenuBar(this, 0);

    QPopupMenu *file_menu = new QPopupMenu;
    file_menu->insertItem("New", this, SLOT(newDocument()));

    file_menu->insertItem("Open", this, SLOT(selectDocument()));
    file_menu->insertSeparator();
    file_menu->insertItem("Properties");

    /* later will want to set this up to clean up first via this, SLOT(quit) */
    menu->insertItem("Document", file_menu); 

    QPopupMenu *edit_menu = new QPopupMenu;
    edit_menu->insertItem("Edit Item", this, SLOT(editItemSlot()));
    edit_menu->insertItem("Edit Keys", this, SLOT(editKeysSlot()));
    edit_menu->insertItem("Edit filters", this, SLOT(filterViewSlot()));
    menu->insertItem("Edit", edit_menu);

    QPopupMenu *view_menu = new QPopupMenu;
    view_menu->insertItem("Browse View", this, SLOT(browseViewSlot()));
    view_menu->insertItem("List View", this, SLOT(listViewSlot()));
    menu->insertItem("View", view_menu); 

    QVBoxLayout *main_layout = new QVBoxLayout;

    /* Build tool bar */
    bool useBigIcon = qApp->desktop()->size().width() > 330;

    navigation = new QToolBar(this, "navigation");
    QToolButton *btn = new QToolButton( Opie::Core::OResource::loadPixmap("new", Opie::Core::OResource::SmallIcon),
                                        "New Item", QString::null, this, SLOT(newItemSlot()), navigation, "New Item");
    btn->setUsesBigPixmap( useBigIcon );
    btn = new QToolButton( Opie::Core::OResource::loadPixmap("edit", Opie::Core::OResource::SmallIcon),
                           "Edit Item", QString::null, this, SLOT(editItemSlot()), navigation, "Edit Item");
    btn->setUsesBigPixmap( useBigIcon );
    btn = new QToolButton( Opie::Core::OResource::loadPixmap("trash", Opie::Core::OResource::SmallIcon),
                           "Delete Item", QString::null, this, SLOT(deleteItemSlot()), navigation, "Delete Item");
    btn->setUsesBigPixmap( useBigIcon );

    navigation->addSeparator();

    btn = new QToolButton( Opie::Core::OResource::loadPixmap("fastback", Opie::Core::OResource::SmallIcon),
                           "First Item", QString::null, this, SLOT(firstItem()), navigation, "First Item");
    btn->setUsesBigPixmap( useBigIcon );
    btn = new QToolButton( Opie::Core::OResource::loadPixmap("back", Opie::Core::OResource::SmallIcon),
                           "Previous Item", QString::null, this, SLOT(previousItem()), navigation, "Previous Item");
    btn->setUsesBigPixmap( useBigIcon );
    btn = new QToolButton( Opie::Core::OResource::loadPixmap("forward", Opie::Core::OResource::SmallIcon),
                           "Next Item", QString::null, this, SLOT(nextItem()), navigation, "Next Item");
    btn->setUsesBigPixmap( useBigIcon );
    btn = new QToolButton( Opie::Core::OResource::loadPixmap("fastforward", Opie::Core::OResource::SmallIcon),
                           "Last Item", QString::null, this, SLOT(lastItem()), navigation, "Last Item");
    btn->setUsesBigPixmap( useBigIcon );

    navigation->addSeparator();
    btn = new QToolButton( Opie::Core::OResource::loadPixmap("day", Opie::Core::OResource::SmallIcon),
                           "View Single Item", QString::null, this, SLOT(browseViewSlot()), navigation, "View Single Item");
    btn->setUsesBigPixmap( useBigIcon );
    btn = new QToolButton( Opie::Core::OResource::loadPixmap("month", Opie::Core::OResource::SmallIcon),
                           "View Multiple Items", QString::null, this, SLOT(listViewSlot()), navigation, "View Multiple Items");
    btn->setUsesBigPixmap( useBigIcon );

    setToolBarsMovable(FALSE);

/* Build widgets */
    browseView = new TVBrowseView(&ts, this, 0);
    listView = new TVListView(&ts, this, 0);
    filterView = new TVFilterView(&ts, this, 0);
    fileSelector = new FileSelector("text/csv;text/x-xml-tableviewer", 
                this, "fileselector");
    fileSelector->setNewVisible(FALSE);
    fileSelector->setCloseVisible(FALSE);

    cw = new QWidgetStack(this, 0);
    cw->addWidget(listView, ListState);
    cw->addWidget(browseView, BrowseState);
    cw->addWidget(filterView, FilterState);
    cw->addWidget(fileSelector, FileState);

    current_view = FileState;
    cw->raiseWidget(current_view);
    fileSelector->reread();

    connect(browseView, SIGNAL(searchOnKey(int,TVVariant)), 
            this, SLOT(searchOnKey(int,TVVariant)));
    connect(browseView, SIGNAL(sortChanged(int)), 
            this, SLOT(setPrimaryKey(int)));

    connect(fileSelector, SIGNAL(closeMe()), this, SLOT(browseViewSlot()));
    connect(fileSelector, SIGNAL(fileSelected(const DocLnk&)), 
             this, SLOT(openDocument(const DocLnk&)));

    main_layout->addWidget(menu);
    main_layout->addWidget(cw);

    setCentralWidget(cw);

}

/*!
    Destroys the TableViewerWindow
*/
TableViewerWindow::~TableViewerWindow()
{
    if(dirty)
      saveDocument();
}

/*!
    Opens a file dialog and loads the file specified by the dialog
*/
void TableViewerWindow::selectDocument()
{
    if(dirty)
  saveDocument();
    current_view = FileState;
    cw->raiseWidget(current_view);
    fileSelector->reread();
}

void TableViewerWindow::saveDocument()
{
    if(!dirty)
  return;

    FileManager fm;
    QIODevice *dev = fm.saveFile(doc);

    if(!ds->saveSource(dev, doc.type())){
  owarn << "Save unsuccessful" << oendl; 
  return;
    }
    dev->close();
    dirty = FALSE;
}

void TableViewerWindow::newDocument()
{
    DocLnk nf;
    nf.setType("text/x-xml-tableviewer");
    nf.setName("table");
   
    delete ds;
    ds = new DBStore();
   
    ts.current_column = 0;
    ts.kRep = ds->getKeys();
    browseView->reset();
    listView->reset();
    filterView->reset();

    doc = nf;
    dirty = FALSE;

    current_view = BrowseState;
    cw->raiseWidget(current_view);

  /* now set up for editing the keys */
  ts.kRep->addKey("key", TVVariant::String);
  editKeysSlot();
}

void TableViewerWindow::setDocument(const QString &f)
{
    openDocument(DocLnk(f, TRUE));
}

void TableViewerWindow::openDocument(const DocLnk &f)
{

    if (!f.isValid()) 
  return;

    FileManager fm;
    QIODevice *dev = fm.openFile(f);
    doc = f;

    if(ds->openSource(dev, doc.type())) {
        DataElem *d;

        browseView->reset();
        listView->reset();
        filterView->reset();

  current_view = BrowseState;
  cw->raiseWidget(current_view);

        /* set up new table state and ensure sub widgets have a reference */
        ts.current_column = 0;
        ts.kRep = ds->getKeys();
        browseView->rebuildKeys();
        listView->rebuildKeys();
        filterView->rebuildKeys();

        ds->first();
        /* set up the list view */
        listView->clearItems();
        do {
            d = ds->getCurrentData();
            if(d) 
                listView->addItem(d);
        } while(ds->next());

        /* Set up browse view, Will be based of structure of listView */
        listView->first();
        ts.current_elem = listView->getCurrentData();
        browseView->rebuildData();
        listView->rebuildData();

        QString scratch = "Table Viewer";/* later take from constant */
        scratch += " - ";
        scratch += ds->getName();
        setCaption(tr(scratch));

  dirty = FALSE;
    } else {
        owarn << tr("could not load Document") << oendl; 
    }
    dev->close();
}

/*! 
    Moves to the first item of the current table 
*/
void TableViewerWindow::firstItem()
{
    listView->first();
    ts.current_elem = listView->getCurrentData();
    browseView->rebuildData();
}

/*! 
    Moves to the lat item of the current table 
*/
void TableViewerWindow::lastItem()
{
    listView->last();
    ts.current_elem = listView->getCurrentData();
    browseView->rebuildData();
}

/*! 
    Moves to the next item of the current table 
*/
void TableViewerWindow::nextItem()
{
    listView->next();
    ts.current_elem = listView->getCurrentData();
    browseView->rebuildData();
}

/*! 
    Moves to the previous item of the current table 
*/
void TableViewerWindow::previousItem()
{
    listView->previous();
    ts.current_elem = listView->getCurrentData();
    browseView->rebuildData();
}

/*!
    Raises the List View.  This is a mode change for the application.
*/
void TableViewerWindow::listViewSlot()
{
    if(current_view == FilterState)
        applyFilter();
    current_view = ListState;
    cw->raiseWidget(current_view);
}

void TableViewerWindow::applyFilter()
{
    DataElem *d;

    listView->clearItems();
    ds->first();
    do {
        d = ds->getCurrentData();
        if(d) 
            if(filterView->passesFilter(d))
    listView->addItem(d);
    } while(ds->next());
    listView->first();
    listView->rebuildData();
}

/*!
    Raises the Browse View.  This is a mode change for the application.
*/
void TableViewerWindow::browseViewSlot()
{
    if(current_view == FilterState)
        applyFilter();

    ts.current_elem = listView->getCurrentData();
    browseView->rebuildData();

    current_view = BrowseState;
    cw->raiseWidget(current_view);
}

/*!
    Raises the List View.  This is a mode change for the application.
*/
void TableViewerWindow::filterViewSlot()
{
    current_view = FilterState;
    cw->raiseWidget(current_view);
}




void TableViewerWindow::editItemSlot()
{
    if(TVEditView::openEditItemDialog(&ts, ts.current_elem, this)) {
  listView->rebuildData();
  browseView->rebuildData();
  dirty = TRUE;
    }
}

void TableViewerWindow::newItemSlot()
{
    DataElem *d = new DataElem(ds);
    if (TVEditView::openEditItemDialog(&ts, d, this)) {

  ds->addItem(d);
  ts.current_elem = d;
  applyFilter();
  listView->rebuildData();
  browseView->rebuildData();
  dirty = TRUE;
    }
}

void TableViewerWindow::deleteItemSlot()
{
  /* delete the actual item, then do a 'filter' */
  DataElem *to_remove = ts.current_elem;

  if(!to_remove) 
    return;

    listView->removeItem();
  ds->removeItem(to_remove);
    
    applyFilter();
    listView->rebuildData();
    browseView->rebuildData();
    dirty = TRUE;
}

void TableViewerWindow::editKeysSlot()
{
    DataElem *d;
    KeyList *k = TVKeyEdit::openEditKeysDialog(&ts, this);

    if(k) {
        /* set as new keys */
        ds->setKeys(k);

        ts.current_column = 0;
        ts.kRep = k;
    
        browseView->reset();
        listView->reset();
        filterView->reset();

        browseView->rebuildKeys();
        listView->rebuildKeys();
        filterView->rebuildKeys();

        ds->first();
        /* set up the list view */
        listView->clearItems();
        do {
            d = ds->getCurrentData();
            if(d) 
                listView->addItem(d);
        } while(ds->next());

        /* Set up browse view, Will be based of structure of listView */
  dirty = TRUE;
    }
}

/*! 
    A Slot that allows for widgets above to indicate a search should be 
    done on a specified key index for a specified value 
*/
void TableViewerWindow::searchOnKey(int i, TVVariant v)
{
    listView->findItem(i, v);
    ts.current_elem = listView->getCurrentData();
    browseView->rebuildData();
}

void TableViewerWindow::setPrimaryKey(int i)
{
    ts.current_column = i;
    listView->rebuildData();
    browseView->rebuildData();
}
