/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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
#include "playlistselection.h"

/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;

/* QT */
#include <qheader.h>

/* STD */
#include <stdlib.h>

class PlayListSelectionItem : public QListViewItem {
public:
    PlayListSelectionItem( QListView *parent, const DocLnk *f ) : QListViewItem( parent ), fl( f ) {
  setText( 0, f->name() );
  setPixmap( 0, f->pixmap() );
    }

    ~PlayListSelectionItem() {
    };

    const DocLnk *file() const { return fl; }

private:
    const DocLnk *fl;
};


PlayListSelection::PlayListSelection( QWidget *parent, const char *name )
    : QListView( parent, name )
{
//    odebug << "starting playlistselector" << oendl; 
// #ifdef USE_PLAYLIST_BACKGROUND
//    setStaticBackground( TRUE );
//      setBackgroundPixmap( Resource::loadPixmap( "opieplayer/background" ) );

//      setBackgroundPixmap( Resource::loadPixmap( "launcher/opielogo" ) );
// #endif
//      addColumn("Title",236);
//      setAllColumnsShowFocus( TRUE );
     addColumn( tr( "Playlist Selection" ) );
    header()->hide();
    setSorting( -1, FALSE );
}


PlayListSelection::~PlayListSelection() {
}


// #ifdef USE_PLAYLIST_BACKGROUND
void PlayListSelection::drawBackground( QPainter *p, const QRect &r ) {
//  odebug << "drawBackground" << oendl; 
   p->fillRect( r, QBrush( white ) );
//        QImage logo = Resource::loadImage( "launcher/opielogo" );
//          if ( !logo.isNull() )
//        p->drawImage( (width() - logo.width()) / 2, (height() - logo.height()) / 2, logo );
}
// #endif


void PlayListSelection::contentsMouseMoveEvent( QMouseEvent *event ) {
    if ( event->state() == QMouseEvent::LeftButton ) {
  QListViewItem *currentItem = selectedItem();
  QListViewItem *itemUnder = itemAt( QPoint( event->pos().x(), event->pos().y() - contentsY() ) );
  if ( currentItem && currentItem->itemAbove() == itemUnder )
      moveSelectedUp();
  else if ( currentItem && currentItem->itemBelow() == itemUnder )
      moveSelectedDown();
    }
}


const DocLnk *PlayListSelection::current() {
    PlayListSelectionItem *item = (PlayListSelectionItem *)selectedItem();
    if ( item )
  return item->file();
    return NULL;
}


void PlayListSelection::addToSelection( const DocLnk &lnk ) {
    PlayListSelectionItem *item = new PlayListSelectionItem( this, new DocLnk( lnk ) );
    QListViewItem *current = selectedItem();
    if ( current )
        item->moveItem( current );
    setSelected( item, TRUE );
    ensureItemVisible( selectedItem() );
}


void PlayListSelection::removeSelected() {
    QListViewItem *item = selectedItem();
    delete item;
    setSelected( currentItem(), TRUE );
    ensureItemVisible( selectedItem() );
}


void PlayListSelection::moveSelectedUp() {
    QListViewItem *item = selectedItem();
    if ( item && item->itemAbove() )
  item->itemAbove()->moveItem( item );
    ensureItemVisible( selectedItem() );
}


void PlayListSelection::moveSelectedDown() {
    QListViewItem *item = selectedItem();
    if ( item && item->itemBelow() )
        item->moveItem( item->itemBelow() );
    ensureItemVisible( selectedItem() );
}


bool PlayListSelection::prev() {
    QListViewItem *item = selectedItem();
    if ( item && item->itemAbove() )
        setSelected( item->itemAbove(), TRUE );
    else
  return FALSE;
    ensureItemVisible( selectedItem() );
    return TRUE;
}

bool PlayListSelection::next() {
    QListViewItem *item = selectedItem();
    if ( item && item->itemBelow() )
        setSelected( item->itemBelow(), TRUE );
    else
  return FALSE;
    ensureItemVisible( selectedItem() );
    return TRUE;
}


bool PlayListSelection::first() {
    QListViewItem *item = firstChild();
    if ( item )
        setSelected( item, TRUE );
    else
  return FALSE;
    ensureItemVisible( selectedItem() );
    return TRUE;
}


bool PlayListSelection::last() {
    QListViewItem *prevItem = NULL;
    QListViewItem *item = firstChild();
    while ( ( item = item->nextSibling() ) )
  prevItem = item;
    if ( prevItem )
        setSelected( prevItem, TRUE );
    else
  return FALSE;
    ensureItemVisible( selectedItem() );
    return TRUE;
}

void PlayListSelection::unSelect()
{
    //QListViewItem *item = selectedItem();
    setSelected( currentItem(), FALSE);
}

void PlayListSelection::writeCurrent( Config& cfg ) {
    cfg.setGroup("PlayList");
    QListViewItem *item = selectedItem();
    if ( item )
        cfg.writeEntry("current", item->text(0) );
    odebug << item->text(0) << oendl; 

}

void  PlayListSelection::setSelectedItem(const QString &strk ) {

    unSelect();
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it ) {
//        odebug << it.current()->text(0) << oendl; 
        if( strk == it.current()->text(0)) {
//             odebug << "We have a match "+strk << oendl; 
            setSelected( it.current(), TRUE);
            ensureItemVisible( it.current() );
            return;
        }
    }
//     setSelected( item, TRUE );
//     ensureItemVisible( selectedItem() );
}
