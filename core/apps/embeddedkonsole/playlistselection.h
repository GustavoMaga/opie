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
#ifndef PLAY_LIST_SELECTION_H
#define PLAY_LIST_SELECTION_H


#include <qlist.h>
#include <qlistview.h>
#include <qpe/applnk.h>

class PlayListSelectionItem : public QListViewItem {
public:
    PlayListSelectionItem( QListView *parent, QString *f ) : QListViewItem( parent ), fl( f ) {
  setText(0, *f );
    }

    ~PlayListSelectionItem() {
    };

    const QString *file() const { return fl; }

private:
    const QString *fl;
};

class PlayListSelection : public QListView {
    Q_OBJECT
public:
    PlayListSelection( QWidget *parent, const char *name=0 );
    ~PlayListSelection();

    const QString *current(); // retrieve the current playlist entry (media file link)

public slots:
    void addToSelection( QListViewItem * ); // Add a media file to the playlist
    void addStringToSelection ( const QString &);
    void removeSelected();   // Remove a media file from the playlist
    void moveSelectedUp();   // Move the media file up the playlist so it is played earlier
    void moveSelectedDown(); // Move the media file down the playlist so it is played later
    bool prev();
    bool next();
    bool first();
    bool last();

protected:
    virtual void contentsMouseMoveEvent(QMouseEvent *);


private:
    QStringList   selectedList;
    QString    *cmd;
};


#endif // PLAY_LIST_SELECTION_H


